
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

#include "precompiled_p.h"

// TODO: add more error handling

struct address_less {
	bool operator ()(sc_smart_addr a,sc_smart_addr b)
	{
		return (sc_addr)a<(sc_addr)b;
	}
};

sc_generator_proxy::~sc_generator_proxy()
{
}

sc_generator_session_proxy::sc_generator_session_proxy(sc_session *_session)
	: sc_generator_proxy(_session)
{
}

sc_generator_session_proxy::~sc_generator_session_proxy()
{
}

sc_addr sc_generator_session_proxy::create_el(sc_segment *seg,sc_type type)
{
	return session->create_el(seg,type);
}

sc_retval sc_generator_session_proxy::set_beg(sc_addr arc,sc_addr beg)
{
	return session->set_beg(arc,beg);
}

sc_retval sc_generator_session_proxy::set_end(sc_addr arc,sc_addr end)
{
	return session->set_end(arc,end);
}

sc_retval sc_generator_session_proxy::erase_el(sc_addr el)
{
	return session->erase_el(el);
}

sc_retval sc_generator_session_proxy::set_idtf(sc_addr el,sc_string idtf)
{
	return session->set_idtf(el,idtf);
}

sc_retval sc_generator_session_proxy::set_content(sc_addr el,const Content &cont)
{
	return session->set_content(el,cont);
}

sc_retval sc_generator_session_proxy::change_type(sc_addr el,sc_type type)
{
	return session->change_type(el,type);
}

typedef std::vector<sc_smart_addr> smart_addr_vector;
typedef std::set<sc_smart_addr,address_less> smart_addr_set;
typedef std::vector<sc_type> type_vector;

struct attr_struct {
	sc_smart_addr addr;
	sc_type type;
	sc_segment *seg;
	
	attr_struct(sc_smart_addr _addr, sc_type _type, sc_segment *_seg) 
		: addr(_addr), type(_type), seg(_seg)
	{}
	// attr_struct(const attr_struct &str)
	// 	: addr(str.addr), type(str.type), seg(str.seg)
	// {}
	// attr_struct &operator =(const attr_struct &str)
	// {
	// 		addr = str.addr;
	// 		type = str.type;
	// 		seg = str.seg;
	// 		return *this;
	// 		}
};
typedef std::vector<attr_struct> attr_vector;

struct real_gen_state {
	smart_addr_vector elements;
	attr_vector attrs;
	smart_addr_vector prev_els;
	sc_smart_addr set;
	sc_type sent_arc_type;
	sc_segment *sent_seg;
	sc_smart_addr sys_sign;
	smart_addr_set system_set;
	sc_segment *system_segment;
	sc_type system_arc_type;
	smart_addr_vector tmp;
	bool system_context;
};

struct sc_generator_state : public std::vector<real_gen_state> {};

sc_generator::sc_generator(sc_session *_session,sc_segment *segment)
:	active_segment(segment),
	session(_session),
	state(new sc_generator_state),
	proxy(new sc_generator_session_proxy(session)),
	own_proxy(true),
	arc_type(SC_A_CONST|SC_POS),
	element_type(SC_N_CONST),
	set_arc_type(SC_A_CONST|SC_POS),
	system_arc_type(SC_A_CONST|SC_POS),
	attr_arc_type(SC_A_CONST|SC_POS)
{
	state->push_back(real_gen_state());
	(*(state->begin())).system_context = false;
}

sc_generator::sc_generator(sc_generator_proxy *_proxy,sc_segment *seg)
	: active_segment(seg),
	  session(_proxy->get_session()),
	  state(new sc_generator_state),
	  proxy(_proxy),
	  own_proxy(false),
	  arc_type(SC_A_CONST|SC_POS),
	  element_type(SC_N_CONST),
	  set_arc_type(SC_A_CONST|SC_POS),
	  system_arc_type(SC_A_CONST|SC_POS),
	  attr_arc_type(SC_A_CONST|SC_POS)
{
	state->push_back(real_gen_state());
	(*(state->begin())).system_context = false;
}

