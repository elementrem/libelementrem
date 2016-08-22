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
/** @file BasicGasPricer.h
 * 
 * 
 */

#pragma once

#include <array>
#include "GasPricer.h"

namespace dev
{
namespace ele
{

class BasicGasPricer: public GasPricer
{
public:
	explicit BasicGasPricer(u256 _meyPerRef, u256 _refsPerBlock): m_meyPerRef(_meyPerRef), m_refsPerBlock(_refsPerBlock) {}

	void setRefPrice(u256 _meyPerRef) { if ((bigint)m_refsPerBlock * _meyPerRef > std::numeric_limits<u256>::max() ) BOOST_THROW_EXCEPTION(Overflow() << errinfo_comment("element price * block fees is larger than 2**256-1, choose a smaller number.") ); else m_meyPerRef = _meyPerRef; }
	void setRefBlockFees(u256 _refsPerBlock) { if ((bigint)m_meyPerRef * _refsPerBlock > std::numeric_limits<u256>::max() ) BOOST_THROW_EXCEPTION(Overflow() << errinfo_comment("element price * block fees is larger than 2**256-1, choose a smaller number.") ); else m_refsPerBlock = _refsPerBlock; }

	u256 ask(Block const&) const override { return m_meyPerRef * m_refsPerBlock / m_gasPerBlock; }
	u256 bid(TransactionPriority _p = TransactionPriority::Medium) const override { return m_octiles[(int)_p] > 0 ? m_octiles[(int)_p] : (m_meyPerRef * m_refsPerBlock / m_gasPerBlock); }

	void update(BlockChain const& _bc) override;

private:
	u256 m_meyPerRef;
	u256 m_refsPerBlock;
	u256 m_gasPerBlock = DefaultBlockGasLimit;
	std::array<u256, 9> m_octiles;
};

}
}
