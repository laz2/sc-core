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
 * @file sc_set.cpp
 * @brief Functions and classes for working with sets in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

/// @addtogroup libsc
/// @{

#include "precompiled_p.h"

bool sc_set::is_empty(sc_session *s, sc_addr set)
{
	SC_ASSERT(s);
	SC_ASSERT(set);

	meta_output_iterator oiter(set, s);
	for (; !oiter.is_over(); oiter.next()) {
		sc_addr arc = oiter.value(1);
		if (sc_isa(s->get_type(arc), SC_POS))
			return false;
	}

	return true;
}

sc_addr sc_set::is_in(sc_session *s, sc_addr el, sc_addr set)
{
	SC_ASSERT(s);
	SC_ASSERT(el);
	SC_ASSERT(set);

	//
	// Optimization:
	// Is count of input arcs for @c el greater then count of output arcs of @c set?
	// If yes then use output arcs of @c set, else input of @c el.
	//
	if (el->get_in_qnt() > set->get_out_qnt()) {
		meta_output_iterator oiter(set, s);
		for (; !oiter.is_over(); oiter.next()) {
			sc_addr arc = oiter.value(1);
			sc_addr end = s->get_end(arc);
			if (end == el)
				return arc;
		}
	} else {
		meta_input_iterator iiter(el, s);
		for (; !iiter.is_over(); iiter.next()) {
			sc_addr arc = iiter.value(1);
			sc_addr beg = s->get_beg(arc);
			if (beg == set)
				return arc;
		}
	}

	return 0;
}

sc_addr sc_set::is_in_any_m(sc_session *s, sc_addr el, ...)
{
	SC_ASSERT(s);
	SC_ASSERT(el);

	va_list list;
	va_start(list, el);

	sc_addr set = 0;
	sc_addr arc = 0;
	while ((set = va_arg(list, sc_addr))) {
		sc_addr arc = sc_set::is_in(s, el, set);
		if (arc)
			return arc;
	}

	va_end(list);

	return arc;
}

bool sc_set::is_in_all_m(sc_session *s, sc_addr el, ...)
{
	SC_ASSERT(s);
	SC_ASSERT(el);

	va_list list;
	va_start(list, el);

	sc_addr set = 0;
	while ((set = va_arg(list, sc_addr))) {
		if (sc_set::is_in(s, el, set) == 0)
			return false;
	}

	va_end(list);

	return true;
}

sc_addr sc_set::include_in_v(sc_session *s, sc_segment *seg, sc_addr el, va_list sets)
{
	SC_ASSERT(s);
	SC_ASSERT(seg);
	SC_ASSERT(el);

	sc_addr arc = 0;
	sc_addr set = 0;
	while ((set = va_arg(sets, sc_addr))) {
		sc_type const_type = s->get_type(set) & SC_CONSTANCY_MASK;
		s->gen3_f_a_f(set, &arc, seg, SC_ARC_ACCESSORY|const_type|SC_POS, el);
	}

	return arc;
}

void sc_set::include_v(sc_session *s, sc_segment *seg, sc_addr set, va_list els)
{
	SC_ASSERT(s);
	SC_ASSERT(seg);
	SC_ASSERT(set);

	sc_type const_type = s->get_type(set) & SC_CONSTANCY_MASK;

	sc_addr el = 0;
	while (el = va_arg(els, sc_addr))
		s->gen3_f_a_f(set, 0, seg, SC_ARC_ACCESSORY|const_type|SC_POS, el);
}

bool sc_set::exclude_from_v(sc_session *s, sc_addr el, bool is_cantor_sets, va_list sets)
{
	SC_ASSERT(s);
	SC_ASSERT(el);

	bool rv = false;

	sc_addr set = 0;
	while ((set = va_arg(sets, sc_addr))) {
		//
		// Optimization:
		// Is count of input arcs for @c el greater then count of output arcs of @c set?
		// If yes then use output arcs of @c set, else input of @c el.
		//
		if (el->get_in_qnt() > set->get_out_qnt()) {
			meta_output_iterator oiter(set, s);
			for (; !oiter.is_over(); oiter.next()) {
				sc_addr arc = oiter.value(1);
				if (sc_isa(s->get_type(arc), SC_POS)
						&& el == s->get_end(arc)) {
					rv = true;
					s->erase_el(arc);

					//
					// If set is cantor then erase only one occurrence.
					//
					if (is_cantor_sets)
						break;
				}
			}
		} else {
			meta_input_iterator iiter(el, s);
			for (; !iiter.is_over(); iiter.next()) {
				sc_addr arc = iiter.value(1);
				if (sc_isa(s->get_type(arc), SC_POS)
						&& set == s->get_beg(arc)) {
					rv = true;
					s->erase_el(arc);

					//
					// If set is cantor then erase only one occurrence.
					//
					if (is_cantor_sets)
						break;
				}
			}
		}
	}

	return rv;
}

