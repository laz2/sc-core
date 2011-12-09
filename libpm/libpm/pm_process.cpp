
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

sc_segment *pm_process::processes_segment;
pm_process *current_process;

sc_retval process_activity_base::init(sc_addr _this)
{
	return RV_OK;
}

void	process_activity_base::done()
{
}

process_activity_base::~process_activity_base()
{
}

void	init_pm_process()
{
	if (pm_process::processes_segment)
		return;
	sc_session *s = libsc_login();
	pm_process::processes_segment = s->create_segment("/proc/processes");
	s->close();
}

pm_process::pm_process(pm_process *parent,sc_addr _param,proc_mode _mode)
:
	param(_param),
	mode(_mode),
	completed(false)
{
	if (!parent)
		session = libsc_login();
	else {
		session = parent->session->__fork();
		session->close_segment(parent->autoseg);
	}
	if (!session)
		SC_ABORT();
	init_pm_process();
	session->open_segment(processes_segment->get_full_uri());
	sign = session->create_el(processes_segment,SC_N_CONST);
	autoseg = create_unique_segment(session,"/tmp/pm_proc_auto");
	if (!autoseg)
		SC_ABORT();
	session->reimplement(sign,this);
	pm_sched_add_agent(sign,SCHED_CLASS_NORMAL);
}

pm_process::~pm_process()
{
	pm_sched_remove_agent(sign);
	session->reimplement(sign,0);
	session->erase_el(sign);
	session->unlink(autoseg->get_full_uri());
	session->close();
}

pm_process *pm_process::sign2process(sc_addr sign)
{
	sc_activity *a = sign->activity;
	if (!a)
		return 0;
	return (pm_process *)a;
}

sc_retval pm_process::activate(sc_session *s,sc_addr _this,sc_addr param1,sc_addr prm2,sc_addr prm3)
{
	current_process = this;
	run();
	current_process = 0;
	if (mode == DETACHED) {
		delete this;
		return RV_OK;
	}
	// signal that we have completed
	wait_queue.wake_all();
	completed = true;
	pm_sched_remove_agent(sign);
	return RV_OK;
}

void	pm_process::yield()
{
	current_process = 0;
	context_switchback();
	current_process = this;
}

pm_process *pm_process::current()
{
	return current_process;
}

void	pm_process::sleep()
{
	current_process = 0;
	pm_sched_put_to_sleep(pm_sched_get_current());
	context_switchback();
	current_process = sign2process(pm_sched_get_current());
}

void	pm_process::detach()
{
	mode = DETACHED;
	if (completed)
		delete this;
}

void	pm_process::undetach()
{
	if (completed)
		return;
	mode = NORMAL;
}

void	pm_process::awake()
{
	pm_sched_wakeup(sign);
}


// implementation of process_new_func
class func_process : public pm_process {
	pm_run_function func;
public:
	func_process(pm_process *parent,pm_run_function _func,sc_addr prm)
		: pm_process(parent,prm), func(_func)
	{
	}
	void run()
	{
		func(this,param,autoseg);
	}
};

pm_process *process_new_func(pm_process *parent,pm_run_function func,sc_addr prm)
{
	return new func_process(parent,func,prm);
}


