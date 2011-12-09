
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
 * @file nsm.cpp
 * @author Roman Serdyukov
 */

#include "libpm_precompiled_p.h"

#include "w_act_out_arc_p.h"
#include "act_on_act_p.h"

sc_session *nsm_session;
sc_segment *nsm_segment;
sc_segment *nsm_keynodes_segment;
static  sc_segment *nsm_mailseg;

sc_retval nsm_isom_init()
{
	nsm_session->open_segment("/home/nsm/config");

	sc_param param[1];
	param[0].addr = NSM_SEARCH_;
	nsm_session->attach_wait(SC_WAIT_HACK_SET_MEMBER, param, 1, new wait_active_out_arc());
	param->addr = NSM_GENERATE_;
	nsm_session->attach_wait(SC_WAIT_HACK_SET_MEMBER, param, 1, new wait_active_out_arc());
	nsm_session->attach_wait(SC_WAIT_DIE_PRE, param, 1, new wait_active_out_arc());

	activity_on_active_ = new Sact_on_active_;
	act_on_gen_ = new Sact_on_gen_;
	activity_on_confirm_ = new act_on_confirm_;

#ifdef NSM_DONT_USE_SCHEDULER
	activity_on_active_->init(0);
	act_on_gen_->init(0);
	activity_on_confirm_->init(0);
#else
	if (nsm_session->reimplement(NSM_SEARCH_, activity_on_active_)) {
		SC_ABORT();
	}

	if (nsm_session->reimplement(NSM_GENERATE_, act_on_gen_)) {
		SC_ABORT();
	}

	if (pm_sched_add_agent(NSM_SEARCH_, SCHED_CLASS_NORMAL)) {
		return 7;
	}

	if (pm_sched_add_agent(NSM_GENERATE_, SCHED_CLASS_NORMAL)) {
		return 7;
	}

	if (nsm_session->reimplement(NSM_CONFIRM_, activity_on_confirm_)) {
		SC_ABORT();
	}

	if (pm_sched_add_agent(NSM_CONFIRM_, SCHED_CLASS_NORMAL)) {
		return 7;
	}
#endif // NSM_DONT_USE_SCHEDULER

	return RV_OK;
}

void nsm_pre_init()
{
	sc_session *system = system_session;

	nsm_session = system;
	nsm_session->mkdir("/home");
	nsm_session->mkdir("/home/nsm");
	nsm_session->create_segment("/home/nsm/profile");

	system->mkdir("/proc/agents/nsm");
	nsm_segment = system->create_segment("/proc/agents/nsm/tmp");
	if (!nsm_segment) {
		return;
	}

	nsm_keynodes_segment = system->create_segment("/proc/agents/nsm/keynode");
	if (!nsm_keynodes_segment) {
		return;
	}

	for (size_t i=0;i<sizeof(nsm_keynodes)/sizeof(sc_addr);i++) {
		if ((nsm_keynodes[i] = create_keynode(nsm_session,nsm_idtfs[i]))) {
			//				TRACE_ELEM_1("[NSM] Created keynode: ", nsm_keynodes[i])
		} else {
		}
	}
}

void nsm_init()
{
	nsm_isom_init();
}
