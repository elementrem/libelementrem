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
/** @file GenesisInfo.cpp
 * 
 * 
 */

#include "GenesisInfo.h"
using namespace dev;
using namespace ele;

static h256 const c_genesisStateRootOlympic;
static std::string const c_genesisInfoOlympic =
R"ELEMENTREM(
{
	"sealEngine": "Elhash",
	"params": {
		"accountStartNonce": "0x00",
		"frontierCompatibilityModeLimit": "0xffffffff",
		"maximumExtraDataSize": "0x0400",
		"tieBreakingGas": false,
		"minGasLimit": "125000",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x08",
		"blockReward": "0x14D1120D7B160000",
		"registrar": "5e70c0bbcd5636e0f9f9316e9f8633feb64d4050",
		"networkID" : "0x0"
	},
	"genesis": {
		"nonce": "0x000000000000002a",
		"difficulty": "0x20000",
		"mixHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"author": "0x0000000000000000000000000000000000000000",
		"timestamp": "0x00",
		"parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"extraData": "0x",
		"gasLimit": "0x2fefd8"
	},
	"accounts": {
		"0000000000000000000000000000000000000001": { "mey": "1", "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "mey": "1", "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "mey": "1", "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "mey": "1", "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } },
		"dbdbdb2cbd23b783741e8d7fcf51e459b497e4a6": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"e6716f9544a56c530d868e4bfbacb172315bdead": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"b9c015918bdaba24b4ff057a92a3873d6eb201be": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"1a26338f0d905e295fccb71fa9ea849ffa12aaf4": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"2ef47100e0787b915105fd5e3f4ff6752079d5cb": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"cd2a3d9f938e13cd947ec05abc7fe734df8dd826": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"6c386a4b26f73c802f34673f7248bb118f97424a": { "mey": "1606938044258990275541962092341162602522202993782792835301376" },
		"e4157b34ea9615cfbde6b4fda419828124b70c78": { "mey": "1606938044258990275541962092341162602522202993782792835301376" }
	}
}
)ELEMENTREM";

static h256 const c_genesisStateRootMorden;
static std::string const c_genesisInfoMorden =
R"ELEMENTREM(
{
	"sealEngine": "Elhash",
	"params": {
		"accountStartNonce": "0x0100000",
		"frontierCompatibilityModeLimit": "0x789b0",
		"maximumExtraDataSize": "0x20",
		"tieBreakingGas": false,
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000",
		"registrar": "",
		"networkID" : "0x2"
	},
	"genesis": {
		"nonce": "0x00006d6f7264656e",
		"difficulty": "0x20000",
		"mixHash": "0x00000000000000000000000000000000000000647572616c65787365646c6578",
		"author": "0x0000000000000000000000000000000000000000",
		"timestamp": "0x00",
		"parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"extraData": "0x",
		"gasLimit": "0x2fefd8"
	},
	"accounts": {
		"0000000000000000000000000000000000000001": { "mey": "1", "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "mey": "1", "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "mey": "1", "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "mey": "1", "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } },
		"102e61f5d8f9bc71d0ad4a084df4e65e05ce0e1c": { "mey": "1606938044258990275541962092341162602522202993782792835301376" }
	}
}
)ELEMENTREM";

static h256 const c_genesisStateRootFrontierTest;
static std::string const c_genesisInfoFrontierTest =
R"E(
{
	"sealEngine": "Elhash",
	"params": {
		"accountStartNonce": "0x00",
		"frontierCompatibilityModeLimit": "0xffffffffffffffff",
		"maximumExtraDataSize": "0x20",
		"tieBreakingGas": false,
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000",
		"registrar" : "0xc6d9d2cd449a754c494264e1809c50e34d64562b",
		"networkID" : "0x12005"
	},
	"genesis": {
        "config":{
                "homesteadBlock":0
                },
        "nonce": "0x0000000000000000",
        "difficulty": "0x20000",
        "mixhash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "coinbase": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "timestamp": "0x00",
        "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "extraData": "0xaa57ee17a7257988a36b7b418cd716d315028b19",
        "gasLimit": "0x2FEFD8"
},
	"accounts": {
		"0000000000000000000000000000000000000001": { "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } }
	}
}
)E";

