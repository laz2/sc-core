
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
 * @file segtable.h
 * @brief Таблица сегментов.
 * @ingroup libsc
 */

#ifndef __SEGTABLE_H__
#define __SEGTABLE_H__

#include "sc_types.h"
#include <memory.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>

#define SC_MAX_SEGMENTS 1024 /**< @brief Максимально допустимое число сегментов. */

struct segments_exausted {};

struct LIBSC_API segment_table
{
	sc_segment *table[SC_MAX_SEGMENTS];
	segment_table() 
	{
		memset(table, 0, sizeof(table));
	} 
	
	sc_uint assign(sc_segment *seg) 
	{
		for (int i = 0; i < SC_MAX_SEGMENTS; i++) {
			if (!table[i]) {
				return table[i] = seg, i;
			}
		} 
			
		printf("Segments exausted\n");
		fflush(stdout);
		throw segments_exausted();
		return 0;
	}
	void release(sc_uint index) 
	{
		assert(table[index]);
		table[index] = 0;
	}
};

extern LIBSC_API segment_table segtable;

struct LIBSC_API segment_set 
{
	enum 
	{
		bpe = sizeof(sc_uint) *8
	};

	sc_uint data[(SC_MAX_SEGMENTS + bpe - 1) / bpe];

	segment_set() 
	{
		memset(data, 0, sizeof(data));
	}

	void set(sc_uint index)throw() 
	{
		assert(index < SC_MAX_SEGMENTS);
		data[index / bpe] |= 1U << (index % bpe);
	}

	void reset(sc_uint index)throw() 
	{
		assert(index < SC_MAX_SEGMENTS);
		data[index / bpe] &= (UINT_MAX ^ (1U << index % bpe));
	}

	sc_uint is_set(sc_uint index)const throw() 
	{
		assert(index < SC_MAX_SEGMENTS);
		return data[index / bpe] &(1U << (index % bpe));
	}

	sc_uint __segnext(sc_uint c, sc_segment * &ptr)const throw() 
	{
		// could be more optimal (?)
		do {
			c++;
			if (c >= SC_MAX_SEGMENTS) {
				return c;
			} 
		} while (!is_set(c));
		ptr = segtable.table[c];
		assert(ptr);
		return c;
	}
};

#define FOR_ALL_SEGMENTS(var,segset) \
	for (sc_uint __i = UINT_MAX; (__i = segset.__segnext(__i,var)) < SC_MAX_SEGMENTS;)

#endif // __SEGTABLE_H__
