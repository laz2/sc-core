
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

#include <signal.h>

#ifndef WIN32
	#include <unistd.h>
#endif 

#if _MSC_VER >= 1400
	#include <Windows.h>
#endif 

typedef addr_list sched_list;

#undef SUBSCHED_KERNEL
#undef SUBSCHED_SYSTEM
#undef SUBSCHED_NORMAL

sched_list SUBSCHED_KERNEL;
sched_list SUBSCHED_SYSTEM;
sched_list SUBSCHED_NORMAL;

sc_context *default_context;

static sc_segment *sched_segment;

static int quit_flag = 0;

static void pm_sched_sighandler(int a) 
{
	quit_flag = 1;
}

sc_retval pm_sched_init(sc_session *system, bool catch_sig) 
{
	context_init();
	default_context = context_alloc();

	if (catch_sig) {
		if (signal(SIGINT, pm_sched_sighandler) == SIG_ERR) {
			SC_ABORT();
		}

		if (signal(SIGTERM, pm_sched_sighandler) == SIG_ERR) {
			SC_ABORT();
		}
	}

	sched_segment = system->create_segment("/proc/sched");
	system->gen3_f_a_f(SCHEDULING_CLASS, 0, sched_segment, SC_A_CONST|SC_POS, SCHED_CLASS_KERNEL);
	system->gen3_f_a_f(SCHEDULING_CLASS, 0, sched_segment, SC_A_CONST|SC_POS, SCHED_CLASS_SYSTEM);
	system->gen3_f_a_f(SCHEDULING_CLASS, 0, sched_segment, SC_A_CONST|SC_POS, SCHED_CLASS_NORMAL);

	return RV_OK;
}

sc_addr pm_sched_get_sched_class(sc_addr agent) 
{
	sc_addr sched_class = 0;
	search_3l2_f_cpa_cna_cpa_f(system_session, SCHEDULING_CLASS, 0, &sched_class, 0, agent);
	return sched_class;
}

sc_retval pm_sched_add_agent(sc_addr agent, sc_addr sched_class) 
{
	LOCK_EVENT();

	if (!agent->activity) {
		LOG_ERROR("Adding agent without activity");
		return RV_ERR_GEN;
	}

	if (!agent->activity->sched_added) {
		if (pm_sched_get_sched_class(agent)) {
			return RV_ERR_GEN;
		}

		sched_list *sub_sched;
		if (sched_class == SCHED_CLASS_KERNEL) {
			sub_sched = &SUBSCHED_KERNEL;
		} else if (sched_class == SCHED_CLASS_SYSTEM) {
			sub_sched = &SUBSCHED_SYSTEM;
		} else if (sched_class == SCHED_CLASS_NORMAL) {
			sub_sched = &SUBSCHED_NORMAL;
		} else {
			return RV_ERR_GEN;
		}

		sc_addr arc;
		if (system_session->gen3_f_a_f(sched_class, &arc, sched_segment, SC_A_CONST|SC_POS, agent)) {
			return RV_ERR_GEN;
		}

		sub_sched->push_back(agent);

		agent->activity->sched_added = true;
	}

	return RV_OK;
}

static sc_retval sched_remove_agent(sc_session *s, sc_addr agent, sc_addr &sched_class) 
{
	LOCK_EVENT();

	assert(agent->activity && "Somebody is adding agent without activity");

	if (!agent->activity->sched_added) {
		return RV_ERR_GEN;
	}

	sc_addr arc;
	if (search_3l2_f_cpa_cna_cpa_f(s, SCHEDULING_CLASS, 0, &sched_class, &arc, agent)) {
		SC_ABORT();
	}

	sched_list *sub_sched;
	if (sched_class == SCHED_CLASS_KERNEL) {
		sub_sched = &SUBSCHED_KERNEL;
	} else if (sched_class == SCHED_CLASS_SYSTEM) {
		sub_sched = &SUBSCHED_SYSTEM;
	} else if (sched_class == SCHED_CLASS_NORMAL) {
		sub_sched = &SUBSCHED_NORMAL;
	} else {
		return RV_ERR_GEN;
	}

	sub_sched->remove(agent);

	s->erase_el(arc);

	return RV_OK;
}

sc_retval pm_sched_remove_agent(sc_addr agent) {
	LOCK_EVENT();

	sc_transaction *session = new sc_transaction(system_session);

	sc_addr sched_class;
	if (sched_remove_agent(session, agent, sched_class)) {
		session->rollback();
		return RV_ERR_GEN;
	}

	session->commit();
	return RV_OK;
}

