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
 * @file numbers_segment.h
 * @brief Segment which contains numbered keynodes.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_NUMBERS_SEGMENT_INCLUDED__
#define __LIBSC_NUMBERS_SEGMENT_INCLUDED__

#include "sc_segment_base_p.h"

/// @addtogroup libsc
/// @{

class numbers_segment : public sc_segment_base
{
public:
	numbers_segment(const sc_string &_p,
		const sc_string &_s) : sc_segment_base(), prefix(_p), suffix(_s) {}

	numbers_segment(const sc_string &_uri, sc_repo *r,
		const sc_string &_p, const sc_string &_s) : sc_segment_base(uri, r),
		prefix(_p), suffix(_s) {}

	~numbers_segment();

	sc_type get_type(sc_addr_ll addr)
	{
		return SC_N_CONST;
	}

	sc_addr get_element_addr(sc_addr_ll addr);

	void set_element_addr(sc_addr_ll addr_ll, sc_addr addr);


	sc_iterator* create_input_arcs_iterator(sc_addr_ll addr);

	sc_iterator* create_output_arcs_iterator(sc_addr_ll addr);

	iterator* create_iterator_on_elements();


	Content get_content(sc_addr_ll addr);

	const Content *get_content_const(sc_addr_ll addr);


	sc_string get_idtf(sc_addr_ll);

	sc_retval has_erased_idtf(sc_addr_ll) { return RV_ELSE_GEN; }

	sc_addr_ll find_by_idtf(const sc_string &idtf);


	int get_out_qnt(sc_addr_ll ll) { return 0; }

	int get_in_qnt(sc_addr_ll ll) { return 0; }

	const sc_string& get_prefix() const { return prefix; }

	const sc_string& get_suffix() const { return suffix; }

private:
	sc_string prefix;
	sc_string suffix;

private:
	struct element
	{
		int num;
		sc_addr addr;
		Content content;

		element(int _num) :
			num(_num),
			content(Cont::integer(_num), TCINT),
			addr(SCADDR_NIL)
		{}
	};

	static inline element* ADDR_TO_EL(sc_addr_ll addr)
	{
		return reinterpret_cast<element *>(addr);
	}

	typedef std::map<int, element *> num2element_map;
	num2element_map num2element;

private:
	class iterator_on_elements : public sc_segment::iterator
	{
	public:
		iterator_on_elements(
			num2element_map::const_iterator _begin,
			num2element_map::const_iterator _end) :
		begin(_begin), end(_end) {}

		sc_addr_ll next()
		{
			if (is_over())
				return SCADDR_LL_NIL;

			element *el = begin->second;
			++begin;

			return sc_addr_ll(el);
		}

		bool is_over() { return begin == end; }

	private:
		num2element_map::const_iterator begin;
		num2element_map::const_iterator end;
	};

	friend class iterator_on_elements;
};


/// @}

#endif // __LIBSC_NUMBERS_SEGMENT_INCLUDED__