void sc_set::erase_from(sc_session *s, sc_addr set)
{
	SC_ASSERT(s);
	SC_ASSERT(set);

	meta_output_iterator oiter(set, s);
	for (; !oiter.is_over(); oiter.next()) {
		sc_addr arc = oiter.value(1);
		if (sc_isa(s->get_type(arc), SC_POS))
			s->erase_el(s->get_end(arc));
	}
}

void sc_set::unpack(sc_session *s, sc_addr element, sc_addr sets[], bool rv[], int count)
{
	meta_input_iterator iiter(element,s);
	memset(rv, 0, count);
	for (; !iiter.is_over(); iiter.next()) {
		sc_addr arc = iiter.value(1);
		sc_addr set = s->get_beg(arc);
		for (int i = count - 1; i >= 0; i--) {
			if (sets[i] == set) {
				rv[i] = true;
				break;
			}
		}
	}
}

struct addr_pair_comparator : public std::binary_function<addr2addr_pair, addr2addr_pair, bool>
{
	bool operator()(const addr2addr_pair &a, const addr2addr_pair &b) const
	{
		return a.first < b.first;
	}
};

static
sc_addr my_find(addr2addr_vector &vec,const sc_addr key)
{
	addr2addr_vector::iterator it = std::lower_bound(vec.begin(),vec.end(),
		addr2addr_pair(key, SCADDR_NIL), addr_pair_comparator());
	if (it == vec.end() || key != (*it).first)
		return 0;
	return (*it).second;
}

// relies on ordering of iterator
void sc_set::copy(sc_session *s, sc_segment *to, const addr_set &a_set, copy_callback cb, void *cb_data)
{
	/* we may use simple vector instead of map because keys are added
	 * in ascending order
	 * NOTE: keys are distinct
	 */
	addr2addr_vector copies;
	addr_set::const_iterator it;
	sc_iterator *iter;
	for (it=a_set.begin();it != a_set.end();it++) {
		sc_addr element = *it;
		sc_addr c = s->create_el(to,s->get_type(element));
		// move all copied arc's ends
		// input
		iter = s->create_iterator(sc_constraint_new(CONSTR_ALL_INPUT, element), true);
		for (;!iter->is_over();iter->next()) {
			sc_addr arc = iter->value(1);
			sc_addr arc_copy;
			if (arc >= element)
				continue;
			arc_copy = my_find(copies,arc);
			if (!arc_copy)
				continue;
			s->set_end(arc_copy,c);
		}
		delete iter;
		// output
		iter = s->create_iterator(sc_constraint_new(CONSTR_ALL_OUTPUT,
								element
						  ),true);
		for (;!iter->is_over();iter->next()) {
			sc_addr arc = iter->value(1);
			sc_addr arc_copy;
			if (arc >= element)
				continue;
			arc_copy = my_find(copies,arc);
			if (!arc_copy)
				continue;
			s->set_beg(arc_copy,c);
		}
		delete iter;
		copies.push_back(std::make_pair(element,c));
		if (sc_isa(s->get_type(element), SC_CONNECTOR_MASK)) {
			sc_addr t;
			sc_addr from = s->get_beg(element);
			sc_addr to = s->get_end(element);
			if (from && from <= element && (t = my_find(copies,from)))
				from = t;
			if (to && to <= element && (t = my_find(copies,to)))
				to = t;
			s->set_end(c,to);
			s->set_beg(c,from);
		}
		s->set_idtf(c,s->get_idtf(element));
		s->set_content(c,s->get_content(element));
		if (cb)
			cb(element,c,cb_data);
	}
}

/// @}
