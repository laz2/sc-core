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
 * @file sc_tuple.h
 * @brief Functions and classes for working with tuplies in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_TUPLE_INCLUDED__
#define __LIBSC_SC_TUPLE_INCLUDED__

#include "libsc.h"

/// @addtogroup libsc
/// @{

/// Class for working with tuples in sc-memory.
class LIBSC_API sc_tup
{
public:
	sc_tup(sc_session *s, sc_segment *seg, sc_type t=SC_N_CONST) : s_(s), seg_(seg), tuple_(0)
	{
		tuple_ = s->create_el(seg, t);
	}

	sc_tup(sc_session *s, sc_segment *seg, sc_addr tuple) : s_(s), seg_(seg), tuple_(tuple)
	{
	}

	sc_tup(sc_session *s, sc_addr tuple) : s_(s), seg_(tuple->seg), tuple_(tuple)
	{
	}

	sc_addr sign() const { return tuple_; }

	static sc_addr at(sc_session *s, sc_addr tuple, sc_addr attr, sc_addr *_arc=0);

	static sc_addr create_ord_m(sc_session *s, sc_segment *seg, ...);

	static inline sc_addr create_ord(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr e2)
	{
		return create_ord_m(s, seg, e1, e2, 0);
	}

	static inline sc_addr create_ord(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr e2, sc_addr e3)
	{
		return create_ord_m(s, seg, e1, e2, e3, 0);
	}

	static inline sc_addr create_ord(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr e2, sc_addr e3, sc_addr e4)
	{
		return create_ord_m(s, seg, e1, e2, e3, e4, 0);
	}

	static inline sc_addr create_ord(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr e2, sc_addr e3, sc_addr e4, sc_addr e5)
	{
		return create_ord_m(s, seg, e1, e2, e3, e4, e5, 0);
	}

	static inline sc_addr create_ord(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr e2, sc_addr e3, sc_addr e4, sc_addr e5, sc_addr e6)
	{
		return create_ord_m(s, seg, e1, e2, e3, e4, e5, e6, 0);
	}

	static inline sc_retval add(sc_session *s, sc_segment *seg, sc_addr tuple, sc_addr attr, sc_addr el)
	{
		sc_type arc_type = SC_ARC_ACCESSORY|(s->get_type(tuple) & SC_CONSTANCY_MASK)|SC_POS;
		return s->gen5_f_a_f_a_f(tuple, 0, seg, arc_type, el, 0, seg, arc_type, attr);
	}

	static inline sc_retval add(sc_session *s, sc_addr tuple, sc_addr attr, sc_addr el)
	{
		return add(s, tuple->seg, tuple, attr, el);
	}

	static inline sc_addr add_c(sc_session *s, sc_addr tuple, sc_addr attr, sc_addr el)
	{
		sc_addr arc = SCADDR_NIL;
		s->gen5_f_a_f_a_f(tuple, &arc, tuple->seg, SC_A_CONST|SC_POS, el, 0, tuple->seg, SC_A_CONST|SC_POS, attr);
		return arc;
	}

	sc_retval add(sc_segment *seg, sc_addr attr, sc_addr el)
	{
		return add(s_, seg, tuple_, attr, el);
	}

	sc_retval add(sc_addr attr, sc_addr el)
	{
		return add(s_, seg_, tuple_, attr, el);
	}

	/// Search first and second elements of ordered binary tuple.
	static sc_retval unpack_ord(sc_session *s, sc_addr tuple, sc_addr *pval1, sc_addr *pval2);

	/// Unpack ordered tuples arcs in @p arcs.
	/// @note Each number attribute should be unique else throw @c std::logic_error.
	static void unpack_ord(sc_session *s, sc_addr tuple, addr_vector &arcs);

	/// Optimized way to find several attribute values in one pass.
	static void unpack(sc_session *s, sc_addr tuple, sc_addr attrs[], sc_addr *rv[], int count);

private:
	sc_session *s_;
	sc_segment *seg_;
	sc_addr     tuple_;
};

/// @}

#endif // __LIBSC_SC_TUPLE_INCLUDED__
