// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#pragma once

#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
    @brief Структура для хранения одной записи в таблице калибровки
*/
typedef struct {
	double a;
	double b;
	double x;
} wlpSensorCalibRow;

/**
    @brief Структура для хранения конфигурации датчика
*/
typedef struct {
	bool act;						///< Опция "Последнее сообщение"
	bool appear_in_popup; 			///< галочка "Видимость": false - снять, true - поставить
	std::string ci;					///< пользовательские интервалы
	uint32_t cm;					///< 
	double consumption;				///< Расход топлива на холостом ходу, л/ч
	uint32_t mu;					///< 
	uint32_t pos;					///< положение в списке Датчиков, счет от 1 
	bool show_time;					///< галочка "Время": false - снять, true - поставить 
	uint32_t timeout;				///< таймаут, секунды 
	bool uct;						///< опция "Переполнение по сырым данным": false - выкл, true - вкл
	std::string unbound_code;		///< код снятия
	bool validate_driver_unbound;	///< валидировать снятие, false - нет, true - да 
	double lower_bound;				///< опция "Нижняя граница"
	double upper_bound;				///< опция "Верхняя граница"
} wlpSensorConfig;

/**
    @brief Структура для хранения данных по датчику
*/
typedef struct {
	std::string description;				///< описание
	/**
	    @brief флаги датчика
		
		Флаг | Описание
		---- | ------------
		0x01 | Тип счетчика: мгновенный
		0x02 | Тип счетчика: дифференциальный
		0x03 | Тип счетчика: дифференциальный с переполнением (2 байта)
		0x04 | Тип счетчика: переключатель из ВЫКЛ в ВКЛ
		0x05 | Тип счетчика: Переключатель из ВКЛ в ВЫКЛ
		0x20 | включить опцию "С переполнением"
		0x40 | Применять нижнюю/верхнюю границы ПОСЛЕ расчета
	*/
	uint32_t flags;							
	int id;									///< ID датчика
	std::string unit;						///< единица измерения
	std::string name;						///< название
	std::string parameter;					///< параметр
	/**
		@brief тип датчика
		
		
		- **absolute fuel consumption** - датчик абсолютного расхода топлива;
		- **accelerometer** - акселерометр;
		- **alarm trigger** - тревожная кнопка;
		- **counter** - счетчик;
		- **custom** - произвольный датчик;
		- **digital** - произвольный цифровой датчик;
		- **driver** - привязка водителя;
		- **engine efficiency** - датчик полезной работы двигателя;
		- **engine hours** - абсолютные моточасы;
		- **engine operation** - датчик зажигания;
		- **engine rpm** - датчик оборотов двигателя;
		- **fuel level impulse sensor** - импульсный датчик уровня топлива;
		- **fuel level** - датчик уровня топлива;
		- **impulse fuel consumption** - импульсный датчик расхода топлива;
		- **instant fuel consumption** - датчик мгновенного расхода топлива;
		- **mileage** - датчик пробега;
		- **odometer** - относительный одометр;
		- **private mode** - частный режим;
		- **relative engine hours** - относительные моточасы;
		- **temperature coefficient** - коэффициент температуры;
		- **temperature** - датчик температуры;
		- **trailer** - привязка прицепа;
		- **voltage** - датчик напряжения;
		- **weight sensor** - датчик веса.
	*/
	std::string type;						
	int validation_sensor_id;				///< ID валидирующего датчика
	/**
		@brief тип валидации
		
		Тип  | описание
		---- | ---------
		0x01 | Логическое И
		0x02 | Логическое ИЛИ
		0x03 | Математическое И
		0x04 | Математическое ИЛИ
		0x05 | Суммировать
		0x06 | Вычесть валидатор из датчика
		0x07 | Вычесть датчик из валидатора
		0x08 | Перемножить
		0x09 | Делить датчик на валидатор
		0x0A | Делить валидатор на датчик
		0x0B | Проверка на неравенство нулю
		0x0C | Заменять датчик валидатором в случае ошибки
	*/
	int validation_type;					
	wlpSensorConfig config;					///< конфигурация датчика
	std::vector<wlpSensorCalibRow> table;	///< таблица калибровки
} wlpSensor;

