/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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
 * @file sc_segment.h
 * @brief Interface of sc-segment.
 * @ingroup libsc
 */

#ifndef __LIBSC_SC_SEGMENT_INCLUDED__
#define __LIBSC_SC_SEGMENT_INCLUDED__

/// @addtogroup libsc
/// @{

#include "config.h"

#include "sc_types.h"
#include "sc_event.h"
#include "event_stream.h"
#include "segtable.h"

/**
 * @brief ������� ����� sc-��������.
 */
class LIBSC_API sc_segment
{
public:
	int refcnt;

	/**
	 * ���� �������� � �������� META � ������ ����������.
	 */
	sc_addr sign;

	/**
	 * ������ �������� � ������� ���������.
	 */
	sc_uint index;

	/**
	 * ����, ������� ���������, ��� ������ ������ ����� ���� ������.
	 */
	bool dead;

	/**
	 * ����, ������� ���������, ��� ������� ���������.
	 */
	bool ring_0;

#ifdef SYNC_MEMORY
	ReentrantLock lock;

	#define CUR_SEGMENT_LOCK_SECTION LOCK_SECTION(lock)
	#define CUR_SEGMENT_LOCAL_LOCK LOCAL_LOCK(lock)
	#define SEGMENT_LOCAL_LOCK(seg) LOCAL_LOCK(seg->lock)
	#define SEGMENT_LOCK_SECTION(seg) LOCK_SECTION(seg->lock)
#else
	#define CUR_SEGMENT_LOCK_SECTION
	#define CUR_SEGMENT_LOCAL_LOCK
	#define SEGMENT_LOCAL_LOCK(seg)
	#define SEGMENT_LOCK_SECTION(seg)
#endif

	/**
	 * ������� �������, ������������ � ��������.
	 */
	event_fifo *evstream;


	int mapcnt;
	#ifdef COUNT_ELEMENTS
		static int seg_cnt;
	#endif

	sc_wait_listhead on_add, on_remove;

	// this iterator is not required to be safe.
	// this iterator must not ref() segment !
	/**
	 * @brief �������� �� ��������� � ��������.
	 */
	class LIBSC_API iterator
	{
	public:
		virtual sc_addr_ll next() = 0;
		virtual bool is_over() = 0;
		virtual ~iterator(){}
	};

	sc_segment(): refcnt(1), sign(0), dead(false), ring_0(false), evstream(0), mapcnt(0)
	{
		index = segtable.assign(this);

		#ifdef COUNT_ELEMENTS
			seg_cnt++;
		#endif
	}

	void add_on_add(sc_wait *w)
	{
		on_add.append(w);
	}

	void add_on_remove(sc_wait *w)
	{
		on_remove.append(w);
	}

private:
	void __clear_waitlist(sc_wait_listhead *head)
	{
		sc_wait *p = head->get_next(),  *next;
		while ((void*)p != (void*)head) {
			// this is subtle
			next = p->get_next();
			delete p;
			p = next;
		}
	}

public:
	// release all addresses used
	virtual ~sc_segment()
	{
		#ifdef COUNT_ELEMENTS
			seg_cnt--;
		#endif
		delete evstream;
		segtable.release(index);
		__clear_waitlist(&on_add);
		__clear_waitlist(&on_remove);
	}

	// after that str belongs to segment
	//	and will be delete'ed on segment death
	// But sink is not owned by segment !
	sc_retval listen_segment(event_fifo *str)
	{
		if (evstream) {
			return RV_ELSE_GEN;
		}
		evstream = str;
		return RV_OK;
	}

	// on RV_OK segment deletes evstream
	sc_retval unlisten_segment(event_fifo *str)
	{
		if (str != evstream)
		{
			return RV_ELSE_GEN;
		}
		delete evstream;
		evstream = 0;
		return RV_OK;
	}

	sc_segment *ref()
	{
		refcnt++;
		return this;
	}

	void unref()
	{
		if (!--refcnt) {
			delete this;
		}
	}

	void die()
	{
		dead = true;
		unref();
	}

	/**
	 * @return ��� ��������.
	 */
	virtual sc_type get_type(sc_addr_ll addr) = 0;

	/**
	 * @brief ������������� ��� ��������.
	 */
	virtual sc_retval change_type(sc_addr_ll addr, sc_type) = 0;


	/**
	 * @brief ������� ������ �� URI.
	 */
	virtual sc_addr_ll create_link(const sc_string &target_uri) = 0;