sc_retval pm_sched_put_to_sleep(sc_addr agent) {
	LOCK_EVENT();
	sc_addr sched_class;
	sc_addr tuple;
	sc_transaction *s = new sc_transaction(system_session);
	if (sched_remove_agent(s, agent, sched_class)) {
		goto __rollback;
	} 
	
	if (!(tuple = s->create_el(sched_segment, SC_N_CONST))) {
		goto __rollback;
	} 
	
	if (s->gen3_f_a_f(SLEEPING, 0, sched_segment, SC_A_CONST|SC_POS, tuple)) {
		goto __rollback;
	} 
	
	if (s->gen5_f_a_f_a_f(tuple, 0, sched_segment, SC_A_CONST|SC_POS, agent, 0, sched_segment, SC_A_CONST|SC_POS, N1_)) {
		goto __rollback;
	} 
	
	if (s->gen5_f_a_f_a_f(tuple, 0, sched_segment, SC_A_CONST|SC_POS, sched_class, 0, sched_segment, SC_A_CONST|SC_POS, N2_)) {
		goto __rollback;
	} 
	s->commit();
	agent->activity->sched_added = false;
	return RV_OK;
__rollback: 
	s->rollback();
	return RV_ERR_GEN;
}

bool pm_sched_is_sleeping(sc_addr agent)
{
	assert(agent);
	return sc_rel::bin_ord_at_2(system_session, SLEEPING, agent) != SCADDR_NIL;
}

sc_retval pm_sched_put_to_sleep_and_switch(sc_addr agent) {
	sc_retval rv = pm_sched_put_to_sleep(agent);
	if (rv) {
		return rv;
	}

	context_switchback();

	return RV_OK;
}

sc_retval pm_sched_wakeup(sc_addr agent) 
{
	LOCK_EVENT();
	sc_addr tuple;
	sc_addr sched_class;
	if (search_oneshot(system_session, sc_constraint_new(CONSTR_3l2_f_a_a_a_f, SLEEPING, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, agent), true, 1, 2, &tuple)) {
		sc_iterator *iter;
		bool flag = 0;
		sc_addr hook_tuple;
		
		if (search_oneshot(system_session, sc_constraint_new(CONSTR_3l2_f_a_a_a_f, WAKEUP_HOOK, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, agent), true, 1, 2, &hook_tuple)) {
			return RV_ERR_GEN;
		} 
		
		iter = system_session->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, hook_tuple, SC_A_CONST|SC_POS, 0, SC_A_CONST|SC_POS, N2_), true);
		
		for (; !iter->is_over(); iter->next()) {
			sc_addr hook = iter->value(2);
			if (!hook) {
				continue;
			} if (!system_session->activate(hook, agent)) {
				flag = 1;
			} 
		}
		delete iter;
		return flag ? RV_OK : RV_ERR_GEN;
	}
	
	if (search_5_f_cpa_cna_cpa_f(system_session, tuple, 0, &sched_class, 0, N2_)) {
		SC_ABORT();
	} 
	
	if (pm_sched_add_agent(agent, sched_class)) {
		return RV_ERR_GEN;
	} 
	
	system_session->erase_el(tuple);
	return RV_OK;
}

sc_retval pm_sched_set_wakeup_hook(sc_addr agent, sc_addr hook) {
	LOCK_EVENT();
	sc_addr tuple = 0;
	sc_transaction *s;
	search_oneshot(system_session, sc_constraint_new(CONSTR_3l2_f_a_a_a_f, WAKEUP_HOOK, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, agent), true, 1, 2, &tuple);
	s = new sc_transaction(system_session);
	if (!tuple) {
		tuple = s->create_el(sched_segment, SC_N_CONST);
		if (!tuple) {
			goto __rollback;
		} 
		
		if (s->gen3_f_a_f(WAKEUP_HOOK, 0, sched_segment, SC_A_CONST|SC_POS, tuple)) {
			goto __rollback;
		} 
		
		if (s->gen5_f_a_f_a_f(tuple, 0, sched_segment, SC_A_CONST|SC_POS, agent, 0, sched_segment, SC_A_CONST|SC_POS, N1_)) {
			goto __rollback;
		} 
	}
	
	if (s->gen5_f_a_f_a_f(tuple, 0, sched_segment, SC_A_CONST|SC_POS, hook, 0, sched_segment, SC_A_CONST|SC_POS, N2_)) {
		goto __rollback;
	} 
	
	s->commit();
	return RV_OK;
__rollback: 
	s->rollback();
	return RV_ERR_GEN;
}

