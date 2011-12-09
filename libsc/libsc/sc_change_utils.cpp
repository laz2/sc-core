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

#include "precompiled_p.h"

void gen5_f_cpa_f_cpa_f(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr *e2, sc_addr e3, sc_addr *e4, sc_addr e5) 
{
	if (s->gen5_f_a_f_a_f(e1, e2, seg, SC_A_CONST|SC_POS, e3, e4, seg, SC_A_CONST|SC_POS, e5)) {
		throw gen_error();
	}
}

void gen5_f_cpa_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3, sc_addr *e4, sc_addr e5) 
{
	sc_segment *seg_e1 = e1->seg;
	if (s->gen5_f_a_f_a_f(e1, e2, seg_e1, SC_A_CONST|SC_POS, e3, e4, seg_e1, SC_A_CONST|SC_POS, e5)) {
		throw gen_error();
	}
}

void gen_tuple(sc_session *s, sc_segment *seg, sc_addr tuple, sc_addr el, sc_addr attr)
{
	gen5_f_cpa_f_cpa_f(s, seg, tuple, 0, el, 0, attr);
}

void gen_tuple(sc_session *s, sc_addr tuple, sc_addr el, sc_addr attr)
{
	gen5_f_cpa_f_cpa_f(s, tuple, 0, el, 0, attr);
}

void gen3_f_cpa_f(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr *e2, sc_addr e3) 
{
	if (s->gen3_f_a_f(e1, e2, seg, SC_A_CONST|SC_POS, e3) != RV_OK) {
		throw gen_error();
	}
}

void gen3_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3) 
{
	if (s->gen3_f_a_f(e1, e2, e1->seg, SC_A_CONST|SC_POS, e3) != RV_OK) {
		throw gen_error();
	}
}

void sc_include_in_set(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el)
{
	gen3_f_cpa_f(system_session, seg, set, 0, el);
}

sc_addr create_el(sc_session *s, sc_segment *seg, sc_type type)
{
	sc_addr elt = s->create_el(seg, type);
	
	if (!elt) {
		throw gen_error();
	}

	return elt;
}

inline sc_addr create_const_node(sc_session *s, sc_segment *seg)
{
	return create_el(s, seg, SC_N_CONST);
}

void reimplement(sc_session *s, sc_addr addr, sc_activity *new_activity)
{
	if (s->reimplement(addr, new_activity) != RV_OK) {
		throw reimplement_error();
	}
}
