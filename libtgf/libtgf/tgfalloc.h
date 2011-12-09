
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


/*
 * TGF allocator is a special kind of allocator. It's very fast
 *  but it can free only all allocated memory
 *  and allows large slack
 * It's like BSD alloca, but more persistent
 * Use it if you need very fast allocations of variable size blocks
 *  and it's constraints are acceptable.
 * On POSIX systems with support of memory mapped files it is even faster
 */
#ifndef __TGF_ALLOC_H__
#define __TGF_ALLOC_H__
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _tgf_page;
struct _tgf_allocator;
typedef struct _tgf_allocator tgf_allocator;
typedef struct _tgf_page tgf_page;


/* You can safely give 0 as chunk_size
    and allocator will choose good default */
tgf_allocator *	tgf_allocator_new(int chunk_size);
void		tgf_allocator_delete(tgf_allocator *);

void *		tgf_allocator_alloc(tgf_allocator *alloc,size_t size);
void		tgf_allocator_free_all(tgf_allocator *);

/* don't expect that allocator will set your value of chunk size.
  It's only advise */
void		tgf_allocator_set_chunk_size(tgf_allocator *,size_t);
int		tgf_allocator_get_chunk_size(tgf_allocator *);

#ifdef __cplusplus
}
#endif

#endif /* __TGF_ALLOC_H__ */
