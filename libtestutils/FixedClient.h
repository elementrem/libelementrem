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
/** @file FixedClient.h
 * 
 * 
 */

#pragma once

#include <libelementrem/ClientBase.h>
#include <libelementrem/BlockChain.h>
#include <libelementrem/State.h>

namespace dev
{
namespace test
{

/**
 * @brief mvp implementation of ClientBase
 * Doesn't support mining interface
 */
class FixedClient: public dev::ele::ClientBase
{
public:
	FixedClient(ele::BlockChain const& _bc, ele::Block const& _block) :  m_bc(_bc), m_block(_block) {}
	virtual ~FixedClient() {}
	
	// stub
	virtual void flushTransactions() override {}
	virtual ele::BlockChain& bc() override { BOOST_THROW_EXCEPTION(InterfaceNotSupported("FixedClient::bc()")); }
	virtual ele::BlockChain const& bc() const override { return m_bc; }
	using ClientBase::asOf;
	virtual ele::Block asOf(h256 const& _h) const override;
	virtual ele::Block preSeal() const override { ReadGuard l(x_stateDB); return m_block; }
	virtual ele::Block postSeal() const override { ReadGuard l(x_stateDB); return m_block; }
	virtual void setAuthor(Address const& _us) override { WriteGuard l(x_stateDB); m_block.setAuthor(_us); }
	virtual void prepareForTransaction() override {}

private:
	ele::BlockChain const& m_bc;
	ele::Block m_block;
	mutable SharedMutex x_stateDB;			///< Lock on the state DB, effectively a lock on m_postSeal.
};

}
}
