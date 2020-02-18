// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#pragma once
#include <vector>
#include <string>

/**
    @brief Класс для парсинга аргументов командной строки
*/
class CCmdArgs
{
public:
/**
    Конструктор по умолчанию
*/
	CCmdArgs();
/**
    Конструктор класса
    @param[in] argc Количество передаваемых аргументов командной строки
    @param[in] argv Указатель на параметры
*/
	CCmdArgs(int argc, char* argv[]);
	
/**
    Деструктор
*/
	~CCmdArgs();

/**
    Проверка передан ли указанный аргумент
    @param[in] arg Запрашиваемый аргумент
    @return true в случае, если аргумент передан
*/
	bool isArgExist(const std::string& arg);

	//Return value of arg.
	//For example arg = --ifile="C:/dirname/filename.xml"
	//argValue("--ifile") will return "C:/dirname/filename.xml"
/**
    @brief Получить дополнительный параметр в аргументе
	
	Например, передан аргумент --ifile="C:/dirname/filename.xml".
	Вызов функции @code argValue("--ifile"); @endcode вернет "C:/dirname/filename.xml"
	
    @param[in] arg Запрашиваемый аргумент
    @return Строка со значением параметра
*/
	std::string argValue(const std::string& arg);
private:
/**
    @brief Переменная для хранения всех переданных аргументов
*/
	std::vector<std::string> m_args;
};

