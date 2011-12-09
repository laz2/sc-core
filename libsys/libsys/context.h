
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
 *        File: context.h
 *        Date: 18.07.02
 *      Author: ALK&IVP
 * Description: this file contains main interfaces &
 *              data structures for contexts
 * alk 19.02.04: changed all comments to C-style
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "libsys.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _sc_context;
typedef struct _sc_context sc_context;

/*
 * context_alloc
 *  allocates sc_context structure
 */
LIBSYS_API sc_context *  context_alloc(void);
            
/*
 * context_free
 *  deallocates & releases sc_context structure,
 *  it's fields and stack
 */
LIBSYS_API void context_free(sc_context *);

/*
 * context_init
 *  initiates data required for managing
 *  (creates top_context & initialize current_context)
 */
LIBSYS_API void context_init(void);

/*
 * context_deinit
 *  destructs data required for managing
 */
LIBSYS_API void context_deinit(void);

/* 
 * context_switchback
 *  passes control from current_context to top_context
 */
LIBSYS_API void context_switchback(void);

/*
 * context_call
 *  !this can be used by top_context only!
 *  starts gived function with parameter 
 *  at specified context
 */
LIBSYS_API void *  context_call(sc_context *,void *(*f)(void *),void *prm);

/*
 * context_recall
 *  !this can be used by top_context only!
 *  continues function at specified context
 */
LIBSYS_API void *  context_recall(sc_context *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif/*__CONTEXT_H__*/
