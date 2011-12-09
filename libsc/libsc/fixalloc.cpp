
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

#ifdef USE_FIXALLOCER

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "fixalloc.h"
#include <stdlib.h>
#include "abort.h"


#if WIN32

#include <Windows.h>

void *falloc_getpage()
{
	void *res = VirtualAlloc(0, FALLOC_PAGESIZE, MEM_COMMIT, PAGE_READWRITE);
	if (!res)
		SC_ABORT();
	return res;
}

void falloc_relpage(void *ptr)
{
	if (!VirtualFree(ptr, FALLOC_PAGESIZE, MEM_RELEASE))
		SC_ABORT();
}

#else

#include <sys/mman.h>
#include <unistd.h>

#if 0
static
char *	block, *block_end;
#endif

void *	falloc_getpage()
{
#if	0
	// Hm. This does not give more speed. Why ?
	if (!block || block == block_end) {
		block = (char *)mmap(0,FALLOC_PAGESIZE*4,PROT_READ|PROT_WRITE,
				MAP_PRIVATE|MAP_ANONYMOUS,0,0);
		if (block == MAP_FAILED)
			SC_ABORT();
		block_end = block+FALLOC_PAGESIZE*4;
	}
	void *res = block;
	block += 4096;
	return res;
#else

	void *res = mmap(0,FALLOC_PAGESIZE,
			PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
	if (res == MAP_FAILED)
		SC_ABORT();
	return res;
#endif
}

void	falloc_relpage(void *ptr)
{
	if (munmap(ptr,FALLOC_PAGESIZE)<0)
		SC_ABORT();
}

#endif

#endif
