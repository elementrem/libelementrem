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
/** @file ElhashProofOfWork.cpp
 * 
 * 
 *
 * Determines the PoW algorithm.
 */

#include "ElhashProofOfWork.h"
#include "Elhash.h"
using namespace std;
using namespace chrono;
using namespace dev;
using namespace ele;

const unsigned ElhashProofOfWork::defaultLocalWorkSize = 64;
const unsigned ElhashProofOfWork::defaultGlobalWorkSizeMultiplier = 4096; // * CL_DEFAULT_LOCAL_WORK_SIZE
const unsigned ElhashProofOfWork::defaultMSPerBatch = 0;
const ElhashProofOfWork::WorkPackage ElhashProofOfWork::NullWorkPackage = ElhashProofOfWork::WorkPackage();

ElhashProofOfWork::WorkPackage::WorkPackage(BlockHeader const& _bh):
	boundary(Elhash::boundary(_bh)),
	headerHash(_bh.hash(WithoutSeal)),
	seedHash(Elhash::seedHash(_bh))
{}
