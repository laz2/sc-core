
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

basic_sleep_marshaller::~basic_sleep_marshaller()
{
}

#define SLEEPING_CALLER_MAGIC 0xfeeddead

sleeping_caller::sleeping_caller(basic_sleep_marshaller *_m, sc_addr _p1, sc_addr _p2, sc_retval &_rv)
		: pm_process(0,0,pm_process::DETACHED)
		, magic(SLEEPING_CALLER_MAGIC)
		, marshaller(_m)
		, p1(_p1)
		, p2(_p2)
		, rv(_rv)
{
	caller = pm_sched_get_current();
	if (!caller)
		SC_ABORT_MESSAGE("sleeping_caller without active agent");
}

sleeping_caller::~sleeping_caller()
{
	if (caller)
		wake_caller();
}

void sleeping_caller::run()
{
	marshaller->marshall(p1,p2);
}

sleeping_caller *sleeping_caller::current()
{
	sleeping_caller *rv = (sleeping_caller *)pm_process::current();
	if (!rv || rv->magic != SLEEPING_CALLER_MAGIC) {
		SC_CRITICAL_MESSAGE("sleeping_caller::current failed");
		return 0;
	}
	return rv;
}

void sleeping_caller::wake_caller()
{
	pm_sched_wakeup(caller);
	caller = 0;
}

void sleeping_caller::do_return(sc_retval retval, bool do_not_wake)
{
	sleeping_caller *process = sleeping_caller::current();
	if (!process)
		return;
	if (!process->caller) {
		SC_CRITICAL_MESSAGE("sleeping_caller::do_return probably called twice");
		return;
	}
	process->rv = retval;
	if (!do_not_wake)
		process->wake_caller();
}

sc_retval sleeping_caller::call(basic_sleep_marshaller *m, sc_addr p1, sc_addr p2)
{
	sc_retval rv=RV_OK;
	new sleeping_caller(m,p1,p2,rv);
	pm_sched_put_to_sleep(pm_sched_get_current());
	context_switchback();
	return rv;
}

void __sleeping_advanced_activity::sleeping_return(sc_retval rv)
{
	sleeping_caller::do_return(rv);
}
