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
/** @file boost.cpp
 * 
 * 
 * Tests for external dependencies: Boost
 */

#include <boost/test/unit_test.hpp>
#include <libdevcore/Common.h>
#include <test/TestHelper.h>

using namespace dev::test;

BOOST_FIXTURE_TEST_SUITE(ExtDepBoost, TestOutputHelper)

// test that reproduces issue https://github.com/elementrem/cpp-elementrem/issues/1977
BOOST_AUTO_TEST_CASE(u256_overflow_test)
{
	dev::u256 a = 14;
	dev::bigint b = dev::bigint("115792089237316195423570985008687907853269984665640564039457584007913129639948");
	// to fix cast `a` to dev::bigint
	BOOST_CHECK(a < b);
}

BOOST_AUTO_TEST_SUITE_END()
