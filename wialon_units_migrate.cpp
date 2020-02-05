// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#include <iostream>
#include <filesystem>
#include <string>
#include <iostream>
#include <clocale>
#include "CWLUnit.h"
#include "CCmdArgs.h"
#include "pugixml.hpp"

using namespace pugi;
namespace fs = std::filesystem;


static bool isDirExist(const std::string& dirPath);
static bool isFileExist(const std::string& fileName);
static bool processXmlFile(const std::string& xmlFilename, const std::string& wlpFilename);

static void addGeneral(const xml_node& node, CWLUnit& wlp);
static void addCustomFields(const xml_node& node, CWLUnit& wlp);
static void addCounters(const xml_node& node, CWLUnit& wlp);
static void addSensor(const xml_node& node, CWLUnit& wlp, const xml_node& fuelConfigMathNode = xml_node());
static void addTripDetector(const xml_node& node, CWLUnit& wlp);
static void addFuelConsumpt(const xml_node& node, CWLUnit& wlp);

static size_t convertPath(const std::string& inDirPath, const std::string& outDirPath, const bool overwrite = false);
static bool convertSingleFile(const std::string& inFileName, const std::string& outDirPath, const bool overwrite = false);

static void printHelp();

//int main()
int main(int argc, char* argv[], char* envp[])
{
    //Set correct russian characters output
    setlocale(LC_ALL, "");
    CCmdArgs args(argc, argv);

    if (args.isArgExist("--help") || args.isArgExist("-h")) {
        printHelp();
        return 0;
    }

    if (args.isArgExist("--ipath") && args.isArgExist("--ifile")) {
        std::cout << "You must specify only one parameter of --ipath or --ifile" << std::endl;
        return -1;
    }

    if (!args.isArgExist("--opath")) {
        std::cout << "You must specity ouput directory path" << std::endl;
        return -1;
    }

    std::string inFileName = args.argValue("--ifile");
    std::string inDirPath = args.argValue("--ipath");
    std::string outDirPath = args.argValue("--opath");
    bool overwrite = false;

    if (args.isArgExist("--overwrite")) {
        overwrite = true;
    }

    if (!inFileName.empty()) {
        //std::cout << "In filename " << inFileName << std::endl;

        if (!isFileExist(inFileName)) {
            std::cout << "Input file not exists" << std::endl;
            return -1;
        }
    }

    if (!inDirPath.empty()) {
        //std::cout << "In dir path " << inDirPath << std::endl;

        if (!isDirExist(inDirPath)) {
            std::cout << "Input dir path is not valid" << std::endl;
            return -1;
        }
    }

    if (!outDirPath.empty()) {
        //std::cout << "Out dir path " << outDirPath << std::endl;

        if (!isDirExist(outDirPath)) {
            std::cout << "Output dir path is not valid" << std::endl;
            return -1;
        }
    }

    //Convert single xml file
    if (!inFileName.empty()) {
        return (convertSingleFile(inFileName, outDirPath, overwrite)) ? 0 : -1;
    }

    //Convert files in path
    if (!inDirPath.empty()) {
        size_t successConvert = convertPath(inDirPath, outDirPath, overwrite);

        std::cout << std::endl << "Complete " << successConvert << " files" << std::endl;

        return (successConvert != 0) ? 0 : -1;
    }
}

static bool isDirExist(const std::string& dirPath)
{
    fs::path pathToCheck(dirPath);
    std::error_code error;
    if (fs::exists(pathToCheck, error) && fs::is_directory(pathToCheck, error)) {
        return true;
    } 

    return false;
}

static bool isFileExist(const std::string& fileName)
{
    fs::path pathToCheck(fileName);
    std::error_code error;

    if (fs::exists(pathToCheck, error) && fs::is_regular_file(pathToCheck, error)) {
        return true;
    }

    return false;
}

