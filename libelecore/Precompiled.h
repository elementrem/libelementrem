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
/** @file Precompiled.h
 * 
 * 
 */

#pragma once

#include <unordered_map>
#include <functional>
#include <libdevcore/CommonData.h>
#include <libdevcore/Exceptions.h>

namespace dev
{
namespace ele
{

using PrecompiledExecutor = std::function<void(bytesConstRef _in, bytesRef _out)>;

DEV_SIMPLE_EXCEPTION(ExecutorNotFound);

class PrecompiledRegistrar
{
public:
	/// Get the executor object for @a _name function or @throw ExecutorNotFound if not found.
	static PrecompiledExecutor const& executor(std::string const& _name);

	/// Register an executor. In general just use ELE_REGISTER_PRECOMPILED.
	static PrecompiledExecutor registerPrecompiled(std::string const& _name, PrecompiledExecutor const& _exec) { return (get()->m_execs[_name] = _exec); }
	/// Unregister an executor. Shouldn't generally be necessary.
	static void unregisterPrecompiled(std::string const& _name) { get()->m_execs.erase(_name); }

private:
	static PrecompiledRegistrar* get() { if (!s_this) s_this = new PrecompiledRegistrar; return s_this; }

	std::unordered_map<std::string, PrecompiledExecutor> m_execs;
	static PrecompiledRegistrar* s_this;
};

// TODO: unregister on unload with a static object.
#define ELE_REGISTER_PRECOMPILED(Name) static void __ele_registerPrecompiledFunction ## Name(bytesConstRef _in, bytesRef _out); static PrecompiledExecutor __ele_registerPrecompiledFactory ## Name = ::dev::ele::PrecompiledRegistrar::registerPrecompiled(#Name, &__ele_registerPrecompiledFunction ## Name); static void __ele_registerPrecompiledFunction ## Name

}
}
