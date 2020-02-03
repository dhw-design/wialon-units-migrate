// Copyright (c) 2020 Dmitriy Bassamykin.
// Licensed under the MIT license <http://opensource.org/licenses/MIT>.

#pragma once
#include <vector>
#include <string>

class CCmdArgs
{
public:
	CCmdArgs();
	CCmdArgs(int argc, char* argv[]);
	~CCmdArgs();

	//Return true if arg exitst
	bool isArgExist(const std::string& arg);

	//Return value of arg.
	//For example arg = --ifile="C:/dirname/filename.xml"
	//argValue("--ifile") will return "C:/dirname/filename.xml"
	std::string argValue(const std::string& arg);
private:
	std::vector<std::string> m_args;
};

