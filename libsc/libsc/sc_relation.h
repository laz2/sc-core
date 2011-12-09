/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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
 * @file sc_relation.h
 * @brief Functions and classes for working with relations in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_RELATION_INCLUDED__
#define __LIBSC_SC_RELATION_INCLUDED__

#include "libsc.h"

#include "pm_keynodes.h"

#include <ostream>

/// @addtogroup libsc
/// @{

/**
 * @brief Class with static members for working with relations in sc-memory.
 */
class LIBSC_API sc_rel
{
public:
	static sc_addr bin_ord_at(sc_session *s, sc_addr rel, sc_addr val, sc_addr val_attr,
		sc_addr res_attr, sc_addr *tuple=0);

	/**
	 * @brief ��� ��������� ���������������� ��������� @p rel ������� ������ @p tuple, 
	 * � ������� ����������� � ��������� 1_ �������� @p val1.
	 * @return ��������� @p tuple c ��������� 2_.
	 */
	static inline sc_addr bin_ord_at_2(sc_session *s, sc_addr rel, sc_addr val1, sc_addr *tuple=0)
	{
		return bin_ord_at(s, rel, val1, N1_, N2_, tuple);
	}

	/**
	 * @brief ��� ��������� ���������������� ��������� @p rel ������� ������ @p tuple, 
	 * � ������� ����������� � ��������� 2_ �������� @p val2.
	 * @return ��������� @p tuple c ��������� 1_.
	 */
	static sc_addr bin_ord_at_1(sc_session *s, sc_addr rel, sc_addr val2, sc_addr *tuple=0)
	{
		return bin_ord_at(s, rel, val2, N2_, N1_, tuple);
	}

	static void unpack(sc_session *s, sc_addr rel, addr2addr_map &out_map);

	static void pack(sc_session *s, sc_addr rel, const addr2addr_map &a2a_map);

	/// Erase all relations tuples and relation sign.
	static void erase(sc_session *s, sc_addr rel);

	static sc_addr add_ord_tuple(sc_session *s, sc_addr rel, sc_addr v1, sc_addr v2, sc_addr *av1=0, sc_addr *av2=0);

};

/// @}

#endif // __LIBSC_SC_RELATION_INCLUDED__
