
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


#ifndef __TGF_UTIL_H__
#define __TGF_UTIL_H__
#include <string.h>
#include <stddef.h>

/* does special type of cast. It's result is lvalue */
#define CAST_LV(val,type) (*((type *)&(val)))

/* isn't this optimization paranoid ? */
#if	defined(__GNUC__)
#define __memcpy_move4(dest,src,count) ____memcpy_move4((int **)&(dest),(int **)&(src),count)
#	if defined(TGF_ARCH_i386) && defined(TGF_USE_i386ASM_MEMCPY)
/* p6 and k7 oriented memcpy */
/*
 * not using __builtin_memcpy 'couse gcc 3.0 on i386
 * optimize it only if it knows about alignment of pointers on 4 bytes
 * or special option given
 */
static __inline__ void ____memcpy_move4(int **dest,int **src,int count)
{
	/* I don't remember details about `rep' prefix
		ecx == 0 means 0 or 0x100000000 iterations
	*/
	if (count) {
		__asm__ volatile (
			"cld\n\t"
			"rep; movsl\n\t"
			: "=&S" (*src), "=&D" (*dest)
			: "0" (*src) , "1" (*dest) , "c" (count)
			: "memory","cc"
		);
	}
}
#	else
/* here gcc knows about allignment, but at cost of few additional
   instructions */
static __inline__ void ____memcpy_move4(int **dest,int **src,int count)
{
	*dest = (int *)((unsigned long)(*dest)&(unsigned long)(-4l));
	*src = (int *)((unsigned long)(*src)&(unsigned long)(-4l));
	__builtin_memcpy(*dest,*src,count*4);
	*dest += count;
	*src += count;
}
#	endif
#else
/* lets hope compilers other than GCC are also smart enough */
#if 0
/* on this case gcc does very good piece of code */
#define __memcpy_move4(dest,src,count) \
	do { \
		register int i; \
		for (i=count;i>0;i--) \
			*CAST_LV(dest,tgf_uint32 *)++ = *CAST_LV(src,tgf_uint32 *)++; \
	} while (0)
#else /* 0/1 */
#define __memcpy_move4(dest,src,count) \
	do { \
		memcpy(dest,src,count*4); \
		CAST_LV(dest,tgf_uint32 *) += count; \
		CAST_LV(src,tgf_uint32 *) += count; \
	} while (0)
#endif /* 0/1 */
#endif /* __GNUC__ */

#endif /* __TGF_UTIL_H__ */
