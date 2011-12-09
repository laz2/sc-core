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
 * @file sc_segment_base.h
 * @brief Base implementation of #sc_segment methods.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_SEGMENT_BASE_INCLUDED__
#define __LIBSC_SC_SEGMENT_BASE_INCLUDED__

/// @addtogroup libsc
/// @{

#include "sc_segment.h"

#include "repository_p.h"

class sc_segment_base : public sc_segment
{
public:
	sc_segment_base() : sc_segment(), uri(""), repo(0) {}

	sc_segment_base(const sc_string &_uri, sc_repo *r) :
	  sc_segment(), uri(_uri), repo(r) {}

	~sc_segment_base() {}

	sc_type get_type(sc_addr_ll addr)
	{
		return SC_N_CONST;
	}

	sc_retval change_type(sc_addr_ll addr, sc_type)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}


	sc_addr_ll create_link(const sc_string &target_uri)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return SCADDR_LL_NIL;
	}

	iterator* create_link_iterator()
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return 0;
	}

	sc_string get_link_target(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return "";
	}

	void set_link_target(sc_addr_ll addr, const sc_string &new_target)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
	}

	bool is_link(sc_addr_ll addr)
	{
		return false;
	}


	sc_addr_ll gen_el(sc_type type)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return SCADDR_LL_NIL;
	}

	void erase_el(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
	}

	sc_addr_ll get_beg(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return SCADDR_LL_NIL;
	}

	sc_addr_ll get_end(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return SCADDR_LL_NIL;
	}

	
	sc_retval set_beg(sc_addr_ll addr, sc_addr_ll beg)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}

	sc_retval set_end(sc_addr_ll addr, sc_addr_ll end)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}


	sc_addr get_element_addr(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return SCADDR_NIL;
	}

	void set_element_addr(sc_addr_ll addr_ll, sc_addr addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
	}

	sc_iterator* create_input_arcs_iterator(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return new over_iterator();
	}

	sc_iterator* create_output_arcs_iterator(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return new over_iterator();
	}

	iterator* create_iterator_on_elements()
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return 0;
	}

	Content get_content(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return Content();
	}

	const Content *get_content_const(sc_addr_ll addr)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return 0;
	}

	sc_retval set_content(sc_addr_ll, const Content &)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}

	sc_retval erase_content(sc_addr_ll)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}

	bool find_by_content(const Content &content, ll_list &result)
	{
		return false;
	}

	sc_string get_idtf(sc_addr_ll)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return "";
	}

	sc_retval set_idtf(sc_addr_ll, const sc_string &idtf)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}

	sc_retval erase_idtf(sc_addr_ll)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}

	sc_retval has_erased_idtf(sc_addr_ll)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return RV_ERR_GEN;
	}

	sc_addr_ll find_by_idtf(const sc_string &idtf)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return SCADDR_LL_NIL;
	}


	int get_out_qnt(sc_addr_ll ll)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return 0;
	}

	int get_in_qnt(sc_addr_ll ll)
	{
		SC_ASSERT(false && "Segment doesn't support this operation");
		return 0;
	}

	sc_string get_full_uri() { return repo->get_uri() + uri; }

	void set_uri(const sc_string &str) { uri = str; }

	const sc_string& get_uri() const { return uri; }

	void set_repo(sc_repo *r) { repo = r; }

	const sc_repo* get_repo() const { return repo; }

protected:
	sc_string uri;
	sc_repo *repo;
};

/// @}

#endif // __LIBSC_SC_SEGMENT_BASE_INCLUDED__
