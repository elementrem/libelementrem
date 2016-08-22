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
/** @file ElhashGPUMiner.cpp
 * 
 * 
 *
 * Determines the PoW algorithm.
 */

#if ELE_ELHASHCL

#include "ElhashGPUMiner.h"
#include <thread>
#include <chrono>
#include <libelhash-cl/elhash_cl_miner.h>

using namespace std;
using namespace dev;
using namespace ele;

namespace dev
{
namespace ele
{

class ElhashCLHook: public elhash_cl_miner::search_hook
{
public:
	ElhashCLHook(ElhashGPUMiner* _owner): m_owner(_owner) {}
	ElhashCLHook(ElhashCLHook const&) = delete;

	void abort()
	{
		{
			UniqueGuard l(x_all);
			if (m_aborted)
				return;
//		cdebug << "Attempting to abort";

			m_abort = true;
		}
		// m_abort is true so now searched()/found() will return true to abort the search.
		// we hang around on this thread waiting for them to point out that they have aborted since
		// otherwise we may end up deleting this object prior to searched()/found() being called.
		m_aborted.wait(true);
//		for (unsigned timeout = 0; timeout < 100 && !m_aborted; ++timeout)
//			std::this_thread::sleep_for(chrono::milliseconds(30));
//		if (!m_aborted)
//			cwarn << "Couldn't abort. Abandoning OpenCL process.";
	}

	void reset()
	{
		UniqueGuard l(x_all);
		m_aborted = m_abort = false;
	}

protected:
	virtual bool found(uint64_t const* _nonces, uint32_t _count) override
	{
//		dev::operator <<(std::cerr << "Found nonces: ", vector<uint64_t>(_nonces, _nonces + _count)) << std::endl;
		for (uint32_t i = 0; i < _count; ++i)
			if (m_owner->report(_nonces[i]))
				return (m_aborted = true);
		return m_owner->shouldStop();
	}

	virtual bool searched(uint64_t _startNonce, uint32_t _count) override
	{
		UniqueGuard l(x_all);
//		std::cerr << "Searched " << _count << " from " << _startNonce << std::endl;
		m_owner->accumulateHashes(_count);
		m_last = _startNonce + _count;
		if (m_abort || m_owner->shouldStop())
			return (m_aborted = true);
		return false;
	}

private:
	Mutex x_all;
	uint64_t m_last;
	bool m_abort = false;
	Notified<bool> m_aborted = {true};
	ElhashGPUMiner* m_owner = nullptr;
};

}
}

unsigned ElhashGPUMiner::s_platformId = 0;
unsigned ElhashGPUMiner::s_deviceId = 0;
unsigned ElhashGPUMiner::s_numInstances = 0;

ElhashGPUMiner::ElhashGPUMiner(ConstructionInfo const& _ci):
	GenericMiner<ElhashProofOfWork>(_ci),
	Worker("gpuminer" + toString(index())),
	m_hook(new ElhashCLHook(this))
{
}

ElhashGPUMiner::~ElhashGPUMiner()
{
	pause();
	delete m_miner;
	delete m_hook;
}

bool ElhashGPUMiner::report(uint64_t _nonce)
{
	Nonce n = (Nonce)(u64)_nonce;
	ElhashProofOfWork::Result r = ElhashAux::eval(work().seedHash, work().headerHash, n);
	if (r.value < work().boundary)
		return submitProof(Solution{n, r.mixHash});
	return false;
}

void ElhashGPUMiner::kickOff()
{
	m_hook->reset();
	startWorking();
}

void ElhashGPUMiner::workLoop()
{
	// take local copy of work since it may end up being overwritten by kickOff/pause.
	try {
		WorkPackage w = work();
		cnote << "workLoop" << !!m_miner << m_minerSeed << w.seedHash;
		if (!m_miner || m_minerSeed != w.seedHash)
		{
			cnote << "Initialising miner...";
			m_minerSeed = w.seedHash;

			delete m_miner;
			m_miner = new elhash_cl_miner;

			unsigned device = instances() > 1 ? index() : s_deviceId;

			ElhashAux::FullType dag;
			while (true)
			{
				if ((dag = ElhashAux::full(w.seedHash, true)))
					break;
				if (shouldStop())
				{
					delete m_miner;
					m_miner = nullptr;
					return;
				}
				cnote << "Awaiting DAG";
				this_thread::sleep_for(chrono::milliseconds(500));
			}
			bytesConstRef dagData = dag->data();
			m_miner->init(dagData.data(), dagData.size(), s_platformId, device);
		}

		uint64_t upper64OfBoundary = (uint64_t)(u64)((u256)w.boundary >> 192);
		m_miner->search(w.headerHash.data(), upper64OfBoundary, *m_hook);
	}
	catch (cl::Error const& _e)
	{
		delete m_miner;
		m_miner = nullptr;
		cwarn << "Error GPU mining: " << _e.what() << "(" << _e.err() << ")";
	}
}

void ElhashGPUMiner::pause()
{
	m_hook->abort();
	stopWorking();
}

std::string ElhashGPUMiner::platformInfo()
{
	return elhash_cl_miner::platform_info(s_platformId, s_deviceId);
}

unsigned ElhashGPUMiner::getNumDevices()
{
	return elhash_cl_miner::getNumDevices(s_platformId);
}

void ElhashGPUMiner::listDevices()
{
	return elhash_cl_miner::listDevices();
}

bool ElhashGPUMiner::configureGPU(
	unsigned _localWorkSize,
	unsigned _globalWorkSizeMultiplier,
	unsigned _msPerBatch,
	unsigned _platformId,
	unsigned _deviceId,
	bool _allowCPU,
	unsigned _extraGPUMemory,
	uint64_t _currentBlock
)
{
	s_platformId = _platformId;
	s_deviceId = _deviceId;

	if (_localWorkSize != 32 && _localWorkSize != 64 && _localWorkSize != 128 && _localWorkSize != 256)
	{
		cout << "Given localWorkSize of " << toString(_localWorkSize) << " is invalid. Must be either 32, 64, 128 or 256" << endl;
		return false;
	}

	if (!elhash_cl_miner::configureGPU(
			_platformId,
			_localWorkSize,
			_globalWorkSizeMultiplier * _localWorkSize,
			_msPerBatch,
			_allowCPU,
			_extraGPUMemory,
			_currentBlock)
	)
	{
		cout << "No GPU device with sufficient memory was found. Can't GPU mine. Remove the -G argument" << endl;
		return false;
	}
	return true;
}

#endif // ELE_ELHASHCL