/**
    @brief Структура для хранения данных о настройках детектора поездок
*/
typedef struct {
	uint32_t type;					///< тип определения движения
	uint32_t gpsCorrection;			///< коррекция по GPS: 0 - выключить, 1 - включить
	uint32_t minSat;				///< минимальное количество спутников
	uint32_t minMovingSpeed;		///< минимальная скорость движения, км/ч
	uint32_t minStayTime;			///< минимальное время стоянки, секунд
	uint32_t maxMessagesDistance;	///< максимальное расстояние между сообщениями, метров
	uint32_t minTripTime;			///< минимальное время поездки, секунд
	uint32_t minTripDistance;		///< минимальное расстояние поездки, метров
} wlpTripDetector;

/**
    @brief Структура для хранения основных данных по объекту
*/
typedef struct {
	std::string name;		///< Имя объекта
	std::string phone1; 	///< Номер СИМ карты 1
	std::string phone2; 	///< Номер СИМ карты 2
	std::string hw;     	///< Тип оборудования
	std::string uid;    	///< Уникальный ID оборудования
	std::string password;	///< Пароль для доступа
} wlpGeneral;

/**
    @brief Структура для хранения настроек ДУТов
*/
typedef struct {
	/**
		@brief флаги заправки/слива топлива
		
		Флаг  | Описание
		----  | --------
		0x01  | Группировать датчики уровня топлива с одинаковым именем
		0x02  | Фильтровать значения датчиков уровня топлива
		0x04  | Группировать датчики расхода топлива с одинаковым именем
		0x08  | Поиск заправок только при остановке
		0x10  | Расчет датчиков уровня топлива по времени
		0x40  | Рассчитывать объем заправки без учета фильтрации
		0x80  | Рассчитывать объем слива без учета фильтрации
		0x100 | Поиск сливов в движении 
		0x200 | Рассчитывать заправки по времени
		0x400 | Рассчитывать сливы по времени
		0x800 | Рассчитывать расход топлива по времени
	*/
	uint32_t flags;						
	uint32_t ignoreStayTimeout;			///< игнорировать сообщения после начала движения, секунд
	double minFillingVolume;			///< минимальный объем заправки, литров
	uint32_t minTheftTimeout;			///< минимальное время остановки для определения слива, секунд
	double minTheftVolume;				///< минимальный объем слива, литров
	uint8_t filterQuality;				///< степень фильтрации (0..255)
	uint32_t fillingsJoinInterval;		///< таймаут для разделения заправок, секунд
	uint32_t theftsJoinInterval;		///< таймаут для разделения сливов, секунд
	uint32_t extraFillingTimeout;		///< таймаут для определения полного объема заправки, секунд
} wlpFuelLevelParams;

/**
    @brief Структура для хранения настроек расхода топлива по расчету
*/
typedef struct {
	double idling;						///< расход на холостом ходу, литров в час
	double urban;						///< городской цикл, литров на 100 км 
	double suburban;					///< загородный цикл, литров на 100 км 
} wlpFuelConsMath;

/**
    @brief Структура для хранения настроек расхода топлива по нормам
*/
typedef struct {
	double consSummer;					///< расход летом, литров на 100 км 
	double consWinter;					///< расход зимой, литров на 100 км
	uint32_t winterMonthFrom;			///< зима от (месяц 0-11) 
	uint32_t winterDayFrom;				///< зима от (день 1-31) 
	uint32_t winterMonthTo;				///< зима до (месяц 0-11) 
	uint32_t winterDayTo;				///< зима до (день 1-31) 
} wlpFuelConsRates;