sc_retval pm_sched_remove_wakeup_hook(sc_addr agent, sc_addr hook)
{
	LOCK_EVENT();
	sc_addr tuple, arc;
	if (search_oneshot(system_session, sc_constraint_new(CONSTR_3l2_f_a_a_a_f, WAKEUP_HOOK, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, agent), true, 1, &tuple)) {
		// TODO: ошибка, нет номера параметра.
		return RV_ERR_GEN;
	} 
	
	if (search_oneshot(system_session, sc_constraint_new(CONSTR_5_f_a_f_a_f, tuple, SC_A_CONST|SC_POS, hook, SC_A_CONST|SC_POS, N2_), true, 1, 1, &arc)) {
		return RV_ERR_GEN;
	} 
	
	if (system_session->erase_el(arc)) {
		return RV_ERR_GEN;
	} 
	
	if (search_5_f_cpa_cna_cpa_f(system_session, tuple, 0, 0, 0, N2_)) {
		return system_session->erase_el(tuple);
	} 
	
	return RV_OK;
}

static inline sched_list *get_next_turn(sched_list &addr)
{
	if (&addr ==  &SUBSCHED_KERNEL) {
		return  &SUBSCHED_SYSTEM;
	} else if (&addr ==  &SUBSCHED_SYSTEM) {
		return  &SUBSCHED_NORMAL;
	} else {
		return  &SUBSCHED_KERNEL;
	}
}

static sched_list *turn = &SUBSCHED_NORMAL;

sc_retval sched_queue_get(sched_list &q, sc_addr &a)
{
	if (q.empty()) {
		return RV_ELSE_GEN;
	}

	a = *(q.begin());
	q.pop_front();
	return RV_OK;
}

void pm_sched_yield() {
	#ifdef WIN32
		#if _MSC_VER >= 1400
			Sleep(1);
		#else 
			_sleep(1);
		#endif 
	#endif 
}

static void *activater(sc_addr agent)
{

	if (system_session->activate(agent)) {
		return (void*)2;
	} 
	return (void*)1;
}

static sc_addr current = 0;

sc_addr pm_sched_get_current() 
{
	return current;
}

sc_retval activate_agent(sc_addr agent)
{
	void *rv;
	if (!agent->activity) {
		return RV_OK;
	} 
	
	if (agent->activity->is_on_main_stack()) {
		return system_session->activate(agent);
	} 
	
	current = agent;
	
	if (agent->activity->ctx) {
		agent->ref();
		sc_context *ctx = (sc_context*)(agent->activity->ctx);

		rv = context_recall(ctx);
		if (rv) {
			context_free(ctx);
			if (!agent->dead) {
				agent->activity->ctx = 0;
			} 
		} else {
		}
		agent->unref();
		return RV_OK;
	}

	rv = context_call(default_context, (void *(*)(void*))activater, agent);
	if (!rv) {
		agent->activity->ctx = default_context;
		default_context = context_alloc();
		return RV_OK;
	}
	current = 0;
	return (rv == (void*)1) ? RV_OK : RV_ERR_GEN;
}

void pm_sched_main()
{
	while (!quit_flag || true) {
		turn = get_next_turn(*turn);

		sc_addr agent;
		if (!sched_queue_get(*turn, agent)) {
			turn->push_back(agent);

			if (activate_agent(agent)) {
				turn->remove(agent);
			}
		} else if (turn ==  &SUBSCHED_NORMAL) {
			if (quit_flag) {
				break;
			}

			pm_sched_yield();
		}
	}
}

sched_list next_run_list;

bool pm_sched_do_step() 
{
	sc_addr agent;

	if (quit_flag) {
		return false;
	}

	turn = get_next_turn(*turn);
	if (!sched_queue_get(*turn, agent)) {
		goto __quant;
	}
	if (turn ==  &SUBSCHED_NORMAL) {
		pm_sched_yield();
	}

	turn = get_next_turn(*turn);
	if (!sched_queue_get(*turn, agent)) {
		goto __quant;
	}
	if (turn ==  &SUBSCHED_NORMAL) {
		pm_sched_yield();
	}

	turn = get_next_turn(*turn);
	if (sched_queue_get(*turn, agent)) {
		//fprintf(stderr, "All scheduler queues are empty!\n");
		return false;
	}

	__quant: turn->push_back(agent);
	if (activate_agent(agent)) {
		fprintf(stderr, "pm_sched_do_step: agent activation resulted in error!\n");
		turn->remove(agent);
	}

	while (!next_run_list.empty()) {
		activate_agent(next_run_list.front());
		next_run_list.pop_front();
	}

	return true;
}

void pm_sched_this_next(sc_addr agent) 
{
	next_run_list.push_back(agent);
}

void pm_sched_quit() 
{
	quit_flag = 1;
}

sleep_until_event_wait::~sleep_until_event_wait()
{
	s->detach_wait(this);
}

bool sleep_until_event_wait::activate(sc_wait_type type, sc_param *params, int len)
{
	if (!process->is_dead() && !cond->is_dead()) {
		if (type == event) {
			for (int i = 0; i < len; ++i) {
				if (cond == params[i].addr) {
					pm_sched_wakeup(process);
					delete this;
				}
			}
		}
	} else {
		delete this;
	}

	return true;
}
