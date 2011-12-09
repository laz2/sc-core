
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
 * @file sc_utils.h
 * @brief Helper functions for working with sc-memory.
 * @ingroup libsc
 */

#ifndef __LIBSC_SC_UTILS_H_INCLUDED__
#define __LIBSC_SC_UTILS_H_INCLUDED__

#include <algorithm>
#include <sstream>

#include "sc_types.h"

/// @addtogroup libsc
/// @{

class sc_session;
class sc_segment;
class sc_iterator;
class sc_dir_iterator;

LIBSC_API sc_string get_full_uri(sc_session *s, sc_addr el);

LIBSC_API sc_string get_full_uri(sc_session *s, sc_segment *seg);

LIBSC_API sc_string get_quoted_full_uri(sc_session *s, sc_addr el);

LIBSC_API sc_string get_quoted_full_uri(sc_session *s, sc_segment *seg);

LIBSC_API sc_string get_quoted_idtf(sc_session *s, sc_addr el);

LIBSC_API sc_addr get_el_by_full_uri(sc_session *s, const sc_string &uri);

//! Create element by full uri @p uri.
LIBSC_API sc_addr create_el_by_full_uri(sc_session *s, const sc_string &uri, sc_type type);

LIBSC_API bool is_system_id(const sc_string& id); 

LIBSC_API sc_retval search_3l2_f_cpa_cna_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr *e3, sc_addr *e4, sc_addr e5);
LIBSC_API sc_retval search_5_f_cpa_a_cpa_f(sc_session *s,sc_addr e1,sc_addr *e2,sc_addr *e3,sc_addr *e4,sc_addr e5);
LIBSC_API sc_retval search_5_f_cpa_cna_cpa_f(sc_session *s,sc_addr e1,sc_addr *e2,sc_addr *e3,sc_addr *e4,sc_addr e5);
LIBSC_API sc_retval search_3_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3);
LIBSC_API sc_retval search_3_f_cpa_cna(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr *e3);
LIBSC_API sc_retval search_3_f_cpa_a(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr *e3);
LIBSC_API sc_retval search_oneshot(sc_session *s,sc_constraint *c,bool sink,int len,...);

LIBSC_API sc_retval search_oneshot_a(sc_session *s,sc_constraint *c,bool sink,int len,sc_param *res);

LIBSC_API sc_retval delete_oneshot(sc_session *s,sc_constraint *,bool sink,int len,...);

LIBSC_API void dprintf(const char *format,...);

LIBSC_API extern int diag_output;
LIBSC_API extern int verb_output;

/*
 * All elements in 'from' segment identical elements in 'to',
 * will be glued to them.
 * Identical 'from' element which have incompatible sc_type
 * to element in 'to' is placed in err_el and RV_ERR_GEN is returned
 * from & to must be opened in 's'.
 */
LIBSC_API
sc_retval glue_identical(sc_session *s,sc_segment *from,sc_segment *to, sc_addr *err_el);


class LIBSC_API glue_identical_checker {
	sc_session *session;
	sc_segment *from;
	sc_segment *to;
	bool check_in_progress;
public:
	enum error_reason {
		DIFFERENT_CONTENT,
		INCOMPATIBLE_TYPE,
		DIFFERENT_ARC_FROM,
		DIFFERENT_ARC_TO
	};
	glue_identical_checker(sc_session *s, sc_segment *from, sc_segment *to);
	void reset(sc_session *s, sc_segment *_from, sc_segment *_to)
	{
		session = s;
		from = _from;
		to = _to;
	}
	sc_session *get_session() {return session;}
	sc_segment *get_from() {return from;}
	sc_segment *get_to() {return to;}
	virtual ~glue_identical_checker();
	virtual void error(/*error_reason*/int reason, sc_addr from_element, sc_addr to_element);
	virtual void to_merge(sc_addr from_element, sc_addr to_element);
	void stop_check()
	{
		check_in_progress = false;
	}
	bool check();
};

LIBSC_API sc_retval glue_segment(sc_session *s,sc_segment *from,sc_segment *to);

LIBSC_API sc_addr move_element_neighborhood(sc_session *s,sc_addr el,sc_segment *seg);

LIBSC_API sc_retval sc_is_mergeable(sc_session *s, sc_addr main, sc_addr other,
			  sc_type *new_type=0, Content *new_content=0);

LIBSC_API sc_retval sc_merge_elements(sc_session *s,sc_addr main, sc_addr other);

#define SC_THROW_EXCEPTION(ex, out) \
	{ \
		std::ostringstream __out_buf; \
		__out_buf << out; \
		throw ex(__out_buf.str()); \
	}

/// @}

#endif // __LIBSC_SC_UTILS_H_INCLUDED__
