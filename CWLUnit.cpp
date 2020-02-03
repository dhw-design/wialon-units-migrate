// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#include "CWLUnit.h"
#include <fstream> 
#include <string>


CWLUnit::CWLUnit()
{
	wlpInit();
}

CWLUnit::CWLUnit(const CWLUnit& wlobject)
{
	m_wlp.clear();

	m_wlp = wlobject.m_wlp;
}

CWLUnit::~CWLUnit()
{
}

void CWLUnit::addAfield(const std::string& name, const std::string& value)
{
	json newVal = json::object();

	if (m_wlp["afields"].size() == 0) {
		m_wlp["afields"] = json::array();
	}

	newVal["id"] = m_wlp["afields"].size() + 1;
	newVal["n"] = name;
	newVal["v"] = value;

	m_wlp["afields"].push_back(newVal);
}

void CWLUnit::addGeneral(const wlpGeneral& general)
{
	/*if (!m_wlp.contains("general")) {
		m_wlp["general"] = json::object();
	}*/

	m_wlp["general"] = json::object();

	m_wlp["general"] += json::object_t::value_type("n", general.name);
	m_wlp["general"] += json::object_t::value_type("ph", general.phone1);
	m_wlp["general"] += json::object_t::value_type("ph2", general.phone2);
	m_wlp["general"] += json::object_t::value_type("hw", general.hw);
	m_wlp["general"] += json::object_t::value_type("uid", general.uid);
	m_wlp["general"] += json::object_t::value_type("psw", general.password);
	m_wlp["general"] += json::object_t::value_type("uid2", "");
	/*json newVal = json::object();
	newVal[name] = value;*/

	//m_wlp["general"] += json::object_t::value_type(name, value);
}

void CWLUnit::addSensor(const wlpSensor& sensor)
{
	json newVal = json::object();

	if (m_wlp["sensors"].size() == 0) {
		m_wlp["sensors"] = json::array();
	}

	newVal["d"] = sensor.description;
	newVal["f"] = sensor.flags;
	newVal["id"] = sensor.id;
	newVal["m"] = sensor.unit;
	newVal["n"] = sensor.name;
	newVal["p"] = sensor.parameter;
	newVal["t"] = sensor.type;
	newVal["vs"] = sensor.validation_sensor_id;
	newVal["vt"] = sensor.validation_type;

	//std::string common = "{\"act\":1,\"appear_in_popup\":true,\"ci\":{},\"cm\":1,\"mu\":\"0\",\"pos\":";// 0, \"show_time\":false,\"timeout\":0,\"uct\":0,\"unbound_code\":\"\",\"validate_driver_unbound\":0}";
	//common += std::to_string(m_wlp["sensors"].size() + 1);
	//common += ", \"show_time\":false,\"timeout\":0,\"uct\":0,\"unbound_code\":\"\",\"validate_driver_unbound\":0}";
	//newVal["c"] = common;
	newVal["c"] = createSensorConfig(sensor.config, sensor.type, m_wlp["sensors"].size() + 1);
	
//	if (!sensor.table.empty()) {
		json table = json::array();
		for (auto rowIt = sensor.table.cbegin(); rowIt != sensor.table.cend(); ++rowIt) {
			json row = json::object();

			row["a"] = (*rowIt).a;
			row["b"] = (*rowIt).b;
			row["x"] = (*rowIt).x;

			table.push_back(row);
		}

		newVal["tbl"] = table;
//	}

	m_wlp["sensors"].push_back(newVal);
}

