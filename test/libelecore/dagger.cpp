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
/** @file dagger.cpp
 * 
 * 
 * Dashimoto test functions.
 */

#include <fstream>
#include <json_spirit/JsonSpiritHeaders.h>
#include <libdevcore/CommonIO.h>
#include <libelhashseal/Elhash.h>
#include <libelhashseal/ElhashAux.h>
#include <boost/test/unit_test.hpp>
#include <test/TestHelper.h>

using namespace std;
using namespace dev;
using namespace dev::ele;
using namespace dev::test;

namespace js = json_spirit;

using dev::operator <<;

BOOST_FIXTURE_TEST_SUITE(DashimotoTests, TestOutputHelper)

BOOST_AUTO_TEST_CASE(basic_test)
{
	string testPath = test::getTestPath();

	testPath += "/PoWTests";

	cnote << "Testing Proof of Work...";
	js::mValue v;
	string s = contentsString(testPath + "/elhash_tests.json");
	BOOST_REQUIRE_MESSAGE(s.length() > 0, "Contents of 'elhash_tests.json' is empty. Have you cloned the 'tests' repo branch develop?");
	js::read_string(s, v);
	for (auto& i: v.get_obj())
	{
		cnote << i.first;
		js::mObject& o = i.second.get_obj();
		vector<pair<string, string>> ss;
		BlockHeader header(fromHex(o["header"].get_str()), HeaderData);
		h256 headerHash(o["header_hash"].get_str());
		Nonce nonce(o["nonce"].get_str());
		BOOST_REQUIRE_EQUAL(headerHash, header.hash(WithoutSeal));
		BOOST_REQUIRE_EQUAL(nonce, Elhash::nonce(header));

		unsigned cacheSize(o["cache_size"].get_int());
		h256 cacheHash(o["cache_hash"].get_str());
		BOOST_REQUIRE_EQUAL(ElhashAux::get()->light(Elhash::seedHash(header))->size, cacheSize);
		BOOST_REQUIRE_EQUAL(sha3(ElhashAux::get()->light(Elhash::seedHash(header))->data()), cacheHash);

#if TEST_FULL
		unsigned fullSize(o["full_size"].get_int());
		h256 fullHash(o["full_hash"].get_str());
		BOOST_REQUIRE_EQUAL(ElhashAux::get()->full(Elhash::seedHash(header))->size(), fullSize);
		BOOST_REQUIRE_EQUAL(sha3(ElhashAux::get()->full(Elhash::seedHash(header))->data()), fullHash);
#endif

		h256 result(o["result"].get_str());
		ElhashProofOfWork::Result r = ElhashAux::eval(Elhash::seedHash(header), header.hash(WithoutSeal), Elhash::nonce(header));
		BOOST_REQUIRE_EQUAL(r.value, result);
		BOOST_REQUIRE_EQUAL(r.mixHash, Elhash::mixHash(header));
	}
}

BOOST_AUTO_TEST_SUITE_END()
