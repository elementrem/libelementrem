/*
	This file is part of cpp-elementrem.

	cpp-elementrem is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-elementrem is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-elementrem.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file Common.h
 * 
 * 
 */

#pragma once

#include <string>
#include <json/json.h>
#include <libdevcore/Log.h>

namespace dev
{
namespace test
{

struct TestChannel: public LogChannel  { static const char* name(); };
#define ctest dev::LogOutputStream<dev::test::TestChannel, true>()

std::string getTestPath();
int randomNumber();
Json::Value loadJsonFromFile(std::string const& _path);
std::string toTestFilePath(std::string const& _filename);
std::string getFolder(std::string const& _file);
std::string getRandomPath();

}

}
