
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
 *        File: win32-nt4-context.h
 *        Date: 18.07.02
 *      Author: IVP
 * Description: this file contains context structure
 *              declaration for WIN32-NT4 platform
 */

#ifndef __WIN32_NT4_CONTEXT_H__
#define __WIN32_NT4_CONTEXT_H__

#include "libsys.h"

#include "context.h"

#include <windows.h>
#include <winbase.h>
#include <WindowsX.h>
#include <tchar.h>
#include <assert.h>

const int CONTEXT_STACK_SIZE = 2097152; //2Mb
 
/*
 * type of pointer for routines which can be 
 * started at new context
 */
typedef void *  (*pcontext_call_routine) (void *  prm);

typedef enum {
  FS_PREPARED,     /* prepared fiber  */
  FS_DELAYED,      /* delayed fiber   */
  FS_COMPLETED     /* completed fiber */
} fiber_state;

#ifdef __cplusplus
#define INLINE inline
#elif _MSC_EXTENSIONS
#define INLINE __inline
#elif __STDC__
#define INLINE
#else
#define INLINE
#endif

INLINE void context_switch(sc_context *);
INLINE int delayed(sc_context *);
INLINE int completed(sc_context *);

#endif/*__WIN32_NT4_CONTEXT_H__*/
