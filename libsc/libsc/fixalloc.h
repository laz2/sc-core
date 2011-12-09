
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
 * @file fixalloc.h
 * @brief Быстрый аллокатор блоков фиксированного размера.
 * @ingroup libsc
 */

#ifndef __FIXALLOC_H__
#define __FIXALLOC_H__

#include <new>
#include <stdio.h>
#include "abort.h"

#ifdef USE_FIXALLOCER

#if defined(_MSC_VER)
// disable warning for MSVC C++ exception specification
// ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
#endif 

// TODO: make this configurable parameters later
#define FALLOC_PAGESIZE 4096 * 10

#define FALLOC_PAGES_ALLIGNED 1

#define FALLOC_DEBUG 0
#define FALLOC_MAGIC 0x1981dead

// getpage cannot return 0, but may abort the program
extern void *falloc_getpage();
extern void falloc_relpage(void *);

// to get compile time error
// cannot find smarter way
#define DECLARE_ERROR_CONDITION(name) \
template <bool P> \
struct name { \
}; \
template <> \
struct name <false> {\
	enum { \
		ok = 1 \
	}; \
};

DECLARE_ERROR_CONDITION(size_is_less_than_pointer_size)
DECLARE_ERROR_CONDITION(size_is_not_pointer_size_alligned)

template <typename T>
class fixallocer {
public:
	struct page_hdr {
#if		FALLOC_DEBUG
		unsigned magic;
#endif
		page_hdr *prev;
		page_hdr *next;
		void **free_place;
		int free_blocks;
	};
	enum {
		size_error = !size_is_less_than_pointer_size<sizeof(T) < sizeof(void *)>::ok,
		allign_error = !size_is_not_pointer_size_alligned<sizeof(T)%sizeof(void*)>::ok,
		page_size = FALLOC_PAGESIZE,
		blocks_per_page = (page_size - sizeof(page_hdr))/sizeof(T),
		// data offs is pointer size alligned here
		data_offs = page_size - blocks_per_page*sizeof(T),
		data_index = data_offs/sizeof(void *)
	};
private:
	page_hdr *last_page;
	void init_page(struct page_hdr *page)
	{
		T *p = (T *)((void **)page + data_index);
		page->free_place = (void **)p;
		page->free_blocks = blocks_per_page;
		for (int i=blocks_per_page-1;i>0;i--,p++) {
			*(T **)p = p + 1;
		}
#		if FALLOC_DEBUG
			page->magic = FALLOC_MAGIC;
#		endif
		*(T **)p = 0;
	}
	page_hdr* add_page()
	{
//		printf("add_page\n");
		page_hdr *page = (page_hdr *)falloc_getpage();
		init_page(page);
		page->next = 0;
		page->prev = last_page;
		if (last_page)
			last_page->next = page;
		return last_page = page;
	}
	void rem_page(page_hdr *page)
	{
		if (page == last_page)
			last_page = page->prev;
		if (page->prev)
			page->prev->next = page->next;
		if (page->next)
			page->next->prev = page->prev;
	}
public:
	fixallocer() : last_page(0)
	{
	}
	~fixallocer()
	{
	}
	T *alloc()
	{
		page_hdr *p = last_page;
		while (p) {
			if (p->free_blocks)
				goto __do_alloc;
			p = p->prev;
		}
		p = add_page();
	__do_alloc:
		T *q = (T *)(p->free_place);
		p->free_place = (void **)*(p->free_place);
		p->free_blocks--;
		return q;
	}
	void free(T *block)
	{
		page_hdr *p;
#		if FALLOC_PAGES_ALLIGNED
			p = (page_hdr *)((unsigned)(block) & ~(FALLOC_PAGESIZE-1));
#			if FALLOC_DEBUG
			if (p->magic != FALLOC_MAGIC)
				SC_ABORT();
#			endif
#		else
			Non-compileable variant!
#		endif
#		if FALLOC_DEBUG
			if ((unsigned)block & (sizeof(void *)-1))
				SC_ABORT();
#		endif
		//asm volatile ("");
		*((void **)block) = p->free_place;
		p->free_place = (void **)block;
		p->free_blocks++;
	}
};

#define DECLARE_FIXALLOCER(t) \
	void *operator new(size_t) throw (std::bad_alloc); \
	void operator delete(void *);

#define DEFINE_FIXALLOCER(t) \
	fixallocer<t> t##_allocer; \
	void * t::operator new(size_t __dummy) throw(std::bad_alloc) \
	{ \
		return ::t##_allocer.alloc(); \
	} \
	void t::operator delete(void *p) {::t##_allocer.free((t *)p);}

#else //FIXALLOCER

#define DECLARE_FIXALLOCER(a)
#define DEFINE_FIXALLOCER(a)

#endif

#endif // __FIXALLOC_H__