static bool processXmlFile(const std::string& xmlFilename, const std::string& wlpFilename)
{
    if (xmlFilename.empty() || wlpFilename.empty()) {
        return false;
    }

    xml_document doc;
    CWLUnit wlp;
    
    if (!doc.load_file(xmlFilename.c_str())) {
        return false;
    }

    xml_node device = doc.child("devices").child("device");

    if (device.empty()) {
        return false;
    }

    //General settings
    addGeneral(device, wlp);

    xml_node node = device.child("custom");

    //Custom fields
    addCustomFields(node, wlp);

    //Counters
    node = device.child("counters");
    addCounters(node, wlp);
    

    //Sensors
    node = device.child("sensors");
    for (auto sensor = node.child("sensor"); sensor; sensor = sensor.next_sibling("sensor")) {
        addSensor(sensor, wlp, device.child("fuelc").child("math"));
    }

    //Trip detector
    node = device.child("tripd");
    if (node) {
        addTripDetector(node, wlp);
    }

    //Fiel consumption
    node = device.child("fuelc");
    if (node) {
        addFuelConsumpt(node, wlp);
    }

    if (!wlp.wlpSave(wlpFilename)) {
        return false;
    }

    return true;
}

static void addGeneral(const xml_node& node, CWLUnit& wlp)
{
    wlpGeneral general;
    general.name = node.attribute("name").value();
    general.phone1 = node.attribute("phone").value();
    general.hw = node.attribute("type").value();
    general.uid = node.attribute("unique_id").value();
    wlp.addGeneral(general);
}

static void addCustomFields(const xml_node& node, CWLUnit& wlp)
{
    for (auto customField = node.child("field"); customField; customField = customField.next_sibling("field")) {
        wlp.addAfield(customField.attribute("name").value(), customField.attribute("value").value());
    }
}

static void addCounters(const xml_node& node, CWLUnit& wlp)
{
    for (auto counter = node.first_child(); counter; counter = counter.next_sibling()) {
        std::string counterName = counter.name();

        if (counterName == "engine_hours") {
            wlp.addCounterEngineHours(
                counter.attribute("value").as_double(),
                counter.attribute("auto").as_bool(),
                counter.attribute("type").value()
            );
        }
        else if (counterName == "mileage") {
            wlp.addCounterMileage(
                counter.attribute("value").as_double(),
                counter.attribute("auto").as_bool(),
                counter.attribute("type").value()
            );
        }
        else if (counterName == "bytes") {

            wlp.addCounterBytes(
                counter.attribute("value").as_double() / 1024.0,
                counter.attribute("auto").as_bool()
            );
        }
    }
}

static void addSensor(const xml_node& node, CWLUnit& wlp, const xml_node& fuelConfigMathNode)
{
    wlpSensor wlp_sensor;
    wlp.createDefaultSensorConfig(wlp_sensor.config);

    wlp_sensor.description = node.attribute("description").value();

    std::string stmp = node.attribute("flags").value();
    wlp_sensor.flags = std::stoul(stmp, nullptr, 16);

    wlp_sensor.id = node.attribute("id").as_int();
    wlp_sensor.unit = node.attribute("unit").value();
    wlp_sensor.name = node.attribute("name").value();
    wlp_sensor.parameter = node.attribute("parameter").value();
    wlp_sensor.type = node.attribute("type").value();
    wlp_sensor.validation_sensor_id = node.attribute("validation_sensor_id").as_int();
    wlp_sensor.validation_type = node.attribute("validation_type").as_int();

    if (wlp_sensor.type == "engine operation") { //add consumption to sensor config
        wlp_sensor.config.consumption = fuelConfigMathNode.attribute("idle").as_double(2.0);
    }

    //Calibration table
    auto ctable = node.child("ctable");
    wlp_sensor.table.clear();
    for (auto row = ctable.child("row"); row; row = row.next_sibling("row")) {
        wlpSensorCalibRow cRow = { 0 };
        cRow.a = row.attribute("a").as_double();
        cRow.b = row.attribute("b").as_double();
        cRow.x = row.attribute("x").as_double();

        wlp_sensor.table.push_back(cRow);
    }

    wlp.addSensor(wlp_sensor);
}

