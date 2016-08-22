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
/** @file ElhashAux.cpp
 * 
 * 
 */

#include "ElhashAux.h"
#include <boost/detail/endian.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <array>
#include <thread>
#include <libelhash/internal.h>
#include <libdevcore/Common.h>
#include <libdevcore/Guards.h>
#include <libdevcore/Log.h>
#include <libdevcrypto/CryptoPP.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/FileSystem.h>
#include <libelecore/Exceptions.h>
#include <libelecore/BlockHeader.h>
#include "Elhash.h"
using namespace std;
using namespace chrono;
using namespace dev;
using namespace ele;

const char* DAGChannel::name() { return EleGreen "DAG"; }

ElhashAux* dev::ele::ElhashAux::s_this = nullptr;

ElhashAux::~ElhashAux()
{
}

ElhashAux* ElhashAux::get()
{
	static std::once_flag flag;
	std::call_once(flag, []{s_this = new ElhashAux();});
	return s_this;
}

uint64_t ElhashAux::cacheSize(BlockHeader const& _header)
{
	return elhash_get_cachesize((uint64_t)_header.number());
}

uint64_t ElhashAux::dataSize(uint64_t _blockNumber)
{
	return elhash_get_datasize(_blockNumber);
}

h256 ElhashAux::seedHash(unsigned _number)
{
	unsigned epoch = _number / ELHASH_EPOCH_LENGTH;
	Guard l(get()->x_epochs);
	if (epoch >= get()->m_seedHashes.size())
	{
		h256 ret;
		unsigned n = 0;
		if (!get()->m_seedHashes.empty())
		{
			ret = get()->m_seedHashes.back();
			n = get()->m_seedHashes.size() - 1;
		}
		get()->m_seedHashes.resize(epoch + 1);
//		cdebug << "Searching for seedHash of epoch " << epoch;
		for (; n <= epoch; ++n, ret = sha3(ret))
		{
			get()->m_seedHashes[n] = ret;
//			cdebug << "Epoch" << n << "is" << ret;
		}
	}
	return get()->m_seedHashes[epoch];
}

uint64_t ElhashAux::number(h256 const& _seedHash)
{
	Guard l(get()->x_epochs);
	unsigned epoch = 0;
	auto epochIter = get()->m_epochs.find(_seedHash);
	if (epochIter == get()->m_epochs.end())
	{
		//		cdebug << "Searching for seedHash " << _seedHash;
		for (h256 h; h != _seedHash && epoch < 2048; ++epoch, h = sha3(h), get()->m_epochs[h] = epoch) {}
		if (epoch == 2048)
		{
			std::ostringstream error;
			error << "apparent block number for " << _seedHash << " is too high; max is " << (ELHASH_EPOCH_LENGTH * 2048);
			throw std::invalid_argument(error.str());
		}
	}
	else
		epoch = epochIter->second;
	return epoch * ELHASH_EPOCH_LENGTH;
}

void ElhashAux::killCache(h256 const& _s)
{
	WriteGuard l(x_lights);
	m_lights.erase(_s);
}

ElhashAux::LightType ElhashAux::light(h256 const& _seedHash)
{
	UpgradableGuard l(get()->x_lights);
	if (get()->m_lights.count(_seedHash))
		return get()->m_lights.at(_seedHash);
	UpgradeGuard l2(l);
	return (get()->m_lights[_seedHash] = make_shared<LightAllocation>(_seedHash));
}

ElhashAux::LightAllocation::LightAllocation(h256 const& _seedHash)
{
	uint64_t blockNumber = ElhashAux::number(_seedHash);
	light = elhash_light_new(blockNumber);
	if (!light)
		BOOST_THROW_EXCEPTION(ExternalFunctionFailure("elhash_light_new()"));
	size = elhash_get_cachesize(blockNumber);
}

ElhashAux::LightAllocation::~LightAllocation()
{
	elhash_light_delete(light);
}

bytesConstRef ElhashAux::LightAllocation::data() const
{
	return bytesConstRef((byte const*)light->cache, size);
}

ElhashAux::FullAllocation::FullAllocation(elhash_light_t _light, elhash_callback_t _cb)
{
//	cdebug << "About to call elhash_full_new...";
	full = elhash_full_new(_light, _cb);
//	cdebug << "Called OK.";
	if (!full)
	{
		clog(DAGChannel) << "DAG Generation Failure. Reason: "  << strerror(errno);
		BOOST_THROW_EXCEPTION(ExternalFunctionFailure("elhash_full_new"));
	}
}

