
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
 * @file tgf_io.h
 * @brief Интерефейс tgf i/o функциональности для загрузки tgf в sc-память и выгрузки sc-памяти в tgf.
 * @ingroup libsc
 */

#ifndef __LIBSC_TGF_IO_H_INCLUDED__
#define __LIBSC_TGF_IO_H_INCLUDED__

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif 

#include "libsc.h"
#include <libtgf/tgfin.h>
#include <libtgf/tgfout.h>
#include <map>

/// Read TGF-commands from @p st and execute its for segment @p seg.
/// If @p colect_set is present then include generated elements in this set.
LIBSC_API
sc_retval read_tgf_to(tgf_stream_in *st, sc_session *s, sc_segment *seg, sc_addr collect_set=SCADDR_NIL);

/// Write elements from @p seg as TGF to @p stream.
/// If @p need_freeseg is true, then @p seg is free segment,
/// else write declare segment TGF-command at begin.
LIBSC_API
void write_to(sc_session *s, sc_segment *seg, tgf_stream_out *stream, bool need_freeseg=true);

/**
 * @brief Функция сохранения иерархии c URI @p uri в tgf-поток @p stream при помощи сессии @p s.
 */
LIBSC_API sc_retval pm_write_dirhier(sc_session *s, const sc_string &uri, tgf_stream_out *stream);

class LIBSC_API pm_writer
{
protected:
	typedef std::map< sc_addr, std::pair< sc_addr, sc_addr > > arc_map;
	tgf_stream_out *stream;
	sc_session *sess;
	sc_segment *free_segment;
	sc_segment *cur_segment;
	arc_map arcs;
	int lazy_limit;

	sc_retval go_to_segment(sc_segment *s);
public:
	pm_writer(sc_session *session, tgf_stream_out *s, sc_segment *free = 0) 
		: stream(s), sess(session), free_segment(free), cur_segment(free), lazy_limit(-1) {}
	
	int get_lazy_limit() 
	{
		return lazy_limit;
	}

	void set_lazy_limit(int value) 
	{
		lazy_limit = value;
	}

	sc_retval write(sc_addr element);
	sc_retval arc_sync();
	~pm_writer();
};

extern LIBSC_API int tgf2sc_type_table[];
extern LIBSC_API int sc2tgf_type_table[];

LIBSC_API Content pm_in_get_content(tgf_argument*);
LIBSC_API void fill_cont_argument(const Content &cont, tgf_argument *arg, int lazy_limit, const char *idtf);

struct LIBSC_API tgf_dump
{
	int size;
	void *data;
	tgf_stream_out *stream;
	
	tgf_dump() 
	{
		size = 0;
		data = 0;
	} 
	
	~tgf_dump() 
	{
		if (data) {
			free(data);
		} 
	}
};

LIBSC_API tgf_dump *tgf2mem_dumper_new();
LIBSC_API void tgf2mem_dumper_delete(tgf_dump*);

#endif //__LIBSC_TGF_IO_H_INCLUDED__
