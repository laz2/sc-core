
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
 *  tgfout.h - functions to write TGF
 *
 *  Created at Thu 03 May 2001 12:55:13 AM EEST by ALK
 *
 */
#ifndef __TGFOUT_H__
#define __TGFOUT_H__
#include "tgf.h"
#include "tgfmap.h"

__TGF_BEGIN_DECL

struct	_tgf_stream_out;
typedef struct	_tgf_stream_out tgf_stream_out;

LIBTGF_API tgf_stream_out *tgf_stream_out_new(void);
LIBTGF_API void	tgf_stream_out_destroy(tgf_stream_out *);
/* Thu Jun 12 16:52:24 2003 stream_out_buf now accept 4 alligned size only */
LIBTGF_API int	tgf_stream_out_buf(tgf_stream_out *,char *,int size);
LIBTGF_API void	tgf_stream_out_fd(tgf_stream_out *,int fd);
LIBTGF_API void	tgf_stream_out_custom(tgf_stream_out *,tgf_io_fn fn,void *);
LIBTGF_API int	tgf_stream_out_get_error(tgf_stream_out *);

LIBTGF_API int	tgf_stream_out_set_compression(tgf_stream_out *s,int);

LIBTGF_API int	tgf_stream_out_start(tgf_stream_out *,char *id);
LIBTGF_API int	tgf_stream_out_finish(tgf_stream_out *);
LIBTGF_API int	tgf_write_translated_command(tgf_stream_out *,tgf_command *cmd);
/* cl_id == 0 means you will not reference result of this command */
LIBTGF_API int	tgf_write_command(tgf_stream_out *,tgf_command *cmd,void *cl_id);

/* answers if element with given user_id was already written */
/* on true answers !0 */
LIBTGF_API int	tgf_stream_out_is_written(tgf_stream_out *,void *user_id);

__TGF_END_DECL

#endif /*__TGFOUT_H__*/