ElhashAux::FullAllocation::~FullAllocation()
{
	elhash_full_delete(full);
}

bytesConstRef ElhashAux::FullAllocation::data() const
{
	return bytesConstRef((byte const*)elhash_full_dag(full), size());
}

static std::function<int(unsigned)> s_dagCallback;
static int dagCallbackShim(unsigned _p)
{
	clog(DAGChannel) << "Generating DAG file. Progress: " << toString(_p) << "%";
	return s_dagCallback ? s_dagCallback(_p) : 0;
}

ElhashAux::FullType ElhashAux::full(h256 const& _seedHash, bool _createIfMissing, function<int(unsigned)> const& _f)
{
	FullType ret;
	auto l = light(_seedHash);

	DEV_GUARDED(get()->x_fulls)
		if ((ret = get()->m_fulls[_seedHash].lock()))
		{
			get()->m_lastUsedFull = ret;
			return ret;
		}

	if (_createIfMissing || computeFull(_seedHash, false) == 100)
	{
		s_dagCallback = _f;
//		cnote << "Loading from libelhash...";
		ret = make_shared<FullAllocation>(l->light, dagCallbackShim);
//		cnote << "Done loading.";

		DEV_GUARDED(get()->x_fulls)
			get()->m_fulls[_seedHash] = get()->m_lastUsedFull = ret;
	}

	return ret;
}

unsigned ElhashAux::computeFull(h256 const& _seedHash, bool _createIfMissing)
{
	Guard l(get()->x_fulls);
	uint64_t blockNumber;

	DEV_IF_THROWS(blockNumber = ElhashAux::number(_seedHash))
	{
		return 0;
	}

	if (FullType ret = get()->m_fulls[_seedHash].lock())
	{
		get()->m_lastUsedFull = ret;
		return 100;
	}

	if (_createIfMissing && (!get()->m_fullGenerator || !get()->m_fullGenerator->joinable()))
	{
		get()->m_fullProgress = 0;
		get()->m_generatingFullNumber = blockNumber / ELHASH_EPOCH_LENGTH * ELHASH_EPOCH_LENGTH;
		get()->m_fullGenerator = unique_ptr<thread>(new thread([=](){
			cnote << "Loading full DAG of seedhash: " << _seedHash;
			get()->full(_seedHash, true, [](unsigned p){ get()->m_fullProgress = p; return 0; });
			cnote << "Full DAG loaded";
			get()->m_fullProgress = 0;
			get()->m_generatingFullNumber = NotGenerating;
		}));
	}

	return (get()->m_generatingFullNumber == blockNumber) ? get()->m_fullProgress : 0;
}

ElhashProofOfWork::Result ElhashAux::FullAllocation::compute(h256 const& _headerHash, Nonce const& _nonce) const
{
	elhash_return_value_t r = elhash_full_compute(full, *(elhash_h256_t*)_headerHash.data(), (uint64_t)(u64)_nonce);
	if (!r.success)
		BOOST_THROW_EXCEPTION(DAGCreationFailure());
	return ElhashProofOfWork::Result{h256((uint8_t*)&r.result, h256::ConstructFromPointer), h256((uint8_t*)&r.mix_hash, h256::ConstructFromPointer)};
}

ElhashProofOfWork::Result ElhashAux::LightAllocation::compute(h256 const& _headerHash, Nonce const& _nonce) const
{
	elhash_return_value r = elhash_light_compute(light, *(elhash_h256_t*)_headerHash.data(), (uint64_t)(u64)_nonce);
	if (!r.success)
		BOOST_THROW_EXCEPTION(DAGCreationFailure());
	return ElhashProofOfWork::Result{h256((uint8_t*)&r.result, h256::ConstructFromPointer), h256((uint8_t*)&r.mix_hash, h256::ConstructFromPointer)};
}

ElhashProofOfWork::Result ElhashAux::eval(h256 const& _seedHash, h256 const& _headerHash, Nonce const& _nonce)
{
	DEV_GUARDED(get()->x_fulls)
		if (FullType dag = get()->m_fulls[_seedHash].lock())
			return dag->compute(_headerHash, _nonce);
	DEV_IF_THROWS(return ElhashAux::get()->light(_seedHash)->compute(_headerHash, _nonce))
	{
		return ElhashProofOfWork::Result{ ~h256(), h256() };
	}
}
