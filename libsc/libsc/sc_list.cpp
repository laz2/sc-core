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
 * @file sc_list.cpp
 * @brief Functions and classes for working with two-linked list in sc-memory.
 * @ingroup libsc
 * @author Alexey Kondratenko
 */

/// @addtogroup libsc
/// @{

#include "precompiled_p.h"

#include "sc_list.h"

sc_addr sc_list::create(sc_session *s, sc_segment *seg, sc_addr val, sc_addr next)
{
	sc_retval rv;
	sc_addr l = s->create_el(seg,SC_N_CONST);
	if (!l)
		SC_ABORT();
	if (val) {
		rv = s->gen5_f_a_f_a_f(l,
				  0,seg,SC_A_CONST|SC_POS,
				  val,
				  0,seg,SC_A_CONST|SC_POS,
				  LIST_VALUE_
			);
		if (rv)
			SC_ABORT();
	}
	if (next) {
		rv = s->gen5_f_a_f_a_f(l,
					   0,seg,SC_A_CONST|SC_POS,
					   val,
					   0,seg,SC_A_CONST|SC_POS,
					   LIST_NEXT_
			);
		if (rv)
			SC_ABORT();
	}
	return l;
}

sc_retval sc_list::set_next(sc_session *s, sc_segment *seg, sc_addr list1, sc_addr list2)
{
	sc_addr old_next_arc;
	if (!search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
						list1,
						0,
						0,
						0,
						LIST_NEXT_
					),true,1,1,&old_next_arc))
		s->erase_el(old_next_arc);
	return s->gen5_f_a_f_a_f(list1,
			  0,seg,SC_A_CONST|SC_POS,
			  list2,
			  0,seg,SC_A_CONST|SC_POS,
			  LIST_NEXT_
		);
}

sc_retval sc_list::set_value(sc_session *s, sc_segment *seg, sc_addr list, sc_addr val)
{
	sc_addr old_arc;
	if (!search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
						list,
						0,
						0,
						0,
						LIST_VALUE_
					),true,1,1,&old_arc))
		s->erase_el(old_arc);
	if (val) {
		return s->gen5_f_a_f_a_f(list,
				  0,seg,SC_A_CONST|SC_POS,
				  val,
				  0,seg,SC_A_CONST|SC_POS,
				  LIST_VALUE_
			);
	} else {
		return RV_OK;
	}
}

sc_addr sc_list::get_next(sc_session *s, sc_addr list)
{
	sc_addr rv=0;
	search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
					   list,
					   0,
					   0,
					   0,
					   LIST_NEXT_
				   ),true,1,2,&rv);
	return rv;
}

sc_addr sc_list::get_prev(sc_session *s, sc_addr list)
{
	sc_addr rv = 0;
	search_oneshot(s,sc_constraint_new(CONSTR_5_a_a_f_a_f,
		0,
		0,
		list,
		0,
		LIST_NEXT_
		), true, 1, 0, &rv);
	return rv;
}

sc_addr sc_list::get_value(sc_session *s, sc_addr list)
{
	sc_addr rv=0;
	search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
					   list,
					   0,
					   0,
					   0,
					   LIST_VALUE_
				   ),true,1,2,&rv);
	return rv;
}

void sc_list::erase(sc_session *s, sc_addr list)
{
	sc_addr curr = list;
	do {
		sc_addr prev = curr;
		curr = sc_list::get_next(s, prev);
		s->erase_el(prev);
	} while (curr);
}

/// @}
