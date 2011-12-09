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
 * @file tmp_segment.h
 * @brief In-memory sc-segment.
 * @ingroup libsc
 */

#ifndef __LIBSC_TMP_SEGMENT_H_INCLUDED__
#define __LIBSC_TMP_SEGMENT_H_INCLUDED__

#include <map>

#include "libsc.h"
#include "sc_list_head.h"

#include "sc_segment_base_p.h"
#include "repository_p.h"
#include "sc_core_p.h"
#include "content_index_p.h"

struct element;
DECLARE_LIST_HEAD(struct, element)

class tmp_segment_iterator;

struct element_link;
DECLARE_LIST_HEAD(struct, element_link)

class tmp_segment: public sc_segment_base
{
private:
	// TODO: find the way to optimize storage
	typedef std::pair<sc_string, sc_addr_ll> my_pair;
	typedef std::map<sc_string, sc_addr_ll> my_map;

	element_listhead list_head;
	my_map idtf_map;
	unsigned idtf_counter;

	content_index cindex;

	element *create_element(sc_type type);
	void attach_input_arc(sc_addr_ll el, sc_addr_ll arc);
	void attach_output_arc(sc_addr_ll el, sc_addr_ll arc);
	void detach_input_arc(sc_addr_ll el, sc_addr_ll arc);
	void detach_output_arc(sc_addr_ll el, sc_addr_ll arc);
	
	/// @return true if successful else false.
	bool register_idtf(const sc_string &idtf, sc_addr_ll el);
	
	bool deregister_idtf(const sc_string &idtf);
	void clear_links(element_link_listhead *head);
	sc_retval generate_idtf(sc_addr_ll addr);

public:
	tmp_segment(const sc_string &_uri, sc_repo *r);
	~tmp_segment();

	class tmp_segment_iterator : public sc_segment::iterator
	{
	protected:
		element *current;
		tmp_segment *seg;
		
	public:
		tmp_segment_iterator(tmp_segment *_seg);
		sc_addr_ll next();
		bool is_over();
	};
		
	friend class tmp_segment_iterator;
		
	class tmp_links_iterator : public tmp_segment_iterator
	{
	public:
		tmp_links_iterator(tmp_segment *seg);
		sc_addr_ll next();
	};

	friend class tmp_links_iterator;

	sc_type get_type(sc_addr_ll addr);
	sc_retval change_type(sc_addr_ll addr, sc_type type);
	
	bool is_link(sc_addr_ll addr);
	sc_addr_ll create_link(const sc_string &target_uri);
	sc_string get_link_target(sc_addr_ll addr);
	void set_link_target(sc_addr_ll addr, const sc_string &new_target);
	iterator *create_link_iterator();

	sc_addr_ll gen_el(sc_type type);

	void erase_el(sc_addr_ll addr);

	sc_addr_ll get_beg(sc_addr_ll addr);
	sc_addr_ll get_end(sc_addr_ll addr);
	sc_retval set_beg(sc_addr_ll addr, sc_addr_ll beg);
	sc_retval set_end(sc_addr_ll addr, sc_addr_ll end);

	sc_addr get_element_addr(sc_addr_ll addr);
	void set_element_addr(sc_addr_ll ll, sc_addr addr);

	sc_iterator *create_input_arcs_iterator(sc_addr_ll addr);
	sc_iterator *create_output_arcs_iterator(sc_addr_ll addr);

	iterator *create_iterator_on_elements();

	Content get_content(sc_addr_ll addr);
	const Content *get_content_const(sc_addr_ll addr);
	sc_retval set_content(sc_addr_ll addr, const Content &c);
	sc_retval erase_content(sc_addr_ll addr);
	bool find_by_content(const Content &content, ll_list &result);

	sc_string get_idtf(sc_addr_ll addr);
	sc_retval set_idtf(sc_addr_ll addr, const sc_string &idtf);
	sc_retval has_erased_idtf(sc_addr_ll addr);
	sc_retval erase_idtf(sc_addr_ll addr);
	sc_addr_ll find_by_idtf(const sc_string &idtf);

	int get_in_qnt(sc_addr_ll ll);
	int get_out_qnt(sc_addr_ll ll);
};

#endif // __LIBSC_TMP_SEGMENT_H_INCLUDED__
