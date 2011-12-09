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
 * @file sc_tuple.cpp
 * @brief Functions and classes for working with tuplies in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

/// @addtogroup libsc
/// @{

#include "precompiled_p.h"

sc_addr sc_tup::at(sc_session *s, sc_addr tuple, sc_addr attr, sc_addr *_arc)
{
	sc_addr rv = 0;
	sc_addr arc = 0;
	search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
					tuple,
					0,
					0,
					0,
					attr), true, 2, 2, &rv, 1, &arc);
	if (_arc)
		*_arc = arc;
	return rv;
}

sc_addr sc_tup::create_ord_m(sc_session *s, sc_segment *seg, ...)
{
	va_list list;
	va_start(list, seg);

	sc_addr tuple = s->create_el(seg, SC_N_CONST);

	sc_addr el = 0;
	size_t index = 0;
	while (el = va_arg(list, sc_addr))
		s->gen5_f_a_f_a_f(tuple, 0, seg, SC_A_CONST|SC_POS, el, 0, seg, SC_A_CONST|SC_POS, pm_numattr[++index]);

	va_end(list);

	return tuple;
}

sc_retval sc_tup::unpack_ord(sc_session *s, sc_addr tuple, sc_addr *pval1, sc_addr *pval2) 
{
	sc_addr val1, val2;
	if (search_5_f_cpa_a_cpa_f(s, tuple, 0, &val1, 0, N1_) ||
		search_5_f_cpa_a_cpa_f(s, tuple, 0, &val2, 0, N2_)) {
		return RV_ERR_GEN;
	}

	if (pval1)
		*pval1 = val1;

	if (pval2)
		*pval2 = val2;

	return RV_OK;
}

void sc_tup::unpack_ord(sc_session *s, sc_addr tuple, addr_vector &arcs)
{
	// Result arcs vector with maximum expected size
	addr_vector result(tuple->get_out_qnt(), SCADDR_NIL);

	sc_type tup_constancy = s->get_type(tuple) & SC_CONSTANCY_MASK;

	// Maximum index in @c result at end of unpacking
	size_t max_index = -1;

	meta_output_iterator oiter(tuple, s);
	for (; !oiter.is_over(); oiter.next()) {
		sc_addr e2 = oiter.value(1);

		sc_type e2_type = s->get_type(e2);
		if ((e2_type & SC_POS) && (tup_constancy & e2_type)) {
			meta_input_iterator iiter(e2, s);
			for (; !iiter.is_over(); iiter.next()) {
				sc_addr e4 = iiter.value(1);
				sc_addr e5 = s->get_beg(e4);

				if (e5) {
					const Content *c = s->get_content_const(e5);
					if (c->type() == TCINT) {
						size_t index = Cont(*c).i - 1;

						if (0 <= index && index < NUMATTR_CNT && pm_numattr[index + 1] == e5) {
							if (result[index])
								SC_THROW_EXCEPTION(std::logic_error,
									"Not unique attribute " << SC_ID(e5) << " in tuple " << SC_URI(tuple) << ".")

							result[index] = e2;

							if (max_index == static_cast<size_t>(-1) || index > max_index)
								max_index = index;
						}
					}
				}
			}
		}
	}

	if (max_index + 1 < result.size())
		result.resize(max_index + 1);

	arcs.assign(result.begin(), result.end());
}

void sc_tup::unpack(sc_session *s, sc_addr tuple, sc_addr attrs[], sc_addr *rv[], int count)
{
	for (int i = count - 1; i>=0; i--)
		*rv[i] = 0;
	meta_output_iterator oiter(tuple, s);
	for (; !oiter.is_over(); oiter.next()) {
		sc_addr e2 = oiter.value(1);
		sc_addr e3 = s->get_end(e2);
		meta_input_iterator iiter(e2, s);
		for (; !iiter.is_over(); iiter.next()) {
			sc_addr e4 = iiter.value(1);
			sc_addr e5 = s->get_beg(e4);
			for (int i = count - 1; i >= 0; i--)
				if (attrs[i] == e5) {
					*rv[i] = e3;
					break;
				}
		}
	}
}

/// @}