sc_generator::~sc_generator()
{
	leave_all();
	finish();
	if (own_proxy)
		delete proxy;
	delete state;
}

sc_retval sc_generator::set_idtf(sc_addr el,sc_string idtf)
{
	return proxy->set_idtf(el,idtf);
}

sc_retval sc_generator::set_content(sc_addr el,const Content &cont)
{
	return proxy->set_content(el,cont);
}

sc_retval sc_generator::change_type(sc_addr el,sc_type type)
{
	return proxy->change_type(el,type);
}

sc_addr sc_generator::generate_internal(const char *idtf,sc_type type,sc_segment *seg)
{
	if (!type)
		type = element_type;
	if (!seg)
		seg = active_segment;
	sc_addr el = proxy->create_el(seg,type);
	if (!idtf || !idtf[0])
		return el;
	sc_retval rv = proxy->set_idtf(el,idtf);
	if (rv) {
		proxy->erase_el(el);
		return 0;
	}
	return el;
}

sc_addr sc_generator::generate_find_internal(const char *idtf,sc_type type,sc_segment *seg)
{
	if (!seg)
		seg = active_segment;
	sc_addr el = (idtf && idtf[0])?session->find_by_idtf(idtf,seg):0;
	if (!el)
		return generate_internal(idtf,type,seg);
	if (type && type != session->get_type(el))
		return 0;
	return el;
}

#define GET_STATE() (*(state->end()-1))

void sc_generator::push_state(bool is_system)
{
	state->push_back(real_gen_state());
	GET_STATE().system_context = is_system;
}

sc_retval sc_generator::gen3_f_a_f(sc_addr from,sc_addr *parc,sc_segment *seg,sc_type type,sc_addr to)
{
	sc_addr arc = proxy->create_el(seg,type);
	if (!arc)
		return session->error;

	sc_retval rv = proxy->set_beg(arc,from) || proxy->set_end(arc,to);
	if (rv) {
		proxy->erase_el(arc);
		return rv;
	}

	if (parc)
		*parc = arc;
	return RV_OK;
}

sc_addr sc_generator::internal_gen_arc(sc_addr from,sc_addr to,sc_type arc_type,sc_segment *seg)
{
	real_gen_state &st = GET_STATE();
	attr_vector::iterator it = st.attrs.begin();
	sc_addr arc;

	if(gen3_f_a_f(from,&arc,seg,arc_type,to) != RV_OK)
		return 0;

	st.tmp.push_back(arc);
	for (;it<st.attrs.end();it++) {
		sc_addr arc4 = 0;
		const attr_struct &attr = *it;

		if (gen3_f_a_f(attr.addr,&arc4,attr.seg,attr.type,arc)) {
			proxy->erase_el(arc);
			return 0;
		}

		st.tmp.push_back(arc4);
	}
	return arc;
}

sc_addr sc_generator::element(sc_addr el)
{
	if (!el)
		return sc_generator::element("");
	return __element(el);
}

sc_addr sc_generator::__element(sc_addr el)
{
	real_gen_state &st = GET_STATE();
	st.tmp.push_back(el);

	if (!st.prev_els.empty()) {
		smart_addr_vector::iterator it = st.prev_els.begin();
		for (; it < st.prev_els.end(); ++it)
			internal_gen_arc(*it, el, st.sent_arc_type, st.sent_seg);
	}

	if (st.set) {
		sc_addr rv = internal_gen_arc(st.set, el, set_arc_type, active_segment);
		if (!rv)
			return 0;
	}

	if (st.sys_sign) {
		smart_addr_vector::iterator it = st.tmp.begin();
		for (; it < st.tmp.end(); ++it)
			st.system_set.insert(*it);
	}

	st.tmp.clear();
	st.attrs.clear();

	st.elements.push_back(el);

	return el;
}

sc_addr sc_generator::element(const char *idtf,sc_type type,sc_segment *segment)
{
	sc_addr el = generate_find_internal(idtf,type,segment);
	if (!el)
		return 0;
	return __element(el);
}

