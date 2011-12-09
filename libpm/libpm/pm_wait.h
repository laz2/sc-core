
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


#ifndef __PM_WAIT_H__
#define __PM_WAIT_H__
#include <libsc/sc_smart_addr.h>
#include <set>
#include <vector>
//#include <scevent.h>

class pm_process;

// one process cannot be on queue twice
// BE CAREFUL
// Add here just before going to sleep,
//  AND do not cause any SC-level events
//    between adding to queue and going to sleep
// We will need to change this interface in MT version
class pm_wait_queue {
	typedef std::set<sc_smart_addr> entries_t;
	std::set<sc_smart_addr> entries;
public:
	pm_wait_queue() {}
	~pm_wait_queue() {}
	void add(pm_process *);
	void add(sc_addr);
	void remove(sc_addr);
	void remove(pm_process *);
	void wake_one();
	void wake_all();

	static void sleep_on(pm_wait_queue &);
};

class waitable_source {
protected:
	pm_wait_queue wait_queue;
public:
	waitable_source();
	virtual ~waitable_source();
	virtual bool poll() = 0;
	pm_wait_queue &get_wait_queue() {return wait_queue;}
	static void wait_for(waitable_source &ws);
};

class pm_select {
	typedef std::vector<waitable_source *> sources_t;
	sources_t sources;
	sources_t::iterator pos;
	bool in_sleep;
	void find_next_ready();
	struct busy_exception {};
public:
	pm_select();
	~pm_select();
	void add_source(waitable_source *);
	pm_select &operator <<(waitable_source *ws)
	{
		add_source(ws);
		return *this;
	}
	waitable_source *wait(); // timeout ?
	void reset_ready();
	bool has_next_ready();
	waitable_source *next_ready();
	// just alias for easier interface
	static void wait_for(waitable_source &ws) {waitable_source::wait_for(ws);}
};

// multiple inheritance gives better speed
class waitable_set : public waitable_source, public sc_wait {
	//sc_session *session;
	//smart_addr set;
public:
	waitable_set(sc_addr addr,sc_session *s);
	waitable_set(const char *str,sc_segment *seg,sc_type type,sc_session *s);
	~waitable_set();
	bool poll();
	sc_addr get(bool nonblocking = false);
	sc_addr peek(bool nonblocking = false,sc_addr *arc=0);
	bool activate(sc_wait_type type,sc_param *params,int len);
};

#endif //__PM_WAIT_H__
