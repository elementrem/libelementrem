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

#include "Common.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <libdevcore/Base64.h>
#include <libdevcore/Terminal.h>
#include <libdevcore/CommonData.h>
#include <libdevcore/CommonIO.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include "ICAP.h"
#include "Exceptions.h"
#include "BlockHeader.h"

using namespace std;
using namespace dev;
using namespace dev::ele;

namespace dev
{
namespace ele
{

const unsigned c_protocolVersion = 63;
#if ELE_FATDB
const unsigned c_minorProtocolVersion = 3;
const unsigned c_databaseBaseVersion = 9;
const unsigned c_databaseVersionModifier = 1;
#else
const unsigned c_minorProtocolVersion = 2;
const unsigned c_databaseBaseVersion = 9;
const unsigned c_databaseVersionModifier = 0;
#endif

const unsigned c_databaseVersion = c_databaseBaseVersion + (c_databaseVersionModifier << 8) + (23 << 9);

Address toAddress(std::string const& _s)
{
	try
	{
		ele::ICAP i = ele::ICAP::decoded(_s);
		return i.direct();
	}
	catch (ele::InvalidICAP&) {}
	try
	{
		auto b = fromHex(_s.substr(0, 2) == "0x" ? _s.substr(2) : _s, WhenError::Throw);
		if (b.size() == 20)
			return Address(b);
	}
	catch (BadHexCharacter&) {}
	BOOST_THROW_EXCEPTION(InvalidAddress());
}

vector<pair<u256, string>> const& units()
{
	static const vector<pair<u256, string>> s_units =
	{
		{exp10<54>(), "Uelement"},
		{exp10<51>(), "Velement"},
		{exp10<48>(), "Delement"},
		{exp10<45>(), "Nelement"},
		{exp10<42>(), "Yelement"},
		{exp10<39>(), "Zelement"},
		{exp10<36>(), "Eelement"},
		{exp10<33>(), "Pelement"},
		{exp10<30>(), "Telement"},
		{exp10<27>(), "Gelement"},
		{exp10<24>(), "Melement"},
		{exp10<21>(), "grand"},
		{exp10<18>(), "element"},
		{exp10<15>(), "finney"},
		{exp10<12>(), "szabo"},
		{exp10<9>(), "Gmey"},
		{exp10<6>(), "Mmey"},
		{exp10<3>(), "Kmey"},
		{exp10<0>(), "mey"}
	};

	return s_units;
}

std::string formatBalance(bigint const& _b)
{
	ostringstream ret;
	u256 b;
	if (_b < 0)
	{
		ret << "-";
		b = (u256)-_b;
	}
	else
		b = (u256)_b;

	if (b > units()[0].first * 1000)
	{
		ret << (b / units()[0].first) << " " << units()[0].second;
		return ret.str();
	}
	ret << setprecision(5);
	for (auto const& i: units())
		if (i.first != 1 && b >= i.first)
		{
			ret << (double(b / (i.first / 1000)) / 1000.0) << " " << i.second;
			return ret.str();
		}
	ret << b << " mey";
	return ret.str();
}

static void badBlockInfo(BlockHeader const& _bi, string const& _err)
{
	string const c_line = EleReset EleOnMaroon + string(80, ' ') + EleReset;
	string const c_border = EleReset EleOnMaroon + string(2, ' ') + EleReset EleMaroonBold;
	string const c_space = c_border + string(76, ' ') + c_border + EleReset;
	stringstream ss;
	ss << c_line << endl;
	ss << c_space << endl;
	ss << c_border + "  Import Failure     " + _err + string(max<int>(0, 53 - _err.size()), ' ') + "  " + c_border << endl;
	ss << c_space << endl;
	string bin = toString(_bi.number());
	ss << c_border + ("                     Guru Meditation #" + string(max<int>(0, 8 - bin.size()), '0') + bin + "." + _bi.hash().abridged() + "                    ") + c_border << endl;
	ss << c_space << endl;
	ss << c_line;
	cwarn << "\n" + ss.str();
}

void badBlock(bytesConstRef _block, string const& _err)
{
	BlockHeader bi;
	DEV_IGNORE_EXCEPTIONS(bi = BlockHeader(_block));
	badBlockInfo(bi, _err);
}

string TransactionSkeleton::userReadable(bool _toProxy, function<pair<bool, string>(TransactionSkeleton const&)> const& _getNatSpec, function<string(Address const&)> const& _formatAddress) const
{
	if (creation)
	{
		// show notice concerning the creation code. TODO: this needs entering into natspec.
		return string("ÐApp is attempting to create a contract; ") + (_toProxy ? "(this transaction is not executed directly, but forwarded to another ÐApp) " : "") + "to be endowed with " + formatBalance(value) + ", with additional network fees of up to " + formatBalance(gas * gasPrice) + ".\n\nMaximum total cost is " + formatBalance(value + gas * gasPrice) + ".";
	}

	bool isContract;
	std::string natSpec;
	tie(isContract, natSpec) = _getNatSpec(*this);
	if (!isContract)
	{
		// recipient has no code - nothing special about this transaction, show basic value transfer info
		return "ÐApp is attempting to send " + formatBalance(value) + " to a recipient " + _formatAddress(to) + (_toProxy ? " (this transaction is not executed directly, but forwarded to another ÐApp)" : "") + ", with additional network fees of up to " + formatBalance(gas * gasPrice) + ".\n\nMaximum total cost is " + formatBalance(value + gas * gasPrice) + ".";
	}

	if (natSpec.empty())
		return "ÐApp is attempting to call into an unknown contract at address " +
				_formatAddress(to) + ".\n\n" +
				(_toProxy ? "This transaction is not executed directly, but forwarded to another ÐApp.\n\n" : "")  +
				"Call involves sending " +
				formatBalance(value) + " to the recipient, with additional network fees of up to " +
				formatBalance(gas * gasPrice) +
				"However, this also does other stuff which we don't understand, and does so in your name.\n\n" +
				"WARNING: This is probably going to cost you at least " +
				formatBalance(value + gas * gasPrice) +
				", however this doesn't include any side-effects, which could be of far greater importance.\n\n" +
				"REJECT UNLESS YOU REALLY KNOW WHAT YOU ARE DOING!";

	return "ÐApp attempting to conduct contract interaction with " +
	_formatAddress(to) +
	": <b>" + natSpec + "</b>.\n\n" +
	(_toProxy ? "This transaction is not executed directly, but forwarded to another ÐApp.\n\n" : "") +
	(value > 0 ?
		"In addition, ÐApp is attempting to send " +
		formatBalance(value) + " to said recipient, with additional network fees of up to " +
		formatBalance(gas * gasPrice) + " = " +
		formatBalance(value + gas * gasPrice) + "."
	:
		"Additional network fees are at most" +
		formatBalance(gas * gasPrice) + ".");
}

}
}
