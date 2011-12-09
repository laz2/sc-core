
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
 *        File: win32-nt4-context.cpp
 *        Date: 18.07.02
 *      Author: IVP
 * Description: this file stores sc_context functionality
 *              for WIN32-NT4 platform
 */

#include "win32-nt4-context.h"

sc_context *  current_context = NULL;
sc_context *  top_context = NULL;

/* complete declaration for structure _sc_context
 * for WIN32-NT4 platform
 */
struct _sc_context {
   PVOID pFiber;
   fiber_state state;
   DWORD stack_size;
   pcontext_call_routine pRoutine;
   void **  pParameter;
   sc_context *  pParent;
};

/*
 * fiber procedure
 */
void WINAPI fp (void *  p) {
  sc_context *  pSelf = (sc_context *)p;
  *(pSelf->pParameter) = (pSelf->pRoutine)(*(pSelf->pParameter));
  pSelf->state = FS_COMPLETED;
  if (pSelf->pParent != NULL) context_switch(pSelf->pParent);
  return;
}

/*
 * implementation for context_alloc
 */
sc_context *  context_alloc() {
  sc_context *  pContext = (sc_context *)malloc(sizeof(sc_context));
  if (pContext != NULL) {
    pContext->pFiber     = NULL;
    pContext->state      = FS_PREPARED;
    pContext->stack_size = CONTEXT_STACK_SIZE;
    pContext->pRoutine   = NULL;
    pContext->pParameter = NULL;
    pContext->pParent    = NULL;
  }
  return pContext;
}

/*
 * implementation for context_complete
 */
INLINE
void context_complete(sc_context *  pContext) {

  assert(pContext != NULL);

  if (pContext->pFiber != NULL) DeleteFiber(pContext->pFiber);
  if (pContext->pParameter != NULL) free(pContext->pParameter);
  pContext->pFiber     = NULL;
  pContext->pParameter = NULL;
  pContext->pRoutine   = NULL;
  pContext->state      = FS_COMPLETED;
  return;
}

/*
 * implementation for context_free
 */
void context_free(sc_context *  pContext) {
  context_complete(pContext);
  free(pContext);
  return;
}

/*
 * implementation for context_init
 */
void context_init() {
  PVOID pTopFiber = (top_context == NULL || top_context != current_context)? NULL: top_context->pFiber;
  context_deinit();

  /*
   * there is strange but fibers always exist
   * PVOID pCurrentFiber = GetCurrentFiber();
   * if (pCurrentFiber != NULL) DeleteFiber(pCurrentFiber);
   */

  current_context = top_context = context_alloc();
  if (top_context != NULL) top_context->pFiber = (pTopFiber == NULL)? ConvertThreadToFiber(NULL): pTopFiber;
  return;
}

/*
 * implementation for context_deinit
 */
void context_deinit() {
  if (current_context != top_context && current_context != NULL) context_free(current_context);
  if (top_context != NULL) {
    top_context->pFiber = NULL;
    context_free(top_context);
    top_context = NULL;
  }
  current_context = NULL;
  return;
}

/*
 * implementation for context_switch
 */
void context_switch(sc_context *  pContext) {
 
  assert(pContext != NULL);

  if (pContext->pFiber != NULL && !completed(pContext)) {
    if (current_context != NULL) if (!completed(current_context)) current_context->state = FS_DELAYED;
    current_context = pContext;
    SwitchToFiber(pContext->pFiber);
  }
  return;
}

/*
 * implementation for context_switchback
 */
void context_switchback() {
  if (top_context != NULL) {
    if (current_context != top_context && delayed(top_context)) {
      context_switch(top_context);
    }
  }
  return;
}

/*
 * implementation for context_call
 */
void *  context_call(sc_context *  pContext,
                     pcontext_call_routine f,
                     void *  prm) {

  assert(pContext != NULL);
  assert(f        != NULL);

  if (pContext->pFiber != NULL) context_complete(pContext);
  pContext->pRoutine      = f;
  pContext->pParameter    = (void **)malloc(sizeof(void *));
  *(pContext->pParameter) = prm;
  pContext->pParent       = current_context;
  pContext->state         = FS_PREPARED;
  pContext->pFiber        = CreateFiber(pContext->stack_size, fp, pContext);
  return context_recall(pContext);
}

/*
 * implementation for context_recall
 */
void *  context_recall(sc_context *  pContext) {
  void *  r;

  assert(pContext != NULL);

  if (top_context == current_context) /*especially for PM realization*/
    context_switch(pContext);
  r = (pContext->pFiber != NULL)? 
    completed(pContext)? *(pContext->pParameter): NULL:
    NULL;
  if (completed(pContext)) context_complete(pContext);
  return r;
}

INLINE int delayed(sc_context *  pC)   {return (pC->state == FS_DELAYED);};

INLINE int completed(sc_context *  pC) {return (pC->state == FS_COMPLETED);};
