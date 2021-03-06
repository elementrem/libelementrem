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
#pragma once

#include "VMFace.h"

namespace dev
{
namespace ele
{

/// Smart VM proxy.
///
/// This class is a strategy pattern implementation for VM. For every EVM code
/// execution request it tries to select the best VM implementation (Interpreter or JIT)
/// by analyzing available information like: code size, hit count, JIT status, etc.
class SmartVM: public VMFace
{
public:
	virtual bytesConstRef execImpl(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp) override final;

private:
	std::unique_ptr<VMFace> m_selectedVM;
};

}
}
