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
 * @file sc_set.h
 * @brief Functions and classes for working with sets in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_SET_INCLUDED__
#define __LIBSC_SC_SET_INCLUDED__

#include "libsc.h"

/// @addtogroup libsc
/// @{

/**
 * @brief Class with static members for working with sets in sc-memory.
 * @note Working with #SC_CONST, #SC_VAR, #SC_METAVAR sets.
 * @note All methods with various arguments count uses #SCADDR_NIL or 0 as end of arguments list.
 */
class LIBSC_API sc_set
{
public:
	/// Return true if @p set is empty.
	static bool is_empty(sc_session *s, sc_addr set);

	/// Return membership arc of set if it has @p el.
	static sc_addr is_in(sc_session *s, sc_addr el, sc_addr set);


///
/// @name is_in_any
/// @brief Return membership arc of first set which has @p el. Pass sets after @p el.
/// @{
///

	static sc_addr is_in_any_m(sc_session *s, sc_addr el, ...);

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2)
	{
		return is_in_any_m(s, el, set1, set2, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3)
	{
		return is_in_any_m(s, el, set1, set2, set3, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4)
	{
		return is_in_any_m(s, el, set1, set2, set3, set4, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5)
	{
		return is_in_any_m(s, el, set1, set2, set3, set4, set5, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6)
	{
		return is_in_any_m(s, el, set1, set2, set3, set4, set5, set6, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7)
	{
		return is_in_any_m(s, el, set1, set2, set3, set4, set5, set6, set7, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8)
	{
		return is_in_any_m(s, el, set1, set2, set3, set4, set5, set6, set7, set8, 0);
	}

	static inline sc_addr is_in_any(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8,
		sc_addr set9)
	{
		return is_in_any_m(s, el, set1, set2, set3, set4, set5, set6, set7, set8, set9, 0);
	}

///
/// @}
///


///
/// @name is_in_all
/// @brief Return true if @p el is membership in all sets, passed after @p el.
/// @{
///

	static bool is_in_all_m(sc_session *s, sc_addr el, ...);

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2)
	{
		return is_in_all_m(s, el, set1, set2, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3)
	{
		return is_in_all_m(s, el, set1, set2, set3, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4)
	{
		return is_in_all_m(s, el, set1, set2, set3, set4, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5)
	{
		return is_in_all_m(s, el, set1, set2, set3, set4, set5, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6)
	{
		return is_in_all_m(s, el, set1, set2, set3, set4, set5, set6, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7)
	{
		return is_in_all_m(s, el, set1, set2, set3, set4, set5, set6, set7, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8)
	{
		return is_in_all_m(s, el, set1, set2, set3, set4, set5, set6, set7, set8, 0);
	}

	static inline bool is_in_all(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8,
		sc_addr set9)
	{
		return is_in_all_m(s, el, set1, set2, set3, set4, set5, set6, set7, set8, set9, 0);
	}

///
/// @}
///


///
/// @name include_in which generates arcs in specified segment
/// @brief Include @p el in all sets. Return generated arc for last set.
/// @{
///

	static sc_addr include_in_v(sc_session *s, sc_segment *seg, sc_addr el, va_list sets);

	static inline sc_addr include_in_m(sc_session *s, sc_segment *seg, sc_addr el, ...)
	{
		va_list list;
		va_start(list, el);
		sc_addr arc = include_in_v(s, seg, el, list);
		va_end(list);
		return arc;
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1)
	{
		return include_in_m(s, seg, el, set1, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2)
	{
		return include_in_m(s, seg, el, set1, set2, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3)
	{
		return include_in_m(s, seg, el, set1, set2, set3, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4)
	{
		return include_in_m(s, seg, el, set1, set2, set3, set4, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5)
	{
		return include_in_m(s, seg, el, set1, set2, set3, set4, set5, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5, sc_addr set6)
	{
		return include_in_m(s, seg, el, set1, set2, set3, set4, set5, set6, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5, sc_addr set6, sc_addr set7)
	{
		return include_in_m(s, seg, el, set1, set2, set3, set4, set5, set6, set7, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8)
	{
		return include_in_m(s, seg, el, set1, set2, set3, set4, set5, set6, set7, set8, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_segment *seg, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8,
		sc_addr set9)
	{
		return include_in_m(s, seg, el, set1, set2, set3, set4, set5, set6, set7, set8, set9, 0);
	}

///
/// @}
///


///
/// @name include_in  which generates arcs in segment of element
/// @brief Include @p el in all sets. Return generated arc for last set.
/// @{
///

	static inline sc_addr include_in_v(sc_session *s, sc_addr el, va_list sets)
	{
		return include_in_v(s, el->seg, el, sets);
	}

	static inline sc_addr include_in_m(sc_session *s, sc_addr el, ...)
	{
		va_list list;
		va_start(list, el);
		sc_addr arc = include_in_v(s, el, list);
		va_end(list);
		return arc;
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1)
	{
		return include_in_m(s, el, set1, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2)
	{
		return include_in_m(s, el, set1, set2, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3)
	{
		return include_in_m(s, el, set1, set2, set3, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4)
	{
		return include_in_m(s, el, set1, set2, set3, set4, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5)
	{
		return include_in_m(s, el, set1, set2, set3, set4, set5, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5, sc_addr set6)
	{
		return include_in_m(s, el, set1, set2, set3, set4, set5, set6, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5, sc_addr set6, sc_addr set7)
	{
		return include_in_m(s, el, set1, set2, set3, set4, set5, set6, set7, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2, sc_addr set3, sc_addr set4,
		sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8)
	{
		return include_in_m(s, el, set1, set2, set3, set4, set5, set6, set7, set8, 0);
	}

	static inline sc_addr include_in(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8,
		sc_addr set9)
	{
		return include_in_m(s, el, set1, set2, set3, set4, set5, set6, set7, set8, set9, 0);
	}

///
/// @}
///


///
/// @name include which generates arcs in specified segment
/// @brief Include in @p set all passed elements. Generate arcs in @p seg.
/// @{
///

	static void include_v(sc_session *s, sc_segment *seg, sc_addr set, va_list els);

	static inline void include_m(sc_session *s, sc_segment *seg, sc_addr set, ...)
	{
		va_list list;
		va_start(list, set);
		include_v(s, seg, set, list);
		va_end(list);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1)
	{
		include_m(s, seg, set, el1, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2)
	{
		include_m(s, seg, set, el1, el2, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3)
	{
		include_m(s, seg, set, el1, el2, el3, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4)
	{
		include_m(s, seg, set, el1, el2, el3, el4, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5)
	{
		include_m(s, seg, set, el1, el2, el3, el4, el5, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6)
	{
		include_m(s, seg, set, el1, el2, el3, el4, el5, el6, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6, sc_addr el7)
	{
		include_m(s, seg, set, el1, el2, el3, el4, el5, el6, el7, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6, sc_addr el7, sc_addr el8)
	{
		include_m(s, seg, set, el1, el2, el3, el4, el5, el6, el7, el8, 0);
	}

	static inline void include(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6, sc_addr el7, sc_addr el8, sc_addr el9)
	{
		include_m(s, seg, set, el1, el2, el3, el4, el5, el6, el7, el8, el9, 0);
	}

///
/// @}
///


///
/// @name include which generates arcs in segment of set
/// @brief Include in @p set all passed elements. Generate arcs in segment of @p set.
/// @{
///

	static inline void include_v(sc_session *s, sc_addr set, va_list els)
	{
		include_in_v(s, set->seg, set, els);
	}

	static inline void include_m(sc_session *s, sc_addr set, ...)
	{
		va_list list;
		va_start(list, set);
		include_in_v(s, set, list);
		va_end(list);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1)
	{
		include_in_m(s, set, el1, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2)
	{
		include_in_m(s, set, el1, el2, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3)
	{
		include_in_m(s, set, el1, el2, el3, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4)
	{
		include_in_m(s, set, el1, el2, el3, el4, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5)
	{
		include_in_m(s, set, el1, el2, el3, el4, el5, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6)
	{
		include_in_m(s, set, el1, el2, el3, el4, el6, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6, sc_addr el7)
	{
		include_in_m(s, set, el1, el2, el3, el4, el6, el7, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6, sc_addr el7, sc_addr el8)
	{
		include_in_m(s, set, el1, el2, el3, el4, el6, el7, el8, 0);
	}

	static inline void include(sc_session *s, sc_addr set, sc_addr el1, sc_addr el2, sc_addr el3,
		sc_addr el4, sc_addr el5, sc_addr el6, sc_addr el7, sc_addr el8, sc_addr el9)
	{
		include_in_m(s, set, el1, el2, el3, el4, el6, el7, el8, el9, 0);
	}

///
/// @}
///


///
/// @name exclude_from
/// @brief Exclude @p el from all sets. Return true if at least one arc was erased.
/// @note Flag @p is_cantor_sets indicates that excludes only one occurrence.
/// @note @c exclude_from without @p is_cantor_sets excludes only one occurrence.
/// @note @c exclude_multi_from excludes all occurrences of element.
/// @{
///

	static bool exclude_from_v(sc_session *s, sc_addr el, bool is_cantor_sets, va_list sets);

	static inline bool exclude_from_m(sc_session *s, sc_addr el, bool is_cantor_sets, ...)
	{
		va_list list;
		va_start(list, is_cantor_sets);
		bool rv = exclude_from_v(s, el, is_cantor_sets, list);
		va_end(list);
		return rv;
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1)
	{
		return exclude_from_m(s, el, true, set1, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2)
	{
		return exclude_from_m(s, el, true, set1, set2, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, set4, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, set4, set5, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, set4, set5, set6, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, set4, set5, set6, set7, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, set4, set5, set6, set7, set8, 0);
	}

	static inline bool exclude_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8, sc_addr set9)
	{
		return exclude_from_m(s, el, true, set1, set2, set3, set4, set5, set6, set7, set8, set9, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1)
	{
		return exclude_from_m(s, el, false, set1, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2)
	{
		return exclude_from_m(s, el, false, set1, set2, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, set4, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, set4, set5, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, set4, set5, set6, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, set4, set5, set6, set7, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, set4, set5, set6, set7, set8, 0);
	}

	static inline bool exclude_multi_from(sc_session *s, sc_addr el, sc_addr set1, sc_addr set2,
		sc_addr set3, sc_addr set4, sc_addr set5, sc_addr set6, sc_addr set7, sc_addr set8, sc_addr set9)
	{
		return exclude_from_m(s, el, false, set1, set2, set3, set4, set5, set6, set7, set8, set9, 0);
	}

///
/// @}
///

	/// Erase all elements from @p set.
	static void erase_from(sc_session *s, sc_addr set);

	/// Fast unpack sets which include @p el.
	/// Expect size of arrays @p sets and @p rv is @p count.
	/// If set from @p sets includes @p el then method assigns to element of @p rv with same index true (otherwise false).
	static void unpack(sc_session *s, sc_addr el, sc_addr sets[], bool rv[], int count);

	/// Callback for #sc_set.copy.
	/// @see #sc_set.copy
	typedef void (*copy_callback)(sc_addr original,sc_addr copy,void *cb_data);

	/**
	 * @brief Copy all elements from a_set to segment 'to'.
	 * 
	 * Tries to set correct idtf's for copies
	 * in case where it's impossible, just leaves copy unnamed
	 *
	 * copy of watch_copy element (if it's not 0) is placed to *copy
	 *
	 * time complexity is O(n*log_2(n)) (may be O(n) with hashes))
	 *
	 * @see #sc_set.copy_callback
	 */
	static void copy(sc_session *s, sc_segment *to, const addr_set &a_set, copy_callback cb=0, void *cb_data=0);
};

/// @}

#endif // __LIBSC_SC_SET_INCLUDED__
