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
/** @file ElhashGPUMiner.h
 * 
 * 
 *
 * Determines the PoW algorithm.
 */

#pragma once

#if ELE_ELHASHCL

#include "libdevcore/Worker.h"
#include <libelementrem/GenericMiner.h>
#include "ElhashAux.h"

class elhash_cl_miner;

namespace dev
{
namespace ele
{

class ElhashCLHook;

class ElhashGPUMiner: public GenericMiner<ElhashProofOfWork>, Worker
{
	friend class dev::ele::ElhashCLHook;

public:
	ElhashGPUMiner(ConstructionInfo const& _ci);
	~ElhashGPUMiner();

	static unsigned instances() { return s_numInstances > 0 ? s_numInstances : 1; }
	static std::string platformInfo();
	static unsigned getNumDevices();
	static void listDevices();
	static bool configureGPU(
		unsigned _localWorkSize,
		unsigned _globalWorkSizeMultiplier,
		unsigned _msPerBatch,
		unsigned _platformId,
		unsigned _deviceId,
		bool _allowCPU,
		unsigned _extraGPUMemory,
		uint64_t _currentBlock
	);
	static void setNumInstances(unsigned _instances) { s_numInstances = std::min<unsigned>(_instances, getNumDevices()); }

protected:
	void kickOff() override;
	void pause() override;

private:
	void workLoop() override;
	bool report(uint64_t _nonce);

	using GenericMiner<ElhashProofOfWork>::accumulateHashes;

	ElhashCLHook* m_hook = nullptr;
	elhash_cl_miner* m_miner = nullptr;

	h256 m_minerSeed;		///< Last seed in m_miner
	static unsigned s_platformId;
	static unsigned s_deviceId;
	static unsigned s_numInstances;
};

}
}

#endif // ELE_ELHASHCL
