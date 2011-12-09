
/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

/**
 * @file smart_addr.h
 * @brief Smart #sc_addr.
 * @ingroup libsc
 */

#ifndef __SMART_ADDR_H_INCLUDED__
#define __SMART_ADDR_H_INCLUDED__

/// @addtogroup libsc
/// @{

#include "sc_addr.h"

/// Smart #sc_addr.
/// Ref addr in constructor.
/// Unref addr in destructor.
class sc_smart_addr
{
	sc_addr addr;
	void ref_addr()
	{
		if (addr) {
			addr->ref();
		}
	}

	void clear_addr()
	{
		if (addr) {
			addr->unref();
			addr = 0;
		}
	}

public:
	sc_smart_addr() : addr(0) {}
	sc_smart_addr(const sc_smart_addr &a): addr(a.addr)
	{
		ref_addr();
	}

	sc_smart_addr(sc_addr _addr) : addr(_addr)
	{
		ref_addr();
	}

	~sc_smart_addr()
	{
		clear_addr();
	}

	operator sc_addr()const
	{
		return addr;
	}

	sc_smart_addr &operator = (sc_addr a)
	{
		clear_addr();
		addr = a;
		ref_addr();
		return  *this;
	}

	sc_smart_addr &operator = (const sc_smart_addr &a)
	{
		return  *this = (sc_addr)a;
	}

	sc_addr operator *()
	{
		return addr;
	}

	sc_addr get() { return addr; }

	sc_addr operator->()
	{
		return addr;
	}

	bool operator == (const sc_smart_addr &a)const
	{
		return sc_addr(*this) == sc_addr(a);
	}

	bool operator == (const sc_addr a)const
	{
		return sc_addr(*this) == a;
	}

	bool operator < (const sc_smart_addr &a)const
	{
		return sc_addr(*this) < sc_addr(a);
	}

};

/// @}

#endif //__SMART_ADDR_H_INCLUDED__
