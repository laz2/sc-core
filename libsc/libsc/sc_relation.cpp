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
 * @file sc_relation.cpp
 * @brief Functions and classes for working with relations in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

/// @addtogroup libsc
/// @{

#include "precompiled_p.h"

sc_addr sc_rel::bin_ord_at(sc_session *s, sc_addr rel, sc_addr val, sc_addr val_attr,
						   sc_addr res_attr, sc_addr *tuple)
{
	sc_addr rtuple, rv=0;
	search_oneshot(s,sc_constraint_new(CONSTR_ORD_BIN_CONN1,
					   rel,
					   0,
					   0, // tuple
					   0,
					   val,
					   0,
					   0, // rv
					   0,
					   val_attr,
					   0,
					   res_attr
				   ),true,2,2,&rtuple,6,&rv);
	if (rv && tuple)
		*tuple = rtuple;
	return rv;
}

void sc_rel::pack(sc_session *s, sc_addr rel, const addr2addr_map &a2a_map)
{
	SC_ASSERT(s);
	SC_ASSERT(rel);

	addr2addr_map::const_iterator it = a2a_map.begin();
	for(; it != a2a_map.end(); ++it)
		sc_rel::add_ord_tuple(s, rel, it->first, it->second);
}

void sc_rel::erase(sc_session *s, sc_addr rel)
{
	SC_ASSERT(s);
	SC_ASSERT(rel);

	sc_type const_type = s->get_type(rel) & SC_CONSTANCY_MASK;

	sc_iterator *tuple_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, rel, SC_ARC_ACCESSORY|const_type|SC_POS, SC_N_CONST), true);
	sc_for_each(tuple_it) s->erase_el(tuple_it->value(2));

	s->erase_el(rel);
}

void sc_rel::unpack(sc_session *s, sc_addr rel, addr2addr_map &out_map)
{
	SC_ASSERT(s);
	SC_ASSERT(rel);

	sc_type const_type = s->get_type(rel) & SC_CONSTANCY_MASK;
	sc_iterator *tup_it = s->create_iterator(
		sc_constraint_new(CONSTR_3_f_a_a, rel, SC_ARC_ACCESSORY|const_type|SC_POS, SC_N_CONST), true);
	sc_for_each (tup_it) {
		sc_addr tuple = tup_it->value(2);
		sc_addr e1 = sc_tup::at(s, tuple, N1_);
		sc_addr e2 = sc_tup::at(s, tuple, N2_);
		out_map.insert(addr2addr_map::value_type(e1, e2));
	}
}

sc_addr sc_rel::add_ord_tuple(sc_session *s, sc_addr rel, sc_addr v1, sc_addr v2, sc_addr *av1, sc_addr *av2)
{
	sc_addr tuple = s->create_el(rel->seg, SC_N_CONST);
	s->gen3_f_a_f(rel, 0, rel->seg, SC_A_CONST|SC_POS, tuple);
	s->gen5_f_a_f_a_f(tuple, av1, rel->seg, SC_A_CONST|SC_POS, v1, 0, rel->seg, SC_A_CONST|SC_POS, N1_);
	s->gen5_f_a_f_a_f(tuple, av2, rel->seg, SC_A_CONST|SC_POS, v2, 0, rel->seg, SC_A_CONST|SC_POS, N2_);
	return tuple;
}

/// @}
