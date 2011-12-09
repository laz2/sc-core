
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
 * @file sc_struct.h
 * @brief Механизм для пакетных запросов к sc-памяти.
 * @ingroup libsc
 */

#ifndef __SC_STRUCT_H__
#define __SC_STRUCT_H__
	
#include "libsc.h"
#include "sc_smart_addr.h"
#include <new>
#include <vector>
#include <stddef.h>

#ifdef __GNUC__
// macro for add_field method of sc_struct
// some little hack I found on GCC mailing list to suppress warning
// Future gcc's have switch to turn this warning off
// NOTE: this macro may really cause problems with virtual inheritance
//  use it only for simple structures NOT classes
#undef offsetof
#define offsetof(str,field) ((size_t)&((str *)(1))->field - 1)
#endif 

class LIBSC_API sc_struct_searcher 
{
public:
	virtual sc_addr search_from(sc_session *, sc_addr) = 0;
	virtual ~sc_struct_searcher();
};

class LIBSC_API sc_attr_searcher : public sc_struct_searcher
{
	sc_smart_addr attr;
public:
	sc_attr_searcher(sc_addr _attr): attr(_attr) {}
	virtual sc_addr search_from(sc_session *, sc_addr);
	virtual ~sc_attr_searcher();
};

class LIBSC_API sc_struct
{
	struct field_info {
		int offset;
		sc_struct_searcher *searcher;
	};
	field_info *info;
	int size;
	int info_size;
	void resize_info() /* throw(std::bad_alloc)*/;
public:
	sc_struct() /*throw(std::bad_alloc)*/;
	~sc_struct() /*throw()*/;
	void add_field(int offset, sc_struct_searcher*);
	void add_attr_field(int offset, sc_addr attr);
	void unpack(sc_session *, sc_addr, void *a_struct)const;
	static int unpack_array_attr(sc_session *s, sc_addr tuple, sc_smart_addr arr[], sc_addr attrs[], int max_cnt, int min_cnt = 0);
	static int unpack_array_attr_arc(sc_session *s, sc_addr tuple, sc_smart_addr arr[], sc_addr attrs[], sc_addr arcs[], int max_cnt, int min_cnt = 0);
	static std::vector<sc_smart_addr> unpack_vector_attr(sc_session *s, sc_addr tuple, sc_addr attrs[], int max_cnt, int min_cnt = 0);
};


#endif // __SC_STRUCT_H__
