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

#include <memory>
#include <libdevcore/Exceptions.h>
#include "ExtVMFace.h"

namespace dev
{
namespace ele
{

struct VMException: virtual Exception {};
#define ELE_SIMPLE_EXCEPTION_VM(X) struct X: virtual VMException { const char* what() const noexcept override { return #X; } }
ELE_SIMPLE_EXCEPTION_VM(BreakPointHit);
ELE_SIMPLE_EXCEPTION_VM(BadInstruction);
ELE_SIMPLE_EXCEPTION_VM(BadJumpDestination);
ELE_SIMPLE_EXCEPTION_VM(OutOfGas);
ELE_SIMPLE_EXCEPTION_VM(OutOfStack);
ELE_SIMPLE_EXCEPTION_VM(StackUnderflow);

/// EVM Virtual Machine interface
class VMFace
{
public:
	VMFace() = default;
	virtual ~VMFace() = default;
	VMFace(VMFace const&) = delete;
	VMFace& operator=(VMFace const&) = delete;

	/// Execute EVM code by VM.
	///
	/// @param _out		Expected output
	void exec(u256& io_gas, ExtVMFace& _ext, bytesRef _out, OnOpFunc const& _onOp = {})
	{
		execImpl(io_gas, _ext, _onOp).copyTo(_out);
	}

	/// The same as above but returns a copy of full output.
	bytes exec(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp = {})
	{
		return execImpl(io_gas, _ext, _onOp).toVector();
	}

	/// VM implementation
	virtual bytesConstRef execImpl(u256& io_gas, ExtVMFace& _ext, OnOpFunc const& _onOp) = 0;
};

}
}
