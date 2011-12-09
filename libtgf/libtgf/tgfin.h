
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
 *  SCPx processing module project.
 *
 *  tgfin.h - functions to read TGF
 *
 *  Created at Thu 03 May 2001 12:53:46 AM EEST by ALK
 *  New format, interface generalization and speedup
 *    at Thu 08 Nov 2001 02:55:15 AM EET by ALK
 *
 */
#ifndef __TGFIN_H__
#define __TGFIN_H__
#include "tgf.h"
#include "tgfmap.h"

__TGF_BEGIN_DECL

struct _tgf_stream_in;
typedef struct _tgf_stream_in tgf_stream_in;

LIBTGF_API tgf_stream_in *tgf_stream_in_new(void);
LIBTGF_API void	tgf_stream_in_destroy(tgf_stream_in *);
LIBTGF_API int	tgf_stream_in_buf(tgf_stream_in *,char *,int);
LIBTGF_API void	tgf_stream_in_fd(tgf_stream_in *,int fd);
LIBTGF_API void	tgf_stream_in_custom(tgf_stream_in *,tgf_io_fn fn,void *cust);
LIBTGF_API int	tgf_stream_in_get_error(tgf_stream_in *);

LIBTGF_API int	tgf_stream_in_start(tgf_stream_in *,tgf_header_user *hdr);
/* TODO: tgf_stream_in_finish() */

/* lowlevel function */
/* it gives you untranslated command */
LIBTGF_API int	tgf_read_command(tgf_stream_in *stream,tgf_command **cmd);

/* returns 0 on OK */
/* you may put NULL to cl_id if you sure command will be not referrenced */
typedef int (*tgf_in_cb)(tgf_command *cmd,void **cl_id,void *extra);

/* hilevel function */
/* it uses your callback */
/* callback gets translated command */
LIBTGF_API int	tgf_stream_in_process_command(tgf_stream_in *s,tgf_in_cb cb,void *extra);
LIBTGF_API int	tgf_stream_in_process_all(tgf_stream_in *s,tgf_in_cb cb,void *extra);

__TGF_END_DECL

#endif /*__TGFIN_H__*/
