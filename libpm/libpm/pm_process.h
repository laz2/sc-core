
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


#ifndef __PM_PROCESS_H__
#define __PM_PROCESS_H__

#include <libsc/libsc.h>
#include <libsc/sc_smart_addr.h>

#include "pm_wait.h"

class process_activity_base : public sc_activity {
public:
	sc_retval init(sc_addr _this);
	void done();
	sc_retval activate(sc_session *,sc_addr _this,sc_addr param1,sc_addr,sc_addr) = 0;
	~process_activity_base();
};

// this class deletes self, be careful with multiple inheritance
class pm_process : public process_activity_base, public waitable_source {
public:
	enum proc_mode {
		NORMAL,
		DETACHED
	};
protected:
	sc_segment *autoseg;
	sc_session *session;
	sc_smart_addr sign;
	sc_smart_addr param;
private:
	proc_mode mode;
	bool completed;
	// methods of sc_activity
	sc_retval activate(sc_session *,sc_addr,sc_addr,sc_addr,sc_addr);
public:
	// son will get new session, all segments of parent process
	// EXCEPT it's autosegment.
	// If parent == 0 than process will get empty session
	// where only autosegment will be opened.
	// Sign of new process will be created in special
	// "processes_segment" segment
	// ??? Do we need to open param's segment by default ?
	// ??? I think this will be good if we implement recursive {open,close}_segment
	pm_process(pm_process *parent,sc_addr param,proc_mode mode=NORMAL);
	// autosegment and sign will be destroyed here
	virtual ~pm_process();
	// all process work is here.
	// process must periodically give processor back by calling yield().
	virtual void run() = 0;
	void yield();
	// detach/attach interface and mode are to allow parent to wait
	//  for sons completion
	// In detached mode process will just silently die
	// In normal mode process will become "zombie" (completed == true)
	// Detach in this situation will kill it
	void detach();
	// racy in real parallel world
	void undetach();
	proc_mode get_mode() {return mode;}
	bool is_completed() {return completed;}
	sc_addr get_sign() {return sign;}
	// to rise process from sleep
	void awake();

	// methods for waitable_source
	bool poll() {return is_completed();}
	sc_session *get_session() {return session;}


	static pm_process *current();
	static sc_session *current_session() {return current()->session;}
	static void sleep();
	static sc_segment *processes_segment;
	static pm_process *sign2process(sc_addr sign);
};

typedef void (*pm_run_function)(pm_process *self,sc_addr param,sc_segment *autoseg);

pm_process *process_new_func(pm_process *parent,pm_run_function func,sc_addr param);

#endif //__PM_PROCESS_H__
