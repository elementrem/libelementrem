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
/** @file VerfiedBlock.h
 * 
 * 
 */


#include <libdevcore/Common.h>
#include <libelecore/BlockHeader.h>

#pragma once

namespace dev
{
namespace ele
{

class Transaction;

/// @brief Verified block info, does not hold block data, but a reference instead
struct VerifiedBlockRef
{
	bytesConstRef block; 					///<  Block data reference
	BlockHeader info;							///< Prepopulated block info
	std::vector<Transaction> transactions;	///< Verified list of block transactions
};

/// @brief Verified block info, combines block data and verified info/transactions
struct VerifiedBlock
{
	VerifiedBlock() {}

	VerifiedBlock(BlockHeader&& _bi)
	{
		verified.info = std::move(_bi);
	}

	VerifiedBlock(VerifiedBlock&& _other):
		verified(std::move(_other.verified)),
		blockData(std::move(_other.blockData))
	{
	}

	VerifiedBlock& operator=(VerifiedBlock&& _other)
	{
		assert(&_other != this);

		verified = (std::move(_other.verified));
		blockData = (std::move(_other.blockData));
		return *this;
	}

	VerifiedBlockRef verified;				///< Verified block structures
	bytes blockData;						///< Block data

private:
	VerifiedBlock(VerifiedBlock const&) = delete;
	VerifiedBlock operator=(VerifiedBlock const&) = delete;
};

using VerifiedBlocks = std::vector<VerifiedBlock>;

}
}
