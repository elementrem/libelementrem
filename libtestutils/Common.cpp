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
/** @file Common.cpp
 * 
 * 
 */

#include <random>
#include <boost/filesystem.hpp>
#include <libdevcore/CommonData.h>
#include <libdevcore/CommonIO.h>
#include <libdevcore/FileSystem.h>
#include "Common.h"

using namespace std;
using namespace dev;
using namespace dev::test;

const char* TestChannel::name() { return "TST"; }

std::string dev::test::getTestPath()
{
	string testPath;
	const char* ptestPath = getenv("ELEMENTREM_TEST_PATH");

	if (ptestPath == NULL)
	{
		ctest << " could not find environment variable ELEMENTREM_TEST_PATH \n";
		testPath = "../../../tests";
	}
	else
		testPath = ptestPath;

	return testPath;
}

int dev::test::randomNumber()
{
	static std::mt19937 randomGenerator(utcTime());
	randomGenerator.seed(std::random_device()());
	return std::uniform_int_distribution<int>(1)(randomGenerator);
}

Json::Value dev::test::loadJsonFromFile(std::string const& _path)
{
	Json::Reader reader;
	Json::Value result;
	string s = dev::contentsString(_path);
	if (!s.length())
		ctest << "Contents of " + _path + " is empty. Have you cloned the 'tests' repo branch develop and set ELEMENTREM_TEST_PATH to its path?";
	else
		ctest << "FIXTURE: loaded test from file: " << _path;
	
	reader.parse(s, result);
	return result;
}

std::string dev::test::toTestFilePath(std::string const& _filename)
{
	return getTestPath() + "/" + _filename + ".json";
}

std::string dev::test::getFolder(std::string const& _file)
{
	return boost::filesystem::path(_file).parent_path().string();
}

std::string dev::test::getRandomPath()
{
	std::stringstream stream;
	stream << getDataDir("ElementremTests") << "/" << randomNumber();
	return stream.str();
}

