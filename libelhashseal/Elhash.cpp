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
/** @file Elhash.cpp
 * 
 * 
 */

#include "Elhash.h"
#include <libelhash/elhash.h>
#include <libelhash/internal.h>
#include <libelementrem/Interface.h>
#include <libelecore/ChainOperationParams.h>
#include <libelecore/CommonJS.h>
#include "ElhashCPUMiner.h"
#include "ElhashGPUMiner.h"
using namespace std;
using namespace dev;
using namespace ele;

void Elhash::init()
{
	ELE_REGISTER_SEAL_ENGINE(Elhash);
}

Elhash::Elhash()
{
	map<string, GenericFarm<ElhashProofOfWork>::SealerDescriptor> sealers;
	sealers["cpu"] = GenericFarm<ElhashProofOfWork>::SealerDescriptor{&ElhashCPUMiner::instances, [](GenericMiner<ElhashProofOfWork>::ConstructionInfo ci){ return new ElhashCPUMiner(ci); }};
#if ELE_ELHASHCL
	sealers["opencl"] = GenericFarm<ElhashProofOfWork>::SealerDescriptor{&ElhashGPUMiner::instances, [](GenericMiner<ElhashProofOfWork>::ConstructionInfo ci){ return new ElhashGPUMiner(ci); }};
#endif
	m_farm.setSealers(sealers);
	m_farm.onSolutionFound([=](ElhashProofOfWork::Solution const& sol)
	{
//		cdebug << m_farm.work().seedHash << m_farm.work().headerHash << sol.nonce << ElhashAux::eval(m_farm.work().seedHash, m_farm.work().headerHash, sol.nonce).value;
		setMixHash(m_sealing, sol.mixHash);
		setNonce(m_sealing, sol.nonce);
		if (!quickVerifySeal(m_sealing))
			return false;

		if (m_onSealGenerated)
		{
			RLPStream ret;
			m_sealing.streamRLP(ret);
			m_onSealGenerated(ret.out());
		}
		return true;
	});
}

strings Elhash::sealers() const
{
	return {
		"cpu"
#if ELE_ELHASHCL
		, "opencl"
#endif
	};
}

h256 Elhash::seedHash(BlockHeader const& _bi)
{
	return ElhashAux::seedHash((unsigned)_bi.number());
}

StringHashMap Elhash::jsInfo(BlockHeader const& _bi) const
{
	return { { "nonce", toJS(nonce(_bi)) }, { "seedHash", toJS(seedHash(_bi)) }, { "mixHash", toJS(mixHash(_bi)) }, { "boundary", toJS(boundary(_bi)) }, { "difficulty", toJS(_bi.difficulty()) } };
}

EVMSchedule const& Elhash::evmSchedule(EnvInfo const& _envInfo) const
{
	if (_envInfo.number() >= chainParams().u256Param("frontierCompatibilityModeLimit"))
		return HomesteadSchedule;
	else
		return FrontierSchedule;
}

