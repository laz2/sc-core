
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
 * @file sc_event.h
 * @brief Механизмы и описания событий, которые могут происходить в sc-памяти.
 * @ingroup libsc
 */

#ifndef __SC_EVENT_H__
#define __SC_EVENT_H__

#include "sc_list_head.h"
#include "sc_types.h"

#include <stdlib.h>

// _pre version work before action happens
// _pre handlers should be explicit
// If one of them returns true,
//  than holder stops activation and returns true
//   telling session to deny operation
// Installation of such waits should be privileged operation
// // _pre versions will not be implemented yet
enum sc_wait_type {
	SC_WAIT_DIE,
	SC_WAIT_DIE_PRE = SC_WAIT_DIE,
		// this one is _pre
						// but it don't have normal _pre semantic yet
	SC_WAIT_ATTACH_INPUT,
	SC_WAIT_ATTACH_OUTPUT,
	SC_WAIT_CHANGE_TYPE,
	SC_WAIT_DETACH_INPUT,
	SC_WAIT_DETACH_OUTPUT,
	
	/**
	 * @brief Begin of arc changed
	 * @arg begin of arc
	 * @arg arc
	 */
	SC_WAIT_ARC_BEG,
	
	/**
	 * @brief End of arc changed
	 * @arg arc
	 * @arg end of arc
	 */
	SC_WAIT_ARC_END,

	/**
	 * @brief Begin or end of arc changed
	 * @arg begin of arc
	 * @arg arc
	 * @arg end of arc
	 */
	SC_WAIT_ARCS_ENDS,

//	SC_WAIT_CONT_PRE,
	SC_WAIT_CONT,
//	SC_WAIT_IDTF_PRE,
	SC_WAIT_IDTF,
//	SC_WAIT_COMPLEX_GEN,		// yet unsupported
//	SC_WAIT_COMPLEX_ERASE,		// yet unsupported
	SC_WAIT_HACK_SET_MEMBER,	// until complex events supported
	SC_WAIT_HACK_IN_SET,		// until complex events supported
	SC_WAIT_MERGE,
	SC_WAIT_PRE_FIRST,
	SC_WAIT_DIE_PRE_REAL = SC_WAIT_PRE_FIRST,
	SC_WAIT_ATTACH_INPUT_PRE,
	SC_WAIT_ATTACH_OUTPUT_PRE,
	SC_WAIT_CHANGE_TYPE_PRE,
	SC_WAIT_DETACH_INPUT_PRE,
	SC_WAIT_DETACH_OUTPUT_PRE,
	SC_WAIT_HACK_SET_MEMBER_PRE,	// until complex events supported
	SC_WAIT_HACK_IN_SET_PRE,	// until complex events supported
	SC_WAIT_COUNT,        		// not wait type, actually
	// special wait types not handled here
	SC_WAIT_SEGMENT_CREATE,
	SC_WAIT_SEGMENT_ON_ADD,
	SC_WAIT_SEGMENT_ON_REMOVE,
};

// childs of sc_wait should not rely on sc_listhead
class sc_wait;
struct LIBSC_API sc_wait_listhead : protected __sc_listhead_virtual {
	sc_wait *get_next() {return (sc_wait*)next;}
	sc_wait *get_prev() {return (sc_wait*)prev;}
	void append(sc_wait_listhead *p) {__sc_listhead_virtual::append(p);}
	void prepend(sc_wait_listhead *p) {__sc_listhead_virtual::prepend(p);}
	void remove() {__sc_listhead_virtual::remove();}
};
struct LIBSC_API sc_wait_session_list : protected __sc_listhead_virtual {
	inline sc_wait *get_next_session();
	inline sc_wait *get_prev_session();
	void remove_session() {remove();}
	void append_session(sc_wait_session_list *p) {append(p);}
	void prepend_session(sc_wait_session_list *p) {prepend(p);}
};

class LIBSC_API sc_wait: public sc_wait_listhead, public sc_wait_session_list {
public:
	sc_addr element;
	sc_wait_type type;
	sc_session *session;
	sc_wait();
	void remove() {sc_wait_listhead::remove();}
/*	void append(sc_wait *p) {sc_wait_listhead::append((sc_wait_listhead*)p);}
	void prepend(sc_wait *p) {sc_wait_listhead::prepend((sc_wait_listhead *)p);}*/
	virtual ~sc_wait();
	virtual bool activate(sc_wait_type type,sc_param *,int len) {return false;}
};

LIBSC_API
sc_wait *sc_wait_session_list::get_next_session() {return (sc_wait *)(sc_wait_session_list *)next;}
LIBSC_API
sc_wait *sc_wait_session_list::get_prev_session() {return (sc_wait *)(sc_wait_session_list *)prev;}

extern LIBSC_API bool check_prm_access(sc_session *s,sc_param prm[],int len);

// commented comment // Can be optimized a little to consume less space
class sc_waits_holder {
protected:
	sc_wait_listhead *heads;
	void __clean(sc_wait_listhead *head)
	{
		sc_wait *p = head->get_next(),*next;
		while ((void *)p != (void *)head) { // this is subtle
			next = p->get_next();
			delete p;
			p = next;
		}
	}
public:
	sc_waits_holder() : heads(0) {}
	~sc_waits_holder() {clean();}
	void add(sc_wait_type type,sc_wait *w)
	{
		assert(type>=0 && type<SC_WAIT_COUNT);
		// does standard requires zero-filled area ?
		if (!heads)
			heads = new sc_wait_listhead[int(SC_WAIT_COUNT)];
		heads[type].append(w);
	}
	void remove(sc_wait *w)
	{
		w->remove();
	}
	bool is_empty(sc_wait_type type)
	{
		assert(type>=0 && type<SC_WAIT_COUNT);
		if (!heads)
			return true;
		return heads[type].get_next() == heads+type;
	}
	// returns true if one of explicit handlers handled wait
	// false - otherwise
	bool activate(sc_wait_type type,sc_param *prm,int len)
	{
		sc_wait *p,*next;
		sc_wait_listhead *head;
		assert(type>=0 && type<SC_WAIT_COUNT);
		if (!heads)
			return false;
		head = heads+type;
		p = head->get_next();
		if (type >= SC_WAIT_PRE_FIRST)
			goto m__pre;
		while (p != head) {
			next = p->get_next();	// 'couse wait can remove itself
			if (!p->session || check_prm_access(p->session,prm,len))
				p->activate(type,prm,len);
			p = next;
		}
		return false;
m__pre:
		while (p != head) {
			next = p->get_next();
			if (p->activate(type,prm,len))
				return true;
			p = next;
		}
		return false;
	}
	void clean()
	{
		//printf("cleaning waits_holder\n");
		if (heads) {
			for (int i=SC_WAIT_COUNT;i>0;)
				__clean(heads+(--i));
			delete [] heads;
		}
	}
#undef heads
};

/// Gives lvalue.
#define SC_ADDR_AS_PARAM(addr) \
	(*(reinterpret_cast <sc_param *>(&(addr))))

#endif // __SC_EVENT_H__
