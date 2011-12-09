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

/*!
 * @addtogroup libsc
 * @{
 */

/*!
 * @brief Utils for working with SC-memory thats throws exception while error.
 * @file sc_change_utils.h
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_CHANGE_LIB_H__
#define __LIBSC_CHANGE_LIB_H__

class LIBSC_API memory_error {};

class LIBSC_API gen_error : public memory_error {};

class LIBSC_API reimplement_error : public memory_error {};

LIBSC_API void gen5_f_cpa_f_cpa_f(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr *e2, sc_addr e3, sc_addr *e4, sc_addr e5);
LIBSC_API void gen5_f_cpa_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3, sc_addr *e4, sc_addr e5);
LIBSC_API void gen5_f_cpa_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3, sc_addr *e4, sc_addr e5);
LIBSC_API void gen3_f_cpa_f(sc_session *s, sc_segment *seg, sc_addr e1, sc_addr *e2, sc_addr e3);
LIBSC_API void gen3_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3);
LIBSC_API void gen_tuple(sc_session *s, sc_segment *seg, sc_addr tuple, sc_addr el, sc_addr attr);
LIBSC_API void gen_tuple(sc_session *s, sc_addr tuple, sc_addr el, sc_addr attr);
LIBSC_API void sc_include_in_set(sc_session *s, sc_segment *seg, sc_addr set, sc_addr el);
LIBSC_API sc_addr create_el(sc_session *s, sc_segment *seg, sc_type type);
LIBSC_API sc_addr create_const_node(sc_session *s, sc_segment *seg);
LIBSC_API void reimplement(sc_session *s, sc_addr addr, sc_activity *newActivity);

#endif // __LIBSC_CHANGE_LIB_H__

/*!@}*/
