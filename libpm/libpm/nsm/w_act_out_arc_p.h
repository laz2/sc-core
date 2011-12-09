
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



#ifndef __WAIT_ACTIVE_OUT_ARC_H__
#define __WAIT_ACTIVE_OUT_ARC_H__

#include "trace_p.h"
#include "act_on_act_p.h"
#include "act_on_gen_p.h"

struct wait_active_out_arc : public sc_wait 
{
	sc_addr activity_type;
	
    wait_active_out_arc() { activity_type = 0; };
	
    wait_active_out_arc(sc_addr a) : activity_type(a) {};
	
    ~wait_active_out_arc() {};

	bool activate(sc_wait_type type, sc_param *param, int len) 
	{
		std::list<Sact_goals_msg>::iterator it;

		switch (type) {
			case SC_WAIT_HACK_SET_MEMBER:
				if (nsm_session->get_type(param[2].addr) & SC_ARC) {
					if (!nsm_session->get_beg(param[2].addr) ||
						!nsm_session->get_end(param[2].addr)) 
					{
						SYSTRACE3 ("param[2]", param[2].addr, "\n");
						nsm_session->attach_wait(SC_WAIT_ARCS_ENDS, 
									&param[2], 1, new wait_active_out_arc(param[0].addr));
					} else {
						if (param[0].addr == NSM_SEARCH_) {
							activity_on_active_->active_goals.push_back(
											Sact_goals_msg(param[2].addr, 0));
						} else if (param[0].addr == NSM_GENERATE_) {
							act_on_gen_->active_goals.push_back(
											Sact_goals_msg(param[2].addr, 0));
						}

						pm_sched_wakeup(param[0].addr);
						nsm_session->attach_wait(SC_WAIT_DIE_PRE, &param[2], 1, new wait_active_out_arc);
					}
				} else {
					//TODO: вставить обработку ошибочной ситуации
				}
				break;

			case SC_WAIT_DIE_PRE:
				TRACE_ELEM_1("[NSM] SC_WAIT_DIE_PRE:", param[0].addr);
				it = activity_on_active_->active_goals.begin();
				for (; it!=activity_on_active_->active_goals.end(); it++)
					if (it->active_goal == param[0].addr) {
						activity_on_active_->active_goals.erase(it);
						break;
					}
				nsm_session->detach_wait(this);
				delete this;
				return true;
		}
		return false;
	}
};

#endif //__WAIT_ACTIVE_OUT_ARC_H__