static void addTripDetector(const xml_node& node, CWLUnit& wlp)
{
    wlpTripDetector tripd = { 0 };
    tripd.type = node.attribute("type").as_uint(1UL);
    tripd.gpsCorrection = (node.attribute("gps_correction").as_bool(true)) ? 1 : 0;
    tripd.minSat = node.attribute("satellites").as_uint(4UL);
    tripd.minMovingSpeed = node.attribute("moving_speed").as_uint(1UL);
    tripd.minStayTime = node.attribute("stay_time").as_uint(300UL);
    tripd.maxMessagesDistance = node.attribute("message_distance").as_uint(1000UL);
    tripd.minTripTime = node.attribute("trip_time").as_uint(60UL);
    tripd.minTripDistance = node.attribute("trip_distance").as_uint(100UL);

    wlp.addTripDetector(tripd);
}

static void addFuelConsumpt(const xml_node& node, CWLUnit& wlp)
{
    wlpFuelConsumption fuelCons = { 0 };

    for (auto fuelRow = node.first_child(); fuelRow; fuelRow = fuelRow.next_sibling()) {
        std::string stmp = fuelRow.name();

        if (stmp == "absolute") {
            if (fuelRow.attribute("active").as_bool(false)) {
                fuelCons.calcTypes |= 0x08UL;
            }
        }
        else if (stmp == "impulse") {
            if (fuelRow.attribute("active").as_bool(false)) {
                fuelCons.calcTypes |= 0x10UL;
            }

            fuelCons.fuelConsImpulse.maxImpulses = fuelRow.attribute("impulses").as_uint();
            fuelCons.fuelConsImpulse.skipZero = (fuelRow.attribute("skip_first_zero").as_bool()) ? 1 : 0;
        }
        else if (stmp == "instant") {
            if (fuelRow.attribute("active").as_bool(false)) {
                fuelCons.calcTypes |= 0x20UL;
            }
        }
        else if (stmp == "level") {
            if (fuelRow.attribute("active").as_bool(false)) {
                fuelCons.calcTypes |= 0x02UL;
            }

            if (fuelRow.attribute("correct_invalid_values").as_bool(false)) {
                fuelCons.calcTypes |= 0x04UL;
            }
        }
        else if (stmp == "math") {
            if (fuelRow.attribute("active").as_bool(false)) {
                fuelCons.calcTypes |= 0x01UL;
            }

            fuelCons.fuelConsMath.idling = fuelRow.attribute("idle").as_double(2.0);
            fuelCons.fuelConsMath.urban = fuelRow.attribute("urban").as_double(10.0);
            fuelCons.fuelConsMath.suburban = fuelRow.attribute("suburban").as_double(7.0);
        }
        else if (stmp == "rates") {
            if (fuelRow.attribute("active").as_bool(false)) {
                fuelCons.calcTypes |= 0x40UL;
            }

            fuelCons.fuelConsRates.consSummer = fuelRow.attribute("summer_consumption").as_double(10.0);
            fuelCons.fuelConsRates.consWinter = fuelRow.attribute("winter_consumption").as_double(12.0);
            fuelCons.fuelConsRates.winterMonthFrom = fuelRow.attribute("winter_begin_month").as_uint(11);
            fuelCons.fuelConsRates.winterDayFrom = fuelRow.attribute("winter_begin_day").as_uint(1);
            fuelCons.fuelConsRates.winterMonthTo = fuelRow.attribute("winter_end_month").as_uint(1);
            fuelCons.fuelConsRates.winterDayTo = fuelRow.attribute("winter_end_day").as_uint(30);
        }
        else if (stmp == "general") {
            if (fuelRow.attribute("filter_level_values").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x02UL;
            }

            if (fuelRow.attribute("ignore_filtration_when_calc_filling_volume").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x40UL;
            }

            if (fuelRow.attribute("ignore_filtration_when_calc_theft_volume").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x80UL;
            }

            if (fuelRow.attribute("merge_consumption_sensors").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x04UL;
            }

            if (fuelRow.attribute("merge_level_sensors").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x01UL;
            }

            if (fuelRow.attribute("time_based_calculation_of_consumption").as_bool(false)) {
                //fuelSettings.fuelLevelParams.flags |= 0x10UL;
                fuelCons.fuelLevelParams.flags |= 0x800UL;
            }

            if (fuelRow.attribute("time_based_calculation_of_fillings").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x200UL;
            }

            if (fuelRow.attribute("time_based_calculation_of_thefts").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x400UL;
            }

            fuelCons.fuelLevelParams.filterQuality = fuelRow.attribute("filter_quality").as_uint() & 0xff;
        }
        else if (stmp == "theft") {
            if (fuelRow.attribute("detect_fill_when_stopped_only").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x08UL;
            }

            if (fuelRow.attribute("detect_theft_in_motion").as_bool(false)) {
                fuelCons.fuelLevelParams.flags |= 0x100UL;
            }

            if (fuelRow.attribute("ignore_filtration").as_bool(false)) {
                //fuelSettings.fuelLevelParams.flags |= 0x100UL;
            }

            fuelCons.fuelLevelParams.minFillingVolume = fuelRow.attribute("fill_volume").as_double(10.0);
            fuelCons.fuelLevelParams.ignoreStayTimeout = fuelRow.attribute("ignore_stay_timeout").as_uint(20);
            fuelCons.fuelLevelParams.minTheftTimeout = fuelRow.attribute("theft_timeout").as_uint(0);
            fuelCons.fuelLevelParams.minTheftVolume = fuelRow.attribute("theft_volume").as_double(10.0);

            //Default values
            fuelCons.fuelLevelParams.fillingsJoinInterval = 300;
            fuelCons.fuelLevelParams.theftsJoinInterval = 300;
            fuelCons.fuelLevelParams.extraFillingTimeout = 0;
        }
    }

    wlp.addFuelSettings(fuelCons);
}

