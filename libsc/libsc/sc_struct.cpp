
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

#include "precompiled_p.h"

sc_struct_searcher::~sc_struct_searcher()
{
}

sc_attr_searcher::~sc_attr_searcher()
{
}

static
sc_addr find_attr(sc_session *s,sc_addr tuple,sc_addr attr,sc_addr *_arc=0)
{
	sc_addr res=0, arc;
	search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
				tuple,
				0,
				0,
				0,
				attr
			),true,2,2,&res,1,&arc);
	if (_arc)
		*_arc = arc;
	return res;
}

sc_addr	sc_attr_searcher::search_from(sc_session *s,sc_addr from)
{
	return find_attr(s,from,attr);
}

sc_struct::sc_struct()
:
	info(0),
	size(0),
	info_size(0)
{
	resize_info();
}

sc_struct::~sc_struct()
{
	if (info) {
		for (int i = size-1;i>=0;i--)
			delete info[i].searcher;
		free(info);
	}
}

void sc_struct::resize_info()
{
	if (!info) {
		info_size = 8;
		info = (field_info *)malloc(sizeof(field_info)*info_size);
		if (!info)
			throw std::bad_alloc();
		return;
	}
	int new_size = info_size << 1;
	field_info *new_info = (field_info *)realloc(info,sizeof(field_info)*new_size);
	if (!new_info)
		throw std::bad_alloc();
	info = new_info;
	info_size = new_size;
}

void	sc_struct::add_field(int offset,sc_struct_searcher *srch)
{
	if (size >= info_size)
		resize_info();
	info[size].offset = offset;
	info[size].searcher = srch;
	size++;
}

void	sc_struct::add_attr_field(int offset,sc_addr attr)
{
	add_field(offset,new sc_attr_searcher(attr));
}

void sc_struct::unpack(sc_session *s,sc_addr tuple,void *_str) const
{
	char *str = (char *)_str;
	for (int i = size-1;i>=0;i--) {
		sc_addr a = info[i].searcher->search_from(s,tuple);
		*((sc_smart_addr *)(str+info[i].offset)) = a;
	}
}


int	sc_struct::unpack_array_attr(sc_session *s,sc_addr tuple,sc_smart_addr arr[],sc_addr attrs[],int max_cnt,int min_cnt)
{
	int i=0;
	for (;i<min_cnt;i++)
		arr[i] = find_attr(s,tuple,attrs[i]);
	for (;i<max_cnt;i++)
		if (!(arr[i] = find_attr(s,tuple,attrs[i])))
			break;
	return i;
}

int	sc_struct::unpack_array_attr_arc(sc_session *s,sc_addr tuple,
					 sc_smart_addr arr[],sc_addr attrs[],
					 sc_addr arcs[],
					 int max_cnt,int min_cnt)
{
	int i=0;
	if (!arcs)
		return unpack_array_attr(s,tuple,arr,attrs,max_cnt,min_cnt);
	for (;i<min_cnt;i++)
		arr[i] = find_attr(s,tuple,attrs[i],arcs+i);
	for (;i<max_cnt;i++)
		if (!(arr[i] = find_attr(s,tuple,attrs[i],arcs+i)))
			break;
	return i;
}


std::vector<sc_smart_addr> sc_struct::unpack_vector_attr(sc_session *s,sc_addr tuple,sc_addr attrs[],int max_cnt,int min_cnt)
{
	std::vector<sc_smart_addr> rv;
	rv.reserve(min_cnt);
	int i=0;
	for (;i<min_cnt;i++)
		rv.push_back(find_attr(s,tuple,attrs[i]));
	for (;i<max_cnt;i++) {
		sc_addr addr = find_attr(s,tuple,attrs[i]);
		if (!addr)
			break;
		rv.push_back(addr);
	}
	return rv;
}
