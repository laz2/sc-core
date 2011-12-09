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
 * @file sc_print_utils.h
 * @brief Utils for printing SC-elements.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_PRINT_UTILS_INCLUDED__
#define __LIBSC_SC_PRINT_UTILS_INCLUDED__

#include "libsc.h"
#include "sc_content.h"

#include <iostream>

/// @addtogroup libsc
/// @{

/// Return short element info as #sc_string (type and full uri).
LIBSC_API sc_string get_el_short_info(sc_session *s, sc_addr el);

LIBSC_API sc_string get_type_info(sc_type type);

LIBSC_API sc_retval print_content(const Content &cont);

extern LIBSC_API int print_el_variant;
extern LIBSC_API int print_el_max_power;

LIBSC_API sc_retval print_el(sc_session *session, sc_addr el);
LIBSC_API void print_el(sc_addr el);

LIBSC_API void print_element_info(sc_addr a, sc_session *s = 0);

LIBSC_API void print_struct(sc_session *session, sc_addr el);
LIBSC_API void print_struct(sc_addr el);

class LIBSC_API scs_printer
{
public:
	static
	void element(std::ostream &os, sc_session *s, sc_addr addr, addr2idtf_map &idtf_map, size_t depth=1, size_t indent=0);

	static
	inline void element(sc_addr addr, size_t depth=1, size_t indent=0)
	{
		addr2idtf_map addr_map;
		element(std::cout, system_session, addr, addr_map, depth, indent);
	}

	/// Print content in SCs format.
	static
	void content(std::ostream &os, const Content &cont);

	/// Print arc in SCs format.
	static
	void arc(std::ostream &os, sc_session *s, sc_addr addr, addr2idtf_map &idtf_map, size_t depth, size_t indent=0);

	/// Print set in SCs format.
	static
	void set_members(std::ostream &os, sc_session *s, sc_addr addr, addr2idtf_map &idtf_map, size_t depth, size_t indent);

	/// Return exist short SCs idtf from @p idtf_map or create new short idtf in that map and return it.
	static
	const sc_string& get_short_scs_idtf(sc_session *s, addr2idtf_map &idtf_map, sc_addr addr, bool &is_new);

	static
	void open_set(std::ostream &os, sc_type type, size_t indent);

	static
	void close_set(std::ostream &os, sc_type type, size_t indent);

	/// Print to @p os indentation.
	static inline void indentation(std::ostream &os, size_t indent)
	{ while (indent--) os << "  "; }

	/// Returns SCs representation of arc with type @p t.
	static sc_string arc_visual(sc_type t);

	/// Returns SCs representation of attribute with type @p t.
	static const char* attr_visual(sc_type t);
};

/// Print adapter for add functionality of printf-style format to C++ ostream.
class LIBSC_API print_f
{
public:
	~print_f() { delete[] buf; }

	explicit print_f(const char *format, ...);

	friend std::ostream& operator <<(std::ostream &os, const print_f &pf);
private:
	char *buf;
};

inline std::ostream& operator <<(std::ostream &os, const print_f &pf)
{
	os << pf.buf;
	return os;
}

/// @}

#endif // __LIBSC_SC_PRINT_UTILS_INCLUDED__