void Elhash::verify(Strictness _s, BlockHeader const& _bi, BlockHeader const& _parent, bytesConstRef _block) const
{
	SealEngineFace::verify(_s, _bi, _parent, _block);

	if (_s != CheckNothingNew)
	{
		if (_bi.difficulty() < chainParams().u256Param("minimumDifficulty"))
			BOOST_THROW_EXCEPTION(InvalidDifficulty() << RequirementError(bigint(chainParams().u256Param("minimumDifficulty")), bigint(_bi.difficulty())) );

		if (_bi.gasLimit() < chainParams().u256Param("minGasLimit"))
			BOOST_THROW_EXCEPTION(InvalidGasLimit() << RequirementError(bigint(chainParams().u256Param("minGasLimit")), bigint(_bi.gasLimit())) );

		if (_bi.gasLimit() > chainParams().u256Param("maxGasLimit"))
			BOOST_THROW_EXCEPTION(InvalidGasLimit() << RequirementError(bigint(chainParams().u256Param("maxGasLimit")), bigint(_bi.gasLimit())) );

		if (_bi.number() && _bi.extraData().size() > chainParams().maximumExtraDataSize)
			BOOST_THROW_EXCEPTION(ExtraDataTooBig() << RequirementError(bigint(chainParams().maximumExtraDataSize), bigint(_bi.extraData().size())) << errinfo_extraData(_bi.extraData()));
	}

	if (_parent)
	{
		// Check difficulty is correct given the two timestamps.
		auto expected = calculateDifficulty(_bi, _parent);
		auto difficulty = _bi.difficulty();
		if (difficulty != expected)
			BOOST_THROW_EXCEPTION(InvalidDifficulty() << RequirementError((bigint)expected, (bigint)difficulty));

		auto gasLimit = _bi.gasLimit();
		auto parentGasLimit = _parent.gasLimit();
		if (
			gasLimit < chainParams().u256Param("minGasLimit") ||
			gasLimit > chainParams().u256Param("maxGasLimit") ||
			gasLimit <= parentGasLimit - parentGasLimit / chainParams().u256Param("gasLimitBoundDivisor") ||
			gasLimit >= parentGasLimit + parentGasLimit / chainParams().u256Param("gasLimitBoundDivisor"))
			BOOST_THROW_EXCEPTION(
				InvalidGasLimit()
				<< errinfo_min((bigint)((bigint)parentGasLimit - (bigint)(parentGasLimit / chainParams().u256Param("gasLimitBoundDivisor"))))
				<< errinfo_got((bigint)gasLimit)
				<< errinfo_max((bigint)((bigint)parentGasLimit + parentGasLimit / chainParams().u256Param("gasLimitBoundDivisor")))
			);
	}

	// check it hashes according to proof of work or that it's the genesis block.
	if (_s == CheckEverything && _bi.parentHash() && !verifySeal(_bi))
	{
		InvalidBlockNonce ex;
		ex << errinfo_nonce(nonce(_bi));
		ex << errinfo_mixHash(mixHash(_bi));
		ex << errinfo_seedHash(seedHash(_bi));
		ElhashProofOfWork::Result er = ElhashAux::eval(seedHash(_bi), _bi.hash(WithoutSeal), nonce(_bi));
		ex << errinfo_elhashResult(make_tuple(er.value, er.mixHash));
		ex << errinfo_hash256(_bi.hash(WithoutSeal));
		ex << errinfo_difficulty(_bi.difficulty());
		ex << errinfo_target(boundary(_bi));
		BOOST_THROW_EXCEPTION(ex);
	}
	else if (_s == QuickNonce && _bi.parentHash() && !quickVerifySeal(_bi))
	{
		InvalidBlockNonce ex;
		ex << errinfo_hash256(_bi.hash(WithoutSeal));
		ex << errinfo_difficulty(_bi.difficulty());
		ex << errinfo_nonce(nonce(_bi));
		BOOST_THROW_EXCEPTION(ex);
	}
}

void Elhash::verifyTransaction(ImportRequirements::value _ir, TransactionBase const& _t, BlockHeader const& _bi) const
{
	if (_ir & ImportRequirements::TransactionSignatures && _bi.number() >= chainParams().u256Param("frontierCompatibilityModeLimit"))
		_t.checkLowS();
	// Unneeded as it's checked again in Executive. Keep it here since tests assume it's checked.
	if (_ir & ImportRequirements::TransactionBasic && _t.gasRequired(evmSchedule(EnvInfo(_bi))) > _t.gas())
		BOOST_THROW_EXCEPTION(OutOfGasIntrinsic());
}

u256 Elhash::childGasLimit(BlockHeader const& _bi, u256 const& _gasFloorTarget) const
{
	u256 gasFloorTarget = _gasFloorTarget == Invalid256 ? 3141562 : _gasFloorTarget;
	u256 gasLimit = _bi.gasLimit();
	u256 boundDivisor = chainParams().u256Param("gasLimitBoundDivisor");
	if (gasLimit < gasFloorTarget)
		return min<u256>(gasFloorTarget, gasLimit + gasLimit / boundDivisor - 1);
	else
		return max<u256>(gasFloorTarget, gasLimit - gasLimit / boundDivisor + 1 + (_bi.gasUsed() * 6 / 5) / boundDivisor);
}

void Elhash::manuallySubmitWork(const h256& _mixHash, Nonce _nonce)
{
	m_farm.submitProof(ElhashProofOfWork::Solution{_nonce, _mixHash}, nullptr);
}