	/**
	 * @return �������� �� ������� ������� ��������.
	 */
	virtual iterator *create_link_iterator() = 0;

	/**
	 * @return ������� URI ��� ������.
	 */
	virtual sc_string get_link_target(sc_addr_ll addr) = 0;

	/**
	 * ������������� ������� URI ��� ������.
	 */
	virtual void set_link_target(sc_addr_ll addr, const sc_string &new_target) = 0;

	/**
	 * @return true ���� ������� ������, ����� false.
	 */
	virtual bool is_link(sc_addr_ll addr) = 0;


	/**
	 * ������� �������, �� �� ������.
	 */
	virtual sc_addr_ll gen_el(sc_type type) = 0;

	/**
	 * ������� �������. ��� �������� �������� ��� �� ����� �������.
	 */
	virtual void erase_el(sc_addr_ll addr) = 0;

	/**
	 * @return ��������� ������� ��������-����.
	 */
	virtual sc_addr_ll get_beg(sc_addr_ll addr) = 0;

	/**
	 * @return �������� ������� ��������-����.
	 */
	virtual sc_addr_ll get_end(sc_addr_ll addr) = 0;

	/**
	 * ������������� ��� ��������-���� ��������� �������.
	 */
	virtual sc_retval set_beg(sc_addr_ll addr, sc_addr_ll beg) = 0;

	/**
	 * ������������� ��� ��������-���� �������� �������.
	 */
	virtual sc_retval set_end(sc_addr_ll addr, sc_addr_ll end) = 0;


	/**
	 * @return sc-����� ��� ����������� ��������.
	 */
	virtual sc_addr get_element_addr(sc_addr_ll addr) = 0;

	/**
	 * ������������� sc-����� ��� ����������� ��������.
	 */
	virtual void set_element_addr(sc_addr_ll addr_ll, sc_addr addr) = 0;


	/**
	 * @return �������� �� ��������� ����� ��� ��������.
	 */
	virtual sc_iterator *create_input_arcs_iterator(sc_addr_ll addr) = 0;

	/**
	 * @return �������� �� �������� ����� ��� ��������.
	 */
	virtual sc_iterator *create_output_arcs_iterator(sc_addr_ll addr) = 0;

	/**
	 * @return �������� �� ���� ��������� ��������.
	 */
	virtual iterator *create_iterator_on_elements() = 0;


	/**
	 * @return ����� ����������� ��������.
	 */
	virtual Content get_content(sc_addr_ll addr) = 0;

	/**
	 * @return ������ �� ���������� ��������.
	 */
	virtual const Content *get_content_const(sc_addr_ll addr) = 0;

	/**
	 * ������������� ���������� ��������.
	 */
	virtual sc_retval set_content(sc_addr_ll, const Content &) = 0;

	/**
	 * ������� ����������� ��������.
	 */
	virtual sc_retval erase_content(sc_addr_ll) = 0;

	/// Find sc-elements by content in that sc-segment.
	/// @note See #sc_session.find_by_content for semantics.
	virtual bool find_by_content(const Content &content, ll_list &result) = 0;

	/**
	 * @return ������������� ��������. ���� �������������� ����, �� ���������� ����������.
	 */
	virtual sc_string get_idtf(sc_addr_ll) =0;

	/**
	 * ������������� ��� �������� ���������� �������������.
	 */
	virtual sc_retval set_idtf(sc_addr_ll, const sc_string &idtf) = 0;

	/**
	 * ������� ������������� ����������.
	 */
	virtual sc_retval erase_idtf(sc_addr_ll) = 0;

	/**
	 * ��������� ����� �� ������� �������������.
	 */
	virtual sc_retval has_erased_idtf(sc_addr_ll) = 0;

	/**
	 * ������� ������� �� ��������������. ���� ������ �������� ���, �� ���������� SCADDR_LL_NIL.
	 */
	virtual sc_addr_ll find_by_idtf(const sc_string &idtf) = 0;


	/**
	 * @return ���������� ��������� � ������� ���.
	 */
	virtual int get_out_qnt(sc_addr_ll ll) = 0;

	/**
	 * @return ���������� �������� � ������� ���.
	 */
	virtual int get_in_qnt(sc_addr_ll ll) = 0;

	/**
	 * @return ������ URI ������� ��������.
	 */
	virtual sc_string get_full_uri() = 0;
};

/// @}

#endif // __LIBSC_SC_SEGMENT_INCLUDED__