/**
    @brief Структура для хранения настроек расхода топлива по импульсному датчику расхода топлива
*/
typedef struct {
	uint32_t maxImpulses;				///< максимум импульсов 
	uint32_t skipZero;					///< пропускать начальные нулевые значения
} wlpFuelConsImpulse;

/**
    @brief Структура для хранения настроек расхода топлива по импульсному датчику расхода топлива
*/
typedef struct {
	/**
		@brief тип расчета расхода топлива
		
		Флаг | Описание
		---- | --------
		0x00 | не использовать расчет топлива в отчетах
		0x01 | расход по расчету
		0x02 | датчики уровня топлива
		0x04 | заменять ошибочные значения датчиков уровня топлива рассчитанными математически
		0x08 | датчики абсолютного расхода топлива
		0x10 | импульсные датчики
		0x20 | датчики мгновенного расхода топлива
		0x40 | расход по нормам 
	*/
	uint32_t calcTypes;					
	wlpFuelLevelParams fuelLevelParams;	///< настройка ДУТ
	wlpFuelConsMath fuelConsMath;		///< настройка расхода по расчету
	wlpFuelConsRates fuelConsRates;		///< настройка расхода по нормам
	wlpFuelConsImpulse fuelConsImpulse;	///< настройка импульсных датчиков расхода топлива
} wlpFuelConsumption;

/**
    @brief Класс для создания файла для Wialon Local
*/
class CWLUnit
{
public:
/**
    @brief Конструктор по умолчанию
*/
	CWLUnit();
	
/**
    @brief Конструктор копирования
*/
	CWLUnit(const CWLUnit& wlobject);
	
/**
    @brief Деструктор
*/
	~CWLUnit();
/**
    @brief Добавить произвольное поле
	@param[in] name Имя поля
    @param[in] value Значение
*/
	void addAfield(const std::string& name, const std::string& value);
	
/**
    @brief Добавить основные свойства объекта
	@param[in] general Ссылка на структуру с основными свойствами
*/
	void addGeneral(const wlpGeneral& general);
	
/**
    @brief Добавить датчик
	@param[in] sensor Ссылка на структуру с данным по датчику
*/
	void addSensor(const wlpSensor& sensor);

/**
    @brief Добавить значение счетчика моточасов
	@param[in] value Значение счетчика
	@param[in] autoInc Автоинкремент счетчика
	@param[in] type По какому параметру производится автоинкремент
*/
	void addCounterEngineHours(const double value, const bool autoInc, const std::string& type);
	
/**
    @brief Добавить значение счетчика пробега
	@param[in] value Значение счетчика
	@param[in] autoInc Автоинкремент счетчика
	@param[in] type По какому параметру производится автоинкремент
*/
	void addCounterMileage(const int32_t value, const bool autoInc, const std::string& type);
	
/**
    @brief Добавить значение счетчика GPRS трафика
	@param[in] valueKb Значение счетчика, кБ
	@param[in] autoInc Автоинкремент счетчика
*/
	void addCounterBytes(const uint32_t valueKb, const bool autoInc);

/**
    @brief Добавить настройки по детектору поездок
	@param[in] tripDetector Ссылка на структуру с настройками детектора поездок
*/
	void addTripDetector(const wlpTripDetector& tripDetector);
	
/**
    @brief Добавить настройки по расходу топлива
	@param[in] fuelSettings Ссылка на структуру с настройками расхода топлива
*/
	void addFuelSettings(const wlpFuelConsumption& fuelSettings);
	
/**
    @brief Сохранить файл
	@param[in] filename Путь к файлу для сохранения
	@return true в случае успеха
*/
	bool wlpSave(const std::string& filename) const;

/**
    @brief Заполнить структуру конфигурации датчика значениями по умолчанию
	@param config Ссылка на структуру для заполнения
*/
	static void createDefaultSensorConfig(wlpSensorConfig& config);

private:
	json m_wlp;

	void wlpInit();
	std::string createSensorConfig(const wlpSensorConfig& config, const std::string& sensorType, const uint32_t pos = 1);
};