sc_addr sc_generator::attr(sc_addr el)
{
	real_gen_state &st = GET_STATE();
	attr_struct str (el,attr_arc_type,active_segment);
	st.attrs.push_back(str);
	if (st.sys_sign)
		st.system_set.insert(el);
	return el;
}

sc_addr sc_generator::attr(const char *idtf,sc_type type,sc_segment *segment)
{
	sc_addr el = generate_find_internal(idtf,type,segment);
	if (!el)
		return 0;
	return attr(el);
}

void sc_generator::arc()
{
	real_gen_state &st = GET_STATE();
	st.prev_els = st.elements;
	st.elements.clear();
	st.sent_arc_type = arc_type;
	st.sent_seg = active_segment;
}

void sc_generator::finish()
{
	GET_STATE().prev_els.clear();
	GET_STATE().elements.clear();
}

sc_addr sc_generator::enter_set(sc_addr a_set)
{
	element(a_set);
	push_state(false);
	real_gen_state &old_st = *(state->end()-2);
	real_gen_state &st = GET_STATE();
	st.sys_sign = old_st.sys_sign;
	st.system_set = old_st.system_set;
	old_st.system_set.clear();
	st.set = a_set;
	return a_set;
}

sc_addr sc_generator::enter_set(const char *idtf,sc_type type,sc_segment *segment)
{
	sc_addr el = generate_find_internal(idtf,type,segment);
	if (!el)
		return 0;
	return enter_set(el);
}

sc_addr sc_generator::leave_set()
{
	real_gen_state &st = GET_STATE();
	real_gen_state &old_st = *(state->end()-2);
	if (st.sys_sign)
		old_st.system_set = st.system_set;
	
	sc_addr set = st.set;
	state->pop_back();

	return set;
}

sc_addr sc_generator::enter_system(sc_addr sys_sign)
{
	element(sys_sign);
	push_state(true);

	real_gen_state &st = GET_STATE();
	st.sys_sign = sys_sign;
	st.system_segment = active_segment;
	st.system_arc_type = system_arc_type;

	return sys_sign;
}

sc_addr sc_generator::enter_system(const char *idtf,sc_type type,sc_segment *segment)
{
	sc_addr el = generate_find_internal(idtf,type,segment);
	if (!el)
		return 0;
	return enter_system(el);
}


sc_addr sc_generator::leave_system()
{
	real_gen_state &st = GET_STATE();
	real_gen_state &old_st = *(state->end()-2);
	smart_addr_set::iterator it = st.system_set.begin();
	sc_type at = st.system_arc_type;
	sc_segment *seg = st.system_segment;
	if (old_st.sys_sign)
		for (;it != st.system_set.end();it++) {
			sc_addr arc;
			gen3_f_a_f(st.sys_sign,&arc,seg,at,*it);
			old_st.system_set.insert(arc);
			old_st.system_set.insert(*it);
		}
	else
		for (;it != st.system_set.end();it++)
			gen3_f_a_f(st.sys_sign,0,seg,at,*it);

	sc_addr sys_sign = st.sys_sign;
	state->pop_back();

	return sys_sign;
}

// NOTE: may change arc's beg & end
sc_addr sc_generator::gen_arc(sc_addr from,sc_addr to,const char *idtf,sc_type type,sc_segment *segment)
{
	if (!type)
		type = arc_type;
	if (segment)
		segment = active_segment;
	sc_addr el = session->find_by_idtf(idtf,segment);
	if (el) {
		if (type != session->get_type(el))
			return 0;
	} else
		el = proxy->create_el(segment,type);
	proxy->set_beg(el,from);
	proxy->set_end(el,to);
	return element(el);
}

void sc_generator::leave_all()
{
	sc_generator_state::iterator it = state->end()-1;
	for (;it != state->begin();it--) {
		if ((*it).system_context)
			leave_system();
		else
			leave_set();
	}
}
