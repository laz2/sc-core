
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

#include "libpm_precompiled_p.h"

void pm_wait_queue::add(pm_process *process)
{
	add(process->get_sign());
}

void pm_wait_queue::add(sc_addr sign)
{
	entries.insert(sign);
}

void pm_wait_queue::remove(pm_process *process)
{
	remove(process->get_sign());
}

void pm_wait_queue::remove(sc_addr sign)
{
	entries.erase(sign);
}

void pm_wait_queue::wake_one()
{
	entries_t::iterator it = entries.begin();
	if (it == entries.end())
		return;
	pm_process::sign2process(*it)->awake();
}

void pm_wait_queue::wake_all()
{
	entries_t::iterator it = entries.begin();
	for (;it != entries.end();it++)
		pm_process::sign2process(*it)->awake();
}

void pm_wait_queue::sleep_on(pm_wait_queue &wq)
{
	wq.add(pm_process::current());
	pm_process::sleep();
	wq.remove(pm_process::current());
}

waitable_source::waitable_source()
{
}

waitable_source::~waitable_source()
{
}

void waitable_source::wait_for(waitable_source &ws)
{
	do {
		pm_wait_queue::sleep_on(ws.get_wait_queue());
	} while (!ws.poll());
}

pm_select::pm_select()
: pos(sources.end()), in_sleep(false)
{
}

pm_select::~pm_select()
{
	if (in_sleep)
		throw busy_exception();
}

void pm_select::add_source(waitable_source *ws)
{
	if (in_sleep)
		throw busy_exception();
	sources.push_back(ws);
}

void pm_select::find_next_ready()
{
	for (;pos != sources.end();pos++)
		if ((*pos)->poll())
			return;
}

void pm_select::reset_ready()
{
	if (in_sleep)
		throw busy_exception();
	pos = sources.begin();
	find_next_ready();
}

waitable_source *pm_select::next_ready()
{
	waitable_source *rv;
	if (in_sleep)
		throw busy_exception();
	if (pos == sources.end())
		return 0;
	rv = *pos++;
	find_next_ready();
	return rv;
}

bool pm_select::has_next_ready()
{
	return pos != sources.end();
}

waitable_source *pm_select::wait()
{
	if (in_sleep)
		throw busy_exception();
	if (!sources.size())
		return 0;
	reset_ready();
	if (has_next_ready())
		return 0;
	pm_process *ps = pm_process::current();
	// add process to waitqueues
	pos = sources.begin();
	for (;pos != sources.end();pos++)
		(*pos)->get_wait_queue().add(ps);
	// go to sleep
	do {
		in_sleep = true;
		pm_process::sleep();
		in_sleep = false;
		// check readyness
		reset_ready();
	} while (pos == sources.end());
	sources_t::iterator it = sources.begin();
	for (;it != sources.end();it++)
		(*it)->get_wait_queue().remove(ps);
	return *pos;
}

waitable_set::waitable_set(sc_addr _set,sc_session *s)
{
	sc_wait::session = s;
	sc_wait::element = _set;
	sc_param prm[1];
	prm[0].addr = element;
	s->attach_good_wait(SC_WAIT_HACK_SET_MEMBER,prm,1,this);
}

waitable_set::waitable_set(
	const char *str,
	sc_segment *seg,
	sc_type type,
	sc_session *s
) {
	sc_wait::session = s;
	sc_addr el = s->create_el(seg,type);
	if (!el)
		SC_ABORT();
	if (str && str[0]) {
		s->set_idtf(el,str);
		if (s->error)
			SC_ABORT();
	}
	sc_wait::element = el;
	sc_param prm[1];
	prm[0].addr = el;
	s->attach_good_wait(SC_WAIT_HACK_SET_MEMBER,prm,1,this);
}


waitable_set::~waitable_set()
{
	session->detach_wait(this);
}

bool	waitable_set::poll()
{
	return !search_oneshot(session,sc_constraint_new(CONSTR_3_f_a_a,
				sc_addr(element),
				0,
				0),true,0);
}

bool	waitable_set::activate(sc_wait_type wt,sc_param *prm,int len)
{
	wait_queue.wake_all();
	return false;
}

sc_addr waitable_set::peek(bool nonblocking,sc_addr *_arc)
{
	sc_addr arc;
	sc_addr elem;
	sc_retval rv;
again_:
	rv = search_oneshot(session,sc_constraint_new(CONSTR_3_f_a_a,
				sc_addr(element),
				0,
				0),true,2,1,&arc,2,&elem);
	if (rv) {
		if (nonblocking)
			return 0;
		else {
			pm_wait_queue::sleep_on(wait_queue);
			goto again_;
		}
	}
	if (_arc)
		*_arc = arc;
	return elem;
}

sc_addr	waitable_set::get(bool nonblocking)
{
	sc_addr arc;
	sc_addr rv = peek(nonblocking,&arc);
	if (rv)
		session->erase_el(arc);
	return rv;
}
