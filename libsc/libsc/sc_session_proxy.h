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
 * @file sc_session_proxy.h
 * @brief Implementation of proxy #sc_session.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_SESSION_PROXY_INCLUDED__
#define __LIBSC_SC_SESSION_PROXY_INCLUDED__

/// @addtogroup libsc
/// @{

#include "libsc.h"

class LIBSC_API sc_session_proxy : public sc_session
{
protected:
	sc_session *host;

public:
	sc_session_proxy(sc_session *_host) : host(_host)
	{
	}

	~sc_session_proxy() {}

	sc_session *ref() { return host->ref(); }

	void unref() { host->unref(); };

	void close() { host->close(); }

	sc_retval get_error() { return host->get_error(); }

	sc_addr __move_element(sc_addr addr, sc_segment *to) { return host->__move_element(addr, to); }

	bool is_segment_opened(sc_segment *seg) { return host->is_segment_opened(seg); };

	sc_segment *create_segment(const sc_string &uri) { return host->create_segment(uri); }

	sc_segment *open_segment(const sc_string &uri) { return host->open_segment(uri); };

	sc_segment *open_segment_spider(const sc_string &uri, bool complete) { return host->open_segment_spider(uri, complete); }

	sc_retval _stat(const sc_string &uri) { return host->_stat(uri); }

	sc_retval close_segment(sc_segment *seg) { return host->close_segment(seg); }

	sc_retval close_segment(const sc_string &uri) { return host->close_segment(uri); }

	sc_retval unlink(const sc_string &uri) { return host->unlink(uri); }

	sc_retval rename(const sc_string &oldname, const sc_string &newname) { return host->rename(oldname, newname); }

	sc_retval mkdir(const sc_string &uri) { return host->mkdir(uri); }

	sc_retval chdir(const sc_string &uri) { return host->chdir(uri); }

	sc_dir_iterator *search_dir(const sc_string &uri) { return host->search_dir(uri); }

	sc_segment *find_segment(const sc_string &uri) { return host->find_segment(uri); }

	sc_addr get_end(sc_addr arc) { return host->get_end(arc); }

	sc_addr get_beg(sc_addr arc) { return host->get_beg(arc); }

	sc_retval set_beg(sc_addr arc, sc_addr beg) { return host->set_beg(arc, beg); }

	sc_retval set_end(sc_addr arc, sc_addr end) { return host->set_end(arc, end); }

	sc_addr get_beg_spin(sc_addr arc) { return host->get_beg_spin(arc); }

	sc_addr get_end_spin(sc_addr arc) { return host->get_end_spin(arc); }

	sc_iterator *create_iterator(sc_constraint *constr, bool sink) { return host->create_iterator(constr, sink); }

	sc_addr create_el(sc_segment *seg, sc_type type) { return host->create_el(seg, type); }

	sc_retval gen3_f_a_f(sc_addr e1, sc_addr *e2, sc_segment *seg2, sc_type t2, sc_addr e3)
	{
		return host->gen3_f_a_f(e1, e2, seg2, t2, e3);
	}

	sc_retval gen5_f_a_f_a_f(sc_addr e1, sc_addr *e2, sc_segment *seg2, sc_type t2, sc_addr e3, sc_addr *e4, sc_segment *seg4, sc_type t4, sc_addr e5)
	{
		return host->gen5_f_a_f_a_f(e1, e2, seg2, t2, e3, e4, seg4, t4, e5);
	}

	sc_retval attach_wait(sc_wait_type type, sc_param *params, int len, sc_wait *wait) { return host->attach_wait(type, params, len, wait); }

	sc_retval attach_good_wait(sc_wait_type type, sc_param *params, int len, sc_wait *wait) { return host->attach_good_wait(type, params, len, wait); }

	sc_retval detach_wait(sc_wait *wait) { return host->detach_wait(wait); }

	sc_type get_type(sc_addr el) { return host->get_type(el); }

	sc_retval change_type(sc_addr el, sc_type type) { return host->change_type(el, type); }

	sc_retval erase_el(sc_addr el) { return host->erase_el(el); }

	sc_string get_idtf(const sc_addr addr) { return host->get_idtf(addr); }

	sc_retval set_idtf(sc_addr addr, const sc_string &idtf) { return host->set_idtf(addr, idtf); }

	sc_retval erase_idtf(sc_addr addr) { return host->erase_idtf(addr); }

	sc_addr find_by_idtf(const sc_string &idtf, sc_segment *seg) { return host->find_by_idtf(idtf, seg); }

	Content get_content(sc_addr addr) { return host->get_content(addr); }

	const Content *get_content_const(sc_addr addr) { return host->get_content_const(addr); }

	sc_retval set_content(sc_addr addr, const Content &content) { return host->set_content(addr, content); }

	sc_retval erase_content(sc_addr addr) { return host->erase_content(addr); }

	bool find_by_content(const Content &content, addr_list &result) { return host->find_by_content(content, result); }

	int get_out_qnt(sc_addr addr) { return host->get_out_qnt(addr); }

	int get_in_qnt(sc_addr addr) { return host->get_in_qnt(addr); }

	sc_retval activate(sc_addr addr, sc_addr param, sc_addr prm2, sc_addr prm3) { return host->activate(addr, param, prm2, prm3); }

	sc_retval reimplement(sc_addr addr, sc_activity *new_activity) { return host->reimplement(addr, new_activity); }

	sc_addr uri2sign(const sc_string &uri) { return host->uri2sign(uri); }

	sc_string sign2uri(sc_addr addr) { return host->sign2uri(addr); }

	sc_session *__fork() { return host->__fork(); }

	void commit() { host->commit(); }

	void rollback() { host->rollback(); }

	bool erase_el_pre(sc_addr addr) { return host->erase_el_pre(addr); }

	sc_retval __erase_el(sc_addr addr) { return host->__erase_el(addr); }
};

/// @}

#endif // __LIBSC_SC_SESSION_PROXY_INCLUDED__
