
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


#ifndef _BASE_ITERATORS_H_
#define _BASE_ITERATORS_H_
#include "libsc.h"

template< sc_iterator* (sc_segment::*CreateArcsIterator)(sc_addr_ll) >
class meta_iterator : public sc_iterator {
	sc_link_addr *clink;
	sc_iterator *citer;
	sc_addr from;
	sc_session *s;
	//int dummy[3];
public:
	meta_iterator(sc_addr _from, sc_session *_s)
		: clink(_from->links), from(_from->ref()), s(_s->ref())
	{
		citer = (from->seg->*CreateArcsIterator)(from->impl);
		if (citer->is_over())
			next();
	}

	~meta_iterator()
	{
		delete citer;
		from->unref();
		s->unref();
	}

	bool is_over()
	{
		return citer == 0;
	}

	sc_addr value(sc_uint num)
	{
		if (is_over())
			return 0;
		if (!num)
			return from;
		if (num == 1)
			return citer->value(0);
		return 0;
	}

	sc_retval next()
	{
		if (!citer)
			return RV_ELSE_GEN;
		if (!citer->is_over()) {
			citer->next();
			if (!citer->is_over())
				return RV_OK;
		}
__again:
		delete citer;
		citer = 0;
		do {
			if (!clink)
				return RV_ELSE_GEN;
			if (s->is_segment_opened(clink->seg))
				break;
			clink = clink->next;
		} while (1);
		citer = (clink->seg->*CreateArcsIterator)(clink->impl);
		clink = clink->next;
		if (citer->is_over())
			goto __again;
		return RV_OK;
	}
};

/// Iterator over element input arcs in element own segment
/// and element input links (if link segment is opened in passed session).
class meta_input_iterator : public meta_iterator<&sc_segment::create_input_arcs_iterator>
{
public:
	DECLARE_FIXALLOCER(meta_input_iterator)

	meta_input_iterator(sc_addr _from, sc_session *_s)
		: meta_iterator<&sc_segment::create_input_arcs_iterator>(_from, _s)
	{}
};

/// Iterator over element output arcs in element own segment
/// and element output links (if link segment is opened in passed session).
class meta_output_iterator : public meta_iterator<&sc_segment::create_output_arcs_iterator>
{
public:
	DECLARE_FIXALLOCER(meta_output_iterator)

	meta_output_iterator(sc_addr _from, sc_session *_s)
		: meta_iterator<&sc_segment::create_output_arcs_iterator>(_from, _s)
	{}
};

#endif //_BASE_ITERATORS_H_
