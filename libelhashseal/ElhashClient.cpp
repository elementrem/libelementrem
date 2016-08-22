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
/** @file ElhashClient.cpp
 * 
 * 
 */

#include "ElhashClient.h"
#include "Elhash.h"
using namespace std;
using namespace dev;
using namespace dev::ele;
using namespace p2p;

ElhashClient& dev::ele::asElhashClient(Interface& _c)
{
	if (dynamic_cast<Elhash*>(_c.sealEngine()))
		return dynamic_cast<ElhashClient&>(_c);
	throw InvalidSealEngine();
}

ElhashClient* dev::ele::asElhashClient(Interface* _c)
{
	if (dynamic_cast<Elhash*>(_c->sealEngine()))
		return &dynamic_cast<ElhashClient&>(*_c);
	throw InvalidSealEngine();
}

DEV_SIMPLE_EXCEPTION(ChainParamsNotElhash);

ElhashClient::ElhashClient(
	ChainParams const& _params,
	int _networkID,
	p2p::Host* _host,
	std::shared_ptr<GasPricer> _gpForAdoption,
	std::string const& _dbPath,
	WithExisting _forceAction,
	TransactionQueue::Limits const& _limits
):
	Client(_params, _networkID, _host, _gpForAdoption, _dbPath, _forceAction, _limits)
{
	// will throw if we're not an Elhash seal engine.
	asElhashClient(*this);
}

Elhash* ElhashClient::elhash() const
{
	return dynamic_cast<Elhash*>(Client::sealEngine());
}

bool ElhashClient::isMining() const
{
	return elhash()->farm().isMining();
}

WorkingProgress ElhashClient::miningProgress() const
{
	if (isMining())
		return elhash()->farm().miningProgress();
	return WorkingProgress();
}

u256 ElhashClient::hashrate() const
{
	u256 r = externalHashrate();
	if (isMining())
		r += miningProgress().rate();
	return r;
}

std::tuple<h256, h256, h256> ElhashClient::getElhashWork()
{
	// lock the work so a later submission isn't invalidated by processing a transaction elsewhere.
	// this will be reset as soon as a new block arrives, allowing more transactions to be processed.
	bool oldShould = shouldServeWork();
	m_lastGetWork = chrono::system_clock::now();

	if (!sealEngine()->shouldSeal(this))
		return std::tuple<h256, h256, h256>();

	// if this request has made us bother to serve work, prep it now.
	if (!oldShould && shouldServeWork())
		onPostStateChanged();
	else
		// otherwise, set this to true so that it gets prepped next time.
		m_remoteWorking = true;
	elhash()->manuallySetWork(m_sealingInfo);
	return std::tuple<h256, h256, h256>(m_sealingInfo.hash(WithoutSeal), Elhash::seedHash(m_sealingInfo), Elhash::boundary(m_sealingInfo));
}

bool ElhashClient::submitElhashWork(h256 const& _mixHash, h64 const& _nonce)
{
	elhash()->manuallySubmitWork(_mixHash, _nonce);
	return true;
}

void ElhashClient::setShouldPrecomputeDAG(bool _precompute)
{
	bytes trueBytes {1};
	bytes falseBytes {0};
	sealEngine()->setOption("precomputeDAG", _precompute ? trueBytes: falseBytes);
}

void ElhashClient::submitExternalHashrate(u256 const& _rate, h256 const& _id)
{
	WriteGuard(x_externalRates);
	m_externalRates[_id] = make_pair(_rate, chrono::steady_clock::now());
}

u256 ElhashClient::externalHashrate() const
{
	u256 ret = 0;
	WriteGuard(x_externalRates);
	for (auto i = m_externalRates.begin(); i != m_externalRates.end();)
		if (chrono::steady_clock::now() - i->second.second > chrono::seconds(5))
			i = m_externalRates.erase(i);
		else
			ret += i++->second.first;
	return ret;
}
