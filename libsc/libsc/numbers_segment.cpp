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

#include "precompiled_p.h"

#include "numbers_segment_p.h"

numbers_segment::~numbers_segment()
{
	// TODO: release and free elements
}

sc_addr numbers_segment::get_element_addr(sc_addr_ll addr_ll)
{
	return ADDR_TO_EL(addr_ll)->addr;
}

void numbers_segment::set_element_addr(sc_addr_ll addr_ll, sc_addr addr)
{
	ADDR_TO_EL(addr_ll)->addr = addr;
}

sc_iterator* numbers_segment::create_input_arcs_iterator(sc_addr_ll addr_ll)
{
	return new over_iterator();
}

sc_iterator* numbers_segment::create_output_arcs_iterator(sc_addr_ll addr_ll)
{
	return new over_iterator();
}

sc_segment::iterator* numbers_segment::create_iterator_on_elements()
{
	return new iterator_on_elements(num2element.begin(), num2element.end());
}

Content numbers_segment::get_content(sc_addr_ll addr_ll)
{
	return ADDR_TO_EL(addr_ll)->content;
}

const Content* numbers_segment::get_content_const(sc_addr_ll addr_ll)
{
	return &ADDR_TO_EL(addr_ll)->content;
}

sc_string numbers_segment::get_idtf(sc_addr_ll addr_ll)
{
	std::ostringstream str;	
	str << ADDR_TO_EL(addr_ll)->num;
	return str.str();
}

sc_addr_ll numbers_segment::find_by_idtf(const sc_string &idtf)
{
	size_t suffix_start = idtf.size() - suffix.size();

	if (idtf.compare(0, prefix.size(), prefix) ||
			idtf.compare(suffix_start, suffix.size(), suffix))
		return SCADDR_LL_NIL;

	char buf[33];
	if ((suffix_start - prefix.size()) >= sizeof(buf))
		return SCADDR_LL_NIL;

	{
		size_t i = prefix.size();
		for (; i < suffix_start; ++i)
			buf[i] = idtf[i];

		buf[i] = '\0';
	}

	int num;

	std::istringstream istr(buf);
	istr >> num;

	num2element_map::const_iterator it = num2element.find(num);
	if (it != num2element.end()) {
		return sc_addr_ll(it->second);
	} else {
		element *new_el = new element(num);
		new_el->addr = new sc_global_addr(this, sc_addr_ll(new_el));
		num2element.insert(num2element_map::value_type(num, new_el));
		return sc_addr_ll(new_el);
	}
}