static size_t convertPath(const std::string& inDirPath, const std::string& outDirPath, const bool overwrite)
{
    fs::path inPath(inDirPath);
    fs::path outPath(outDirPath);

    size_t fileCnt = 0;

    for (auto& p : fs::directory_iterator(inDirPath)) {
        fs::path file(p);

        if ((file.extension() == ".xml" || file.extension() == ".XML") &&
            isFileExist(file.string())) {
            if (convertSingleFile(file.string(), outDirPath, overwrite)) {
                ++fileCnt;
            }
        }
    }

    return fileCnt;
}

static bool convertSingleFile(const std::string& inFileName, const std::string& outDirPath, const bool overwrite)
{
    fs::path inPath(inFileName);
    fs::path outPath(outDirPath);

    if (inPath.extension() != ".xml" && inPath.extension() != ".XML") {
        std::cout << "Input file is not xml" << std::endl;
        return false;
    }

    outPath /= inPath.filename();
    outPath.replace_extension(".wlp");

    if (!overwrite && isFileExist(outPath.string())) {
        std::cout << "File " << outPath.filename() << " already exists. Skip" << std::endl;
        return false;
    }

    std::cout << "Convert " << inPath.filename() << "... ";

    if (processXmlFile(inFileName, outPath.string())) {
        std::cout << "Success" << std::endl;
        return true;
    }
    else {
        std::cout << "Error" << std::endl;
        return false;
    }
}

static void printHelp()
{
    std::cout << "Migrate units created in Wialon Pro to Wialon Local" << std::endl;
    std::cout <<
        "Usage: wialon_units_migrate.exe [--help | -h]" << std::endl <<
        "                                [--ipath=<path>] [--ifile=<filepath>]" << std::endl << 
        "                                [--opath = <path>] [--overwrite] " << std::endl;

}