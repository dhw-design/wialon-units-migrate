// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#pragma once

#include "nlohmann/json.hpp"

using json = nlohmann::json;

using wlpSensorCalibRow = struct {
	double a;
	double b;
	double x;
};

using wlpSensorConfig = struct {
	bool act;
	bool appear_in_popup;
	std::string ci;
	uint32_t cm;
	double consumption;
	uint32_t mu;
	uint32_t pos;
	bool show_time;
	uint32_t timeout;
	bool uct;
	std::string unbound_code;
	bool validate_driver_unbound;
	double lower_bound;
	double upper_bound;
};

using wlpSensor = struct {
	std::string description;
	uint32_t flags;
	int id;
	std::string unit;
	std::string name;
	std::string parameter;
	std::string type;
	int validation_sensor_id;
	int validation_type;
	wlpSensorConfig config;
	std::vector<wlpSensorCalibRow> table;
};

using wlpTripDetector = struct {
	uint32_t type;
	uint32_t gpsCorrection;
	uint32_t minSat;
	uint32_t minMovingSpeed;
	uint32_t minStayTime;
	uint32_t maxMessagesDistance;
	uint32_t minTripTime;
	uint32_t minTripDistance;
};

using wlpGeneral = struct {
	std::string name;
	std::string phone1;
	std::string phone2;
	std::string hw;
	std::string uid;
	std::string password;
};

using wlpFuelLevelParams = struct {
	uint32_t flags;						//
	uint32_t ignoreStayTimeout;			//
	double minFillingVolume;			//
	uint32_t minTheftTimeout;			//
	double minTheftVolume;				//
	uint8_t filterQuality;				//
	uint32_t fillingsJoinInterval;		//
	uint32_t theftsJoinInterval;		//
	uint32_t extraFillingTimeout;		//
};

using wlpFuelConsMath = struct {
	double idling;						//
	double urban;						//
	double suburban;					//
};

using wlpFuelConsRates = struct {
	double consSummer;					//
	double consWinter;					//
	uint32_t winterMonthFrom;			//
	uint32_t winterDayFrom;				//
	uint32_t winterMonthTo;				//
	uint32_t winterDayTo;				//
};

using wlpFuelConsImpulse = struct {
	uint32_t maxImpulses;				//
	uint32_t skipZero;					//
};

using wlpFuelConsumption = struct {
	uint32_t calcTypes;					//
	wlpFuelLevelParams fuelLevelParams;
	wlpFuelConsMath fuelConsMath;
	wlpFuelConsRates fuelConsRates;
	wlpFuelConsImpulse fuelConsImpulse;
};

class CWLUnit
{
public:
	CWLUnit();
	CWLUnit(const CWLUnit& wlobject);
	~CWLUnit();

	void addAfield(const std::string& name, const std::string& value);
	void addGeneral(const wlpGeneral& general);
	void addSensor(const wlpSensor& sensor);

	void addCounterEngineHours(const double value, const bool autoInc, const std::string& type);
	void addCounterMileage(const int32_t value, const bool autoInc, const std::string& type);
	void addCounterBytes(const uint32_t valueKb, const bool autoInc);

	void addTripDetector(const wlpTripDetector& tripDetector);
	void addFuelSettings(const wlpFuelConsumption& fuelSettings);
	bool wlpSave(const std::string& filename) const;

	static void createDefaultSensorConfig(wlpSensorConfig& config);

private:
	json m_wlp;

	void wlpInit();
	std::string createSensorConfig(const wlpSensorConfig& config, const std::string& sensorType, const uint32_t pos = 1);
};