void CWLUnit::addCounterEngineHours(const double value, const bool autoInc, const std::string& type)
{
	if (!m_wlp.contains("counters")) {
		m_wlp["counters"] = json::object();
	}

	m_wlp["counters"] += json::object_t::value_type("cneh", value);

	/*json newVal = json::object();

	newVal["counters"]["cneh"] = value;*/

	uint32_t flags = m_wlp["counters"].value("cfl", 0UL);

	if (autoInc) {
		flags |= 0x200UL;
	}

	if (type == "0x10") {
		flags |= 0x10UL;
	}
	else if (type == "0x20") {
		flags |= 0x20UL;
	}
	else if (type == "0x40") {
		flags |= 0x40UL;
	}

	//m_wlp["counters"].update(json::object_t::value_type("cfl", flags));
	json newCfl = json::object();
	newCfl += json::object_t::value_type("cfl", flags);
	m_wlp["counters"].update(newCfl);

	//m_wlp["counters"]["cfl"] = flags;
}

void CWLUnit::addCounterMileage(const int32_t value, const bool autoInc, const std::string& type)
{
	if (!m_wlp.contains("counters")) {
		m_wlp["counters"] = json::object();
	}

	m_wlp["counters"] += json::object_t::value_type("cnm", value);


	uint32_t flags = m_wlp["counters"].value("cfl", 0UL);

	if (autoInc) {
		flags |= 0x100UL;
	}

	if (type == "0x0") {
		flags |= 0x00UL;
	}
	else if (type == "0x01") {
		flags |= 0x01UL;
	}
	else if (type == "0x02") {
		flags |= 0x02UL;
	}
	else if (type == "0x03") {
		flags |= 0x03UL;
	}

	json newCfl = json::object();
	newCfl += json::object_t::value_type("cfl", flags);
	m_wlp["counters"].update(newCfl);
}

void CWLUnit::addCounterBytes(const uint32_t valueKb, const bool autoInc)
{
	if (!m_wlp.contains("counters")) {
		m_wlp["counters"] = json::object();
	}

	m_wlp["counters"] += json::object_t::value_type("cnkb", valueKb);

	uint32_t flags = m_wlp["counters"].value("cfl", 0UL);

	if (autoInc) {
		flags |= 0x400UL;
	}

	json newCfl = json::object();
	newCfl += json::object_t::value_type("cfl", flags);
	m_wlp["counters"].update(newCfl);
}

void CWLUnit::addTripDetector(const wlpTripDetector& tripDetector)
{
	if (!m_wlp.contains("trip")) {
		m_wlp["trip"] = json::object();
	}

	m_wlp["trip"] += json::object_t::value_type("type", tripDetector.type);
	m_wlp["trip"] += json::object_t::value_type("gpsCorrection", tripDetector.gpsCorrection);
	m_wlp["trip"] += json::object_t::value_type("minSat", tripDetector.minSat);
	m_wlp["trip"] += json::object_t::value_type("minMovingSpeed", tripDetector.minMovingSpeed);
	m_wlp["trip"] += json::object_t::value_type("minStayTime", tripDetector.minStayTime);
	m_wlp["trip"] += json::object_t::value_type("maxMessagesDistance", tripDetector.maxMessagesDistance);
	m_wlp["trip"] += json::object_t::value_type("minTripTime", tripDetector.minTripTime);
	m_wlp["trip"] += json::object_t::value_type("minTripDistance", tripDetector.minTripDistance);
}