static h256 const c_genesisStateRootTest;
static std::string const c_genesisInfoTest = std::string() +
R"E(
{
	"sealEngine": "Elhash",
	"params": {
		"accountStartNonce": "0x00",
		"frontierCompatibilityModeLimit": "0x05",
		"maximumExtraDataSize": "0x20",
		"tieBreakingGas": false,
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000",
		"registrar" : "0xc6d9d2cd449a754c494264e1809c50e34d64562b",
		"networkID" : "0x1"
	},
	"genesis": {
        "config":{
                "homesteadBlock":0
                },
        "nonce": "0x0000000000000000",
        "difficulty": "0x20000",
        "mixhash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "coinbase": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "timestamp": "0x00",
        "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "extraData": "0xaa57ee17a7257988a36b7b418cd716d315028b19",
        "gasLimit": "0x2FEFD8"
},
	"accounts": {
		"0000000000000000000000000000000000000001": { "mey": "1", "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "mey": "1", "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "mey": "1", "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "mey": "1", "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } }
	}
}
)E";

static h256 const c_genesisStateRootHomesteadTest;
static std::string const c_genesisInfoHomesteadTest = std::string() +
R"E(
{
	"sealEngine": "Elhash",
	"params": {
		"accountStartNonce": "0x00",
		"maximumExtraDataSize": "0x20",
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"tieBreakingGas": false,
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000",
		"registrar" : "0xc6d9d2cd449a754c494264e1809c50e34d64562b",
		"networkID" : "0x1"
	},
	"genesis": {
        "config":{
                "homesteadBlock":0
                },
        "nonce": "0x0000000000000000",
        "difficulty": "0x20000",
        "mixhash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "coinbase": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "timestamp": "0x00",
        "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "extraData": "0xaa57ee17a7257988a36b7b418cd716d315028b19",
        "gasLimit": "0x2FEFD8"
},
	"accounts": {
		"0000000000000000000000000000000000000001": { "mey": "1", "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "mey": "1", "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "mey": "1", "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "mey": "1", "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } }
	}
}
)E";

static h256 const c_genesisStateRootFrontier("07508cdf0dbe8abe8b14b1ac12778463d2a30664104d1d82dacb9e117b031cb4");
static std::string const c_genesisInfoFrontier = std::string() +
R"E(
{
	"sealEngine": "Elhash",
	"params": {
		"accountStartNonce": "0x00",
		"frontierCompatibilityModeLimit": "0x118c30",
		"maximumExtraDataSize": "0x20",
		"tieBreakingGas": false,
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x020000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000",
		"registrar" : "0xc6d9d2cd449a754c494264e1809c50e34d64562b",
		"networkID" : "0x1"
	},
	"genesis": {
        "config":{
                "homesteadBlock":0
                },
        "nonce": "0x0000000000000000",
        "difficulty": "0x20000",
        "mixhash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "coinbase": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "timestamp": "0x00",
        "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
        "extraData": "0xaa57ee17a7257988a36b7b418cd716d315028b19",
        "gasLimit": "0x2FEFD8"
},
	"accounts": {
		"0000000000000000000000000000000000000001": { "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } },
		"a748f720f5989f2b541fa9ef3c78236808123635": {
			"balance": "50000000000000000000000000"
		}
	}
}
)E";

std::string const& dev::ele::genesisInfo(Network _n)
{
	switch (_n)
	{
	case Network::Olympic: return c_genesisInfoOlympic;
	case Network::Frontier: return c_genesisInfoFrontier;
	case Network::Morden: return c_genesisInfoMorden;
	case Network::Test: return c_genesisInfoTest;
	case Network::FrontierTest: return c_genesisInfoFrontierTest;
	case Network::HomesteadTest: return c_genesisInfoHomesteadTest;
	default:
		throw std::invalid_argument("Invalid network value");
	}
}

h256 const& dev::ele::genesisStateRoot(Network _n)
{
	switch (_n)
	{
	case Network::Olympic: return c_genesisStateRootOlympic;
	case Network::Frontier: return c_genesisStateRootFrontier;
	case Network::Morden: return c_genesisStateRootMorden;
	case Network::Test: return c_genesisStateRootTest;
	case Network::FrontierTest: return c_genesisStateRootFrontierTest;
	case Network::HomesteadTest: return c_genesisStateRootHomesteadTest;
	default:
		throw std::invalid_argument("Invalid network value");
	}
}
