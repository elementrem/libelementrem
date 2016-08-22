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

KeyPair const dev::ele::FluidityTreasure(Secret("a15e7af8ea0a717182d3608e6cdb2bff97ccaad3b6befc8787abe4c937796579"));

std::string const dev::ele::c_genesisInfoFluidity =
R"ELEMENTREM(
{
	"sealEngine": "BasicAuthority",
	"params": {
		"accountStartNonce": "0x",
		"maximumExtraDataSize": "0x1000000",
		"blockReward": "0x",
		"registrar": "",
		"networkID" : "0x45"
	},
	"genesis": {
		"author": "0x0000000000000000000000000000000000000000",
		"timestamp": "0x00",
		"parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"extraData": "0x",
		"gasLimit": "0x1000000000000"
	},
	"accounts": {
		"0000000000000000000000000000000000000001": { "mey": "1", "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "mey": "1", "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "mey": "1", "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "mey": "1", "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } },
		"00c9b024c2efc853ecabb8be2fb1d16ce8174ab1": { "mey": "1606938044258990275541962092341162602522202993782792835301376" }
	}
}
)ELEMENTREM";

std::string const dev::ele::c_genesisInfoTestBasicAuthority =
R"E(
{
	"sealEngine": "BasicAuthority",
	"params": {
		"accountStartNonce": "0x00",
		"maximumExtraDataSize": "0x20",
		"minGasLimit": "0x1388",
		"maxGasLimit": "0x7fffffffffffffff",
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
		"0000000000000000000000000000000000000001": { "mey": "1", "precompiled": { "name": "ecrecover", "linear": { "base": 3000, "word": 0 } } },
		"0000000000000000000000000000000000000002": { "mey": "1", "precompiled": { "name": "sha256", "linear": { "base": 60, "word": 12 } } },
		"0000000000000000000000000000000000000003": { "mey": "1", "precompiled": { "name": "ripemd160", "linear": { "base": 600, "word": 120 } } },
		"0000000000000000000000000000000000000004": { "mey": "1", "precompiled": { "name": "identity", "linear": { "base": 15, "word": 3 } } },
		"00c9b024c2efc853ecabb8be2fb1d16ce8174ab1": { "mey": "1606938044258990275541962092341162602522202993782792835301376" }
	}
}
)E";
