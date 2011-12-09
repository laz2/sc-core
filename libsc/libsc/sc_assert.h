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
 * @file sc_assert.h
 * @brief Assert functional of libsc.
 * @author Dmitry Lazurkin
 * @ingroup libsc
 */

#ifndef __LIBSC_SC_ASSERT_INCLUDED__
#define __LIBSC_SC_ASSERT_INCLUDED__

/// @addtogroup libsc
/// @{

#include <iostream>

#include "sc_print_utils.h"
#include "sc_utils.h"

#define SC_ID(el) system_session->get_idtf(el)

#define SC_IDc(el) SC_ID(el).c_str()

#define SC_QID(el) get_quoted_idtf(system_session, el)

#define SC_QIDc(el) SC_QID(el).c_str()

#define SC_URI(foo) ::get_full_uri(system_session, foo)

#define SC_URIc(foo) SC_URI(foo).c_str()

#define SC_QURI(foo) ::get_quoted_full_uri(system_session, foo)

#define SC_QURIc(foo) SC_QURI(foo).c_str()

#define SC_TYPE(type) get_type_info(type)

#define SC_TYPEc(type) SC_TYPE(type).c_str()

#define SC_ELTYPE(el) SC_TYPE(system_session->get_type(el))

#define SC_ELTYPEc(el) SC_TYPEc(system_session->get_type(el))

#define SC_SHORTI(el) get_el_short_info(system_session, el)

#define SC_SHORTIc(el) SC_SHORTI(el).c_str()

#ifdef _DEBUG

#include <cassert>
#include <sstream>

#define SC_ASSERT(exp) assert(exp)

#define SC_ASSERTS(exp, msg) \
	if (!(exp)) { \
		std::cerr << "Assertion failed: " << msg << std::endl; \
		assert(exp); \
	}

#define SC_ASSERTF(exp, fmt, ...) \
	SC_ASSERTS(exp, print_f(fmt, __VA_ARGS__))

#else

#define SC_ASSERT(exp)

#define SC_ASSERTS(exp, msg)

#define SC_ASSERTF(exp, fmt, ...)

#endif // _DEBUG

/// @}

#endif // __LIBSC_SC_ASSERT_INCLUDED__
