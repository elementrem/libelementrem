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
/** @file BlockChainLoader.h
 * 
 * 
 */

#pragma once
#include <string>
#include <json/json.h>
#include <libdevcore/TransientDirectory.h>
#include <libelementrem/BlockChain.h>
#include <libelementrem/Block.h>
#include <libelhashseal/GenesisInfo.h>

namespace dev
{
namespace test
{

/**
 * @brief Should be used to load test blockchain from json file
 * Loads the blockchain from json, creates temporary directory to store it, removes the directory on dealloc
 */
class BlockChainLoader
{
public:
	BlockChainLoader(Json::Value const& _json, ele::Network _sealEngineNetwork = ele::Network::Test);
	ele::BlockChain const& bc() const { return *m_bc; }
	ele::State const& state() const { return m_block.state(); }	// TODO remove?
	ele::Block const& block() const { return m_block; }

private:
	TransientDirectory m_dir;
	std::unique_ptr<ele::BlockChain> m_bc;
	ele::Block m_block;
};

}
}
