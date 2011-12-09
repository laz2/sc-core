
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
 * @file pm_sched.h
 * @brief Interface of PM process scheduler.
 * @ingroup libpm
 **/

#ifndef __PM_SCHED_H__
#define __PM_SCHED_H__

/// @addtogroup libpm
/// {@

#include "pm.h"

#include <libsc/sc_smart_addr.h>

/// Init PM scheduling subsystem.
/// @param system System session for initialization.
/// @param catch_signals If true, then catch SIGINT and SIGTERM.
LIBPM_API sc_retval pm_sched_init(sc_session *system, bool catch_signals);

/// Return scheduling class of @p agent. Return 0 if @p agent isn't added to scheduling queue.
LIBPM_API sc_addr pm_sched_get_sched_class(sc_addr agent);

/// Add @p agent to scheduling queue with scheduling class @p sched_class.
LIBPM_API sc_retval pm_sched_add_agent(sc_addr agent, sc_addr sched_class);

/// Remove @p agent from scheduling queue.
LIBPM_API sc_retval pm_sched_remove_agent(sc_addr agent);

/// Remove @p agent from scheduling queue and put it to sleep. Remember scheduling class of @p agent.
LIBPM_API sc_retval pm_sched_put_to_sleep(sc_addr agent);

LIBPM_API sc_retval pm_sched_put_to_sleep_and_switch(sc_addr agent);

/// Return true if @p agent is sleeping.
LIBPM_API bool pm_sched_is_sleeping(sc_addr agent);

/// Wakeup @p agent and add it to scheduling queue. Restore scheduling class of @p agent.
LIBPM_API sc_retval pm_sched_wakeup(sc_addr agent);
LIBPM_API sc_retval pm_sched_set_wakeup_hook(sc_addr agent, sc_addr hook);
LIBPM_API sc_retval pm_sched_remove_wakeup_hook(sc_addr agent, sc_addr hook);
LIBPM_API void pm_sched_main();
LIBPM_API bool pm_sched_do_step();
LIBPM_API void pm_sched_quit();


LIBPM_API sc_addr pm_sched_get_current();

/// Wait for event and wakeup process.
class LIBPM_API sleep_until_event_wait : public sc_wait
{
public:
	sleep_until_event_wait(sc_session *_s, sc_addr _process,
		sc_wait_type _event, sc_addr _cond) : s(_s), process(_process), cond(_cond), event(_event) {}

	~sleep_until_event_wait();

	virtual bool activate(sc_wait_type type, sc_param *params, int len);
private:
	sc_smart_addr process;
	sc_wait_type event;
	sc_smart_addr cond;
	sc_session *s;
};

/// @}

#endif // __PM_SCHED_H__
