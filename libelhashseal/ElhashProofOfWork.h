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
/** @file ElhashProofOfWork.h
 * 
 * 
 *
 * Determines the PoW algorithm.
 */

#pragma once

#include <libelecore/BlockHeader.h>

namespace dev
{
namespace ele
{

/// Proof of work definition for Elhash.
struct ElhashProofOfWork
{
	struct Solution
	{
		Nonce nonce;
		h256 mixHash;
	};

	struct Result
	{
		h256 value;
		h256 mixHash;
	};

	struct WorkPackage
	{
		WorkPackage() = default;
		WorkPackage(BlockHeader const& _bh);
		void reset() { headerHash = h256(); }
		operator bool() const { return headerHash != h256(); }

		h256 boundary;
		h256 headerHash;	///< When h256() means "pause until notified a new work package is available".
		h256 seedHash;
	};

	static const WorkPackage NullWorkPackage;

	/// Default value of the local work size. Also known as workgroup size.
	static const unsigned defaultLocalWorkSize;
	/// Default value of the global work size as a multiplier of the local work size
	static const unsigned defaultGlobalWorkSizeMultiplier;
	/// Default value of the milliseconds per global work size (per batch)
	static const unsigned defaultMSPerBatch;
};

}
}
