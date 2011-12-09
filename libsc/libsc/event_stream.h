
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
 * @file event_stream.h
 * @brief Описание потока событий sc-памяти.
 * @ingroup libsc
 */

#ifndef __EVENT_STREAM_H__
#define __EVENT_STREAM_H__

#include "sc_types.h"
#include "sc_content.h"

enum sc_event_type 
{
	SCEV_GENEL = 1,  /* idtf, type */
	SCEV_CREATELINK,  /* target */
	SCEV_SETBEG,  /* arc, beg */
	SCEV_SETEND,  /* arc, end */
	SCEV_SETIDTF,  /* oldidtf, newidtf */
	SCEV_ERASEEL,  /* idtf */
	SCEV_SETCONT,  /* idtf, cont */
	SCEV_CHANGETYPE, SCEV_UNLINK
};

struct event_struct 
{
	sc_event_type type;
	sc_string idtf;
	sc_string aux;
	sc_type t;
	sc_type t2;
	Content cont;
};

class event_sink 
{
public:
	virtual void operator()(const event_struct &) = 0;
};

class event_fifo 
{
	enum 
	{
		size = 32
	};

	event_struct fifo[size];
	int head, tail;
	event_sink &sink;
	event_struct &get_struct() 
	{
		event_struct &rv = fifo[head];
		head = (head + 1) % size;
		return rv;
	}

	void release_struct() 
	{
		if (head == tail) {
			__sync();
		} 
	}

	// here we know that there is at least one entry in fifo
	void __sync() 
	{
		do {
			sink(fifo[tail]);
			tail = (tail + 1) % size;
		} while (tail != head);
	}

public:
	event_fifo(event_sink &_sink): sink(_sink) 
	{
		head = tail = 0;
	}

	~event_fifo() 
	{
		sync();
	}
	void put_genel(const sc_string &idtf, sc_type type) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_GENEL;
		str.idtf = idtf;
		str.t = type;
		release_struct();
	}

	void put_create_link(const sc_string &idtf) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_CREATELINK;
		str.idtf = idtf;
		release_struct();
	}

	void put_erase_el(const sc_string &idtf) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_ERASEEL;
		str.idtf = idtf;
		release_struct();
	}

	void put_setbeg(const sc_string &idtf, const sc_string &aux) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_SETBEG;
		str.idtf = idtf;
		str.aux = aux;
		release_struct();
	}

	void put_setend(const sc_string &idtf, const sc_string &aux) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_SETEND;
		str.idtf = idtf;
		str.aux = aux;
		release_struct();
	}

	void put_setidtf(const sc_string &idtf, const sc_string &aux) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_SETIDTF;
		str.idtf = idtf;
		str.aux = aux;
		release_struct();
	}

	void put_setcont(const sc_string &idtf, const Content &cont) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_SETCONT;
		str.idtf = idtf;
		str.cont = cont;
		release_struct();
	}

	void put_unlink() 
	{
		event_struct &str = get_struct();
		str.type = SCEV_UNLINK;
		release_struct();
	}

	void put_changetype(const sc_string &idtf, sc_type t, sc_type t2) 
	{
		event_struct &str = get_struct();
		str.type = SCEV_CHANGETYPE;
		str.idtf = idtf;
		str.t = t;
		str.t2 = t2;
		release_struct();
	}

	void sync() 
	{
		if (tail != head) {
			__sync();
		} 
	}
};

#endif // __EVENT_STREAM_H__
