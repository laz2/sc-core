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
 * @file sc_addr.h
 * @brief SC-addr and other structures.
 * @ingroup libsc
 */

#ifndef __SCADDR_H__
#define __SCADDR_H__

/// @addtogroup libsc
/// @{

#include "sc_segment.h"
#include "fixalloc.h"

extern LIBSC_API int event_lock;

struct lock_event 
{
	lock_event() 
	{
		event_lock++;
	} 
	
	~lock_event() 
	{
		event_lock--;
	}
};

/// Global locks all event handlers in SC-memory.
#define LOCK_EVENT() lock_event ___event_locker

struct LIBSC_API sc_addr_store
{
	sc_segment *seg; ///< Own segment of #impl.
	sc_addr_ll impl; ///< Implementation sc-addresses element.

	sc_addr_store(sc_segment *_seg, sc_addr_ll _impl) : seg(_seg), impl(_impl) {}
	sc_addr_store() : seg(0), impl(0) {}
	sc_addr_store(const sc_addr_store &src) : seg(src.seg), impl(src.impl) {}
};

struct LIBSC_API sc_link_addr : public sc_addr_store
{
	sc_link_addr *next, **prev;

	sc_link_addr(sc_segment *seg, sc_addr_ll impl) : sc_addr_store(seg, impl), next(0), prev(0){}
	sc_link_addr() : sc_addr_store(), next(0), prev(0){}
};

#define SC_ADDR_REFCNT_BARRIER	0x100

class LIBSC_API sc_global_addr : public sc_addr_store
{
	friend struct sc_link_addr;
public:
	struct die_handle;
	typedef void(*die_handler)(die_handle *, sc_addr);
	struct die_handle
	{
		void *extra;
		die_handle *next;
		die_handler handler;
	};

	DECLARE_FIXALLOCER(sc_global_addr)

#ifdef COUNT_ELEMENTS   
	static int addr_cnt;
#endif 

	/// First element in list of links in foreign segments to #impl.
	sc_link_addr *links;
	int refcnt;
	bool dead;
	bool locked;
	bool zombie; ///< Element is dead but address is referrenced.
	sc_activity *activity;
	die_handle *observers;
	sc_waits_holder waits;

	sc_global_addr(sc_segment *seg, sc_addr_ll impl) 
		: sc_addr_store(seg, impl), links(0), refcnt(SC_ADDR_REFCNT_BARRIER),
		dead(0), locked(0), zombie(false), activity(0), observers(0), links_qnt(0)
	{
#ifdef COUNT_ELEMENTS
		addr_cnt++;
#endif
		in_qnt = seg->get_in_qnt(impl);
		out_qnt = seg->get_out_qnt(impl);
	} 

	sc_global_addr() 
		: sc_addr_store(), links(0), refcnt(SC_ADDR_REFCNT_BARRIER), dead(0),
		locked(0), zombie(false), activity(0), observers(0), links_qnt(0),
		in_qnt(0), out_qnt(0)
	{
#ifdef COUNT_ELEMENTS
		addr_cnt++;
#endif
	}

#ifdef COUNT_ELEMENTS
	~sc_global_addr() 
	{
		addr_cnt--;
	}
#endif 

	sc_global_addr *ref()
	{
		refcnt++;
		return this;
	}

	void unref();

	bool is_dead() const { return dead; }

	void die();

	bool activate(sc_wait_type type, sc_param *params, int len);

	sc_waits_holder *get_waits() { return  &waits; }

	void add_observer(die_handle *h) throw();

	bool remove_observer(die_handle *h) throw();

	sc_link_addr* find_link(sc_segment *seg);

	/// keeps list ordered by addr->seg
	void add_link(sc_link_addr *la);

	void remove_link(sc_link_addr *link);

	size_t get_links_qnt() const { return links_qnt; }


	size_t get_in_qnt() const { return in_qnt; }

	void up_in_qnt() { ++in_qnt; }

	void down_in_qnt() { --in_qnt; }


	size_t get_out_qnt() const { return out_qnt; }

	void up_out_qnt() { ++out_qnt; }

	void down_out_qnt() { --out_qnt; }

private:
	/// Inserts after place and before *place.
	inline static
	void insert_link(sc_link_addr **place, sc_link_addr *addr) 
	{
		if (*place)
			(*place)->prev = &addr->next;

		addr->next =  *place;
		*place = addr;
		addr->prev = place;
	}

private:
	/// Count of links for this #sc_addr.
	/// @see #links
	size_t links_qnt;

	/// Count of input arcs in #impl in own and all links segments.
	size_t in_qnt;

	/// Count of output arcs from #impl in own and all links segments.
	size_t out_qnt;
};

/**
 * sc_plain_set is unmutable array of sc_addr'esses. 0 is end-of-list delimiter
 * sc_addr with seg == SC_SET_SEG is actually sc_set. sc_set is stored in impl field.
 * This allows us to pass sc_sets as constraint params.
 *
 * NOTE: that is sc_set is not sc-memory element, it is just internal construct useful mainly for search purposes.
 *
 * NOTE: that sc_set does not increment references on elements it contains for efficiency reasons.
 * This means that sc_sets are considered to live not long (less than scheduling quant).
 * But if someone wants to keep sc_set alive for longer period of time, {s,}he should ref() individual set elements
 * "by hand".
 *
 * sc_addr'esses which represent sc_sets can be ref/unref'ed as usual addresses.
 * To destroy sc_addr representing sc_set, you should use die() method of sc_global_addr.
 */
typedef sc_addr *sc_plain_set;

/**
 * This define represents magic sc_set segment. All addresses with this segment are actually sc_set.
 */
#define SC_SET_SEG (sc_segment *)(1981)

/// @}

#endif // __SCADDR_H__