void CWLUnit::addFuelSettings(const wlpFuelConsumption& fuelSettings)
{
	if (!m_wlp.contains("fuel")) {
		m_wlp["fuel"] = json::object();
	}

	m_wlp["fuel"] += json::object_t::value_type("calcTypes", fuelSettings.calcTypes);

	json impulse = json::object();
	impulse += json::object_t::value_type("maxImpulses", fuelSettings.fuelConsImpulse.maxImpulses);
	impulse += json::object_t::value_type("skipZero", fuelSettings.fuelConsImpulse.skipZero);
	m_wlp["fuel"] += json::object_t::value_type("fuelConsImpulse", impulse);

	json math = json::object();
	math += json::object_t::value_type("idling", fuelSettings.fuelConsMath.idling);
	math += json::object_t::value_type("suburban", fuelSettings.fuelConsMath.suburban);
	math += json::object_t::value_type("urban", fuelSettings.fuelConsMath.urban);
	m_wlp["fuel"] += json::object_t::value_type("fuelConsMath", math);;

	json rates = json::object();
	rates += json::object_t::value_type("consSummer", fuelSettings.fuelConsRates.consSummer);
	rates += json::object_t::value_type("consWinter", fuelSettings.fuelConsRates.consWinter);
	rates += json::object_t::value_type("winterMonthFrom", fuelSettings.fuelConsRates.winterMonthFrom);
	rates += json::object_t::value_type("winterDayFrom", fuelSettings.fuelConsRates.winterDayFrom);
	rates += json::object_t::value_type("winterMonthTo", fuelSettings.fuelConsRates.winterMonthTo);
	rates += json::object_t::value_type("winterDayTo", fuelSettings.fuelConsRates.winterDayTo);
	m_wlp["fuel"] += json::object_t::value_type("fuelConsRates", rates);

	json level = json::object();
	level += json::object_t::value_type("flags", fuelSettings.fuelLevelParams.flags);
	level += json::object_t::value_type("ignoreStayTimeout", fuelSettings.fuelLevelParams.ignoreStayTimeout);
	level += json::object_t::value_type("minFillingVolume", fuelSettings.fuelLevelParams.minFillingVolume);
	level += json::object_t::value_type("minTheftTimeout", fuelSettings.fuelLevelParams.minTheftTimeout);
	level += json::object_t::value_type("minTheftVolume", fuelSettings.fuelLevelParams.minTheftVolume);
	level += json::object_t::value_type("filterQuality", fuelSettings.fuelLevelParams.filterQuality);
	level += json::object_t::value_type("fillingsJoinInterval", fuelSettings.fuelLevelParams.fillingsJoinInterval);
	level += json::object_t::value_type("theftsJoinInterval", fuelSettings.fuelLevelParams.theftsJoinInterval);
	level += json::object_t::value_type("extraFillingTimeout", fuelSettings.fuelLevelParams.extraFillingTimeout);
	m_wlp["fuel"] += json::object_t::value_type("fuelLevelParams", level);
}

bool CWLUnit::wlpSave(const std::string& filename) const
{
	if (filename.empty()) {
		return false;
	}

	std::ofstream out(filename);

	//std::wofstream out(filename);

	if (!out.good()) {
		return false;
	}

	//out << "Test" << std::endl;

	out << m_wlp.dump();

	return true;
}

void CWLUnit::createDefaultSensorConfig(wlpSensorConfig& config)
{
	config.act = true;
	config.appear_in_popup = true;
	//config.ci = std::string();
	config.cm = 1;
	config.consumption = 2.0;
	config.mu = 0;
	config.pos = 1;
	config.show_time = false;
	config.timeout = 0;
	config.uct = false;
	//config.unbound_code = std::string();
	config.validate_driver_unbound = false;
	config.lower_bound = 0.0;
	config.upper_bound = 0.0;
}

void CWLUnit::wlpInit()
{
	m_wlp.clear();

	m_wlp = json::object();

	m_wlp["type"] = "avl_unit";
	m_wlp["version"] = "b4";
	m_wlp["mu"] = 0;
}

std::string CWLUnit::createSensorConfig(const wlpSensorConfig& config, const std::string& sensorType, const uint32_t pos)
{
	json retval = json::object();

	retval["act"] = (config.act) ? 1 : 0;
	retval["appear_in_popup"] = config.appear_in_popup;
	retval["pos"] = pos;
	retval["show_time"] = config.show_time;
	retval["timeout"] = config.timeout;
	retval["uct"] = (config.uct) ? 1 : 0;
	retval["unbound_code"] = config.unbound_code;
	retval["validate_driver_unbound"] = (config.validate_driver_unbound) ? 1 : 0;

	if (sensorType == "engine operation") {
		retval["consumption"] = config.consumption;
	}

	retval["ci"] = json::object(); //default
	retval["cm"] = 1; //default
	retval["mu"] = 0; //default

	std::string sout = retval.dump();
	return sout;
}
