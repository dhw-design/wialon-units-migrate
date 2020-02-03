// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#include "CCmdArgs.h"

CCmdArgs::CCmdArgs()
{
}

CCmdArgs::CCmdArgs(int argc, char* argv[])
{
	m_args.clear();

	for (int count = 0; count < argc; count++) {
		m_args.push_back(argv[count]);
	}
}

CCmdArgs::~CCmdArgs()
{
}

bool CCmdArgs::isArgExist(const std::string& arg)
{
	if (arg.empty()) {
		return false;
	}

	for (auto it = m_args.cbegin(); it != m_args.cend(); ++it) {
		if ((*it).find(arg) == 0) {
			return true;
		}
	}
	return false;
}

//Return value of arg.
//For example arg = --ifile="C:/dirname/filename.xml"
//argValue("--ifile") will return "C:/dirname/filename.xml"
std::string CCmdArgs::argValue(const std::string& arg)
{
	if (arg.empty()) {
		return std::string();
	}


	std::string argTemp = arg;

	if (argTemp.rfind('=') != argTemp.length() - 1) {
		argTemp += "=";
	}
	for (auto it = m_args.cbegin(); it != m_args.cend(); ++it) {
		if ((*it).find(argTemp) == 0) {
			return (*it).substr(argTemp.length());
		}
	}
	return std::string();
}