u256 Elhash::calculateDifficulty(BlockHeader const& _bi, BlockHeader const& _parent) const
{
	const unsigned c_expDiffPeriod = 100000;

	if (!_bi.number())
		throw GenesisBlockCannotBeCalculated();
	auto minimumDifficulty = chainParams().u256Param("minimumDifficulty");
	auto difficultyBoundDivisor = chainParams().u256Param("difficultyBoundDivisor");
	auto durationLimit = chainParams().u256Param("durationLimit");

	bigint target;	// stick to a bigint for the target. Don't want to risk going negative.
	if (_bi.number() < chainParams().u256Param("frontierCompatibilityModeLimit"))
		// Frontier-era difficulty adjustment
		target = _bi.timestamp() >= _parent.timestamp() + durationLimit ? _parent.difficulty() - (_parent.difficulty() / difficultyBoundDivisor) : (_parent.difficulty() + (_parent.difficulty() / difficultyBoundDivisor));
	else
		// Homestead-era difficulty adjustment
		target = _parent.difficulty() + _parent.difficulty() / 2048 * max<bigint>(1 - (bigint(_bi.timestamp()) - _parent.timestamp()) / 10, -99);

	bigint o = target;
	unsigned periodCount = unsigned(_parent.number() + 1) / c_expDiffPeriod;
	if (periodCount > 1)
		o += (bigint(1) << (periodCount - 2));	// latter will eventually become huge, so ensure it's a bigint.

	o = max<bigint>(minimumDifficulty, o);
	return u256(min<bigint>(o, std::numeric_limits<u256>::max()));
}

void Elhash::populateFromParent(BlockHeader& _bi, BlockHeader const& _parent) const
{
	SealEngineFace::populateFromParent(_bi, _parent);
	_bi.setDifficulty(calculateDifficulty(_bi, _parent));
	_bi.setGasLimit(childGasLimit(_parent));
}

bool Elhash::quickVerifySeal(BlockHeader const& _bi) const
{
	if (_bi.number() >= ELHASH_EPOCH_LENGTH * 2048)
		return false;

	auto h = _bi.hash(WithoutSeal);
	auto m = mixHash(_bi);
	auto n = nonce(_bi);
	auto b = boundary(_bi);
	bool ret = !!elhash_quick_check_difficulty(
		(elhash_h256_t const*)h.data(),
		(uint64_t)(u64)n,
		(elhash_h256_t const*)m.data(),
		(elhash_h256_t const*)b.data());
	return ret;
}

bool Elhash::verifySeal(BlockHeader const& _bi) const
{
	bool pre = quickVerifySeal(_bi);
#if !ELE_DEBUG
	if (!pre)
	{
		cwarn << "Fail on preVerify";
		return false;
	}
#endif

	auto result = ElhashAux::eval(seedHash(_bi), _bi.hash(WithoutSeal), nonce(_bi));
	bool slow = result.value <= boundary(_bi) && result.mixHash == mixHash(_bi);

#if ELE_DEBUG
	if (!pre && slow)
	{
		cwarn << "WARNING: evaluated result gives true whereas elhash_quick_check_difficulty gives false.";
		cwarn << "headerHash:" << _bi.hash(WithoutSeal);
		cwarn << "nonce:" << nonce(_bi);
		cwarn << "mixHash:" << mixHash(_bi);
		cwarn << "difficulty:" << _bi.difficulty();
		cwarn << "boundary:" << boundary(_bi);
		cwarn << "result.value:" << result.value;
		cwarn << "result.mixHash:" << result.mixHash;
	}
#endif // ELE_DEBUG

	return slow;
}

void Elhash::generateSeal(BlockHeader const& _bi)
{
	m_sealing = _bi;
	m_farm.setWork(m_sealing);
	m_farm.start(m_sealer);
	m_farm.setWork(m_sealing);		// TODO: take out one before or one after...
	bytes shouldPrecompute = option("precomputeDAG");
	if (!shouldPrecompute.empty() && shouldPrecompute[0] == 1)
		ensurePrecomputed((unsigned)_bi.number());
}

void Elhash::onSealGenerated(std::function<void(bytes const&)> const& _f)
{
	m_onSealGenerated = _f;
}

bool Elhash::shouldSeal(Interface*)
{
	return true;
}

void Elhash::ensurePrecomputed(unsigned _number)
{
	if (_number % ELHASH_EPOCH_LENGTH > ELHASH_EPOCH_LENGTH * 9 / 10)
		// 90% of the way to the new epoch
		ElhashAux::computeFull(ElhashAux::seedHash(_number + ELHASH_EPOCH_LENGTH), true);
}
