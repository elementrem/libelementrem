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
/** @file Precompiled.cpp
 * 
 * 
 */

#include "Precompiled.h"
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcore/Hash.h>
#include <libdevcrypto/Common.h>
#include <libelecore/Common.h>
using namespace std;
using namespace dev;
using namespace dev::ele;

PrecompiledRegistrar* PrecompiledRegistrar::s_this = nullptr;

PrecompiledExecutor const& PrecompiledRegistrar::executor(std::string const& _name)
{
	if (!get()->m_execs.count(_name))
		BOOST_THROW_EXCEPTION(ExecutorNotFound());
	return get()->m_execs[_name];
}

namespace
{

ELE_REGISTER_PRECOMPILED(ecrecover)(bytesConstRef _in, bytesRef _out)
{
	struct inType
	{
		h256 hash;
		h256 v;
		h256 r;
		h256 s;
	} in;

	memcpy(&in, _in.data(), min(_in.size(), sizeof(in)));

	h256 ret;
	u256 v = (u256)in.v;
	if (v >= 27 && v <= 28)
	{
		SignatureStruct sig(in.r, in.s, (byte)((int)v - 27));
		if (sig.isValid())
		{
			try
			{
				if (Public rec = recover(sig, in.hash))
				{
					ret = dev::sha3(rec);
					memset(ret.data(), 0, 12);
					ret.ref().copyTo(_out);
				}
			}
			catch (...) {}
		}
	}
}

ELE_REGISTER_PRECOMPILED(sha256)(bytesConstRef _in, bytesRef _out)
{
	dev::sha256(_in).ref().copyTo(_out);
}

ELE_REGISTER_PRECOMPILED(ripemd160)(bytesConstRef _in, bytesRef _out)
{
	h256(dev::ripemd160(_in), h256::AlignRight).ref().copyTo(_out);
}

ELE_REGISTER_PRECOMPILED(identity)(bytesConstRef _in, bytesRef _out)
{
	_in.copyTo(_out);
}

}
