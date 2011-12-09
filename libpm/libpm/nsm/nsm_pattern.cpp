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
---
 */

/**
 * @file nsm_pattern.cpp
 * @brief Implementation of helper for working with NSM patterns.
 * @author Dmitry Lazurkin
 */

#include "libpm_precompiled_p.h"

void nsm_pattern::clean()
{
	if (results) {
		sc_iterator *results_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, results, 0, 0), true);
		sc_for_each (results_it) {
			sc_addr result = results_it->value(2);

			sc_iterator *pairs_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, result, 0, 0), true);
			sc_for_each(pairs_it) s->erase_el(pairs_it->value(2));
			
			s->erase_el(result);
		}

		s->erase_el(results);
		results = 0;
		goal = 0;
	}
}

sc_addr nsm_pattern::gen_goal(sc_addr pattern_sign, sc_addr goal_attr)
{
	s->open_segment("/proc/agents/nsm/keynode");

	// Generate search goal for NSM.
	sc_generator g(s, segment);
	g.element(NSM_GOALS);
	g.arc();
	goal = g.element("");
	g.arc();
	g.attr(goal_attr);
	g.attr(NSM_CONFIRM_);
	g.element(pattern_sign);
	g.finish();

	sc_addr arc_goal;
	search_3_f_cpa_f(s, goal, &arc_goal, pattern_sign);

	return arc_goal;
}

bool nsm_pattern::gen_goal_and_run(sc_addr pattern_sign, sc_addr goal_attr, sc_addr result_attr)
{
	sc_addr arc_goal = gen_goal(pattern_sign, goal_attr);

	// Sleep until NSM working.
	sc_addr process_agent = pm_sched_get_current();
	s->attach_wait(SC_WAIT_HACK_IN_SET, &SC_ADDR_AS_PARAM(arc_goal), 1,
		new sleep_until_event_wait(s, process_agent, SC_WAIT_HACK_IN_SET, result_attr));
	pm_sched_put_to_sleep_and_switch(process_agent);
	// Switch to another thread.
	// -------------------------

	results = 0;

	search_3l2_f_cpa_cna_cpa_f(s, NSM_RESULT, 0, &results, 0, arc_goal);
	
	s->erase_el(goal); // Erase NSM-goal structure.

	return results != 0;
}


/**
 * @brief Merge element @p main with @p other. Delete @p other.
 */
static sc_retval simple_merge_elements(sc_session* s, sc_addr main, sc_addr other)
{
	sc_iterator *it = system_session->create_iterator(sc_constraint_new(CONSTR_ALL_INPUT, other), true);
	sc_for_each (it) system_session->set_end(it->value(1), main);

	it = system_session->create_iterator(sc_constraint_new(CONSTR_ALL_OUTPUT, other), true);
	sc_for_each (it) system_session->set_beg(it->value(1), main);

	if (s->get_type(other) == s->get_type(main) && s->get_type(other) & SC_ARC) {
		if (!s->get_beg(main))
			s->set_beg(main, s->get_beg(other));
		if (!s->get_end(main))
			s->set_end(main, s->get_end(other));
	}

	s->erase_el(other);

	return RV_OK;
}

void nsm_pattern_with_params::cb_copy_pattern(sc_addr original, sc_addr copy, void *cb_data)
{
	nsm_pattern_with_params *context = (nsm_pattern_with_params *) cb_data;
	system_session->gen3_f_a_f(context->sign_with_params, 0, copy->seg, SC_A_CONST, copy);
	context->original2copy.insert(addr2addr_map::value_type(original, copy));
	context->copy2original.insert(addr2addr_map::value_type(copy, original));
}

bool nsm_pattern_with_params::search()
{
	clean_if_need();
	
	substitute_params();

	bool rv = gen_goal_and_run(sign_with_params, NSM_SEARCH_, NSM_SEARCHED_);
	if (rv)
		substitute_in_results();

	return rv;
}

bool nsm_pattern_with_params::gen()
{
	clean_if_need();

	substitute_params();
	
	bool rv = gen_goal_and_run(sign_with_params, NSM_GENERATE_, NSM_GENERATED_);
	if (rv)
		substitute_in_results();

	return rv;
}

void nsm_pattern_with_params::attach_substitute_in_results_waiter(sc_addr arc_goal, sc_addr result_attr)
{
	sc_param param[2];
	param[0].addr = arc_goal;
	s->attach_wait(SC_WAIT_HACK_IN_SET, param, 1,
		new substitute_in_results_waiter(s, *this, result_attr));
}

sc_addr nsm_pattern_with_params::gen_goal_for_async(sc_addr goal_type, sc_addr result_attr)
{
	clean_if_need();

	substitute_params();

	sc_addr arc_goal = gen_goal(sign_with_params, goal_type);
	attach_substitute_in_results_waiter(arc_goal, result_attr);

	return arc_goal;
}

void nsm_pattern_with_params::clean()
{
	nsm_pattern::clean();

	if (sign_with_params) {
		s->erase_el(sign_with_params);
		sign_with_params = 0;
		original2copy = addr2addr_map();
		original2copy = addr2addr_map();
	}
}

sc_addr nsm_pattern_with_params::substitute_params()
{
	LOCK_EVENT(); // Block event handlers during pattern coping.
	
	addr_set var_els;
	addr_set const_els;

	// Collect vars/metavars and consts pattern elements in two lists.
	sc_iterator* it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, sign, 0, 0), true);
	sc_for_each (it) {
		sc_addr el = it->value(2);

		if (s->get_type(el) & (SC_VAR|SC_METAVAR)) {
			var_els.insert(el);
		} else {
			const_els.insert(el);
		}
	}

	if (var_els.empty()) {
		// TODO: Handle when pattern does not have vars.
	}

	//
	// Copy pattern in tmp segment. Do not copy constants.
	//
	segment_for_copy = create_unique_segment(s, "/tmp");
	sign_with_params = s->create_el(segment_for_copy, SC_N_CONST);
	sc_set::copy(s, segment_for_copy, var_els, cb_copy_pattern, this);
	addr_set::iterator it_consts = const_els.begin();
	for (; it_consts != const_els.end(); it_consts++)
		s->gen3_f_a_f(sign_with_params, 0, segment_for_copy, SC_A_CONST, *it_consts);

	//
	// Substitute params in pattern copy.
	//
	addr_set for_erase;
	params_map::iterator params_it = params.begin();
	for (; params_it != params.end(); params_it++) {
		sc_addr param = params_it->first;

		if (search_3_f_cpa_f(s, sign, 0, param) == RV_OK) {
			sc_addr val = params_it->second;

			addr2addr_map::iterator copies_it = original2copy.find(param);

			sc_addr param_copy = copies_it->second; 
			simple_merge_elements(s, val, param_copy);
			original2copy.erase(copies_it);
			original2copy.insert(addr2addr_map::value_type(param, val));
			
			addr2addr_map::iterator originals_it = copy2original.find(param_copy);
			copy2original.erase(originals_it);
			copy2original.insert(addr2addr_map::value_type(val, param));
		} else {
			for_erase.insert(param);
		}
	}

	for (addr_set::iterator it = for_erase.begin(); 
		it != for_erase.end(); it++)
		params.erase(*it);

	return sign_with_params;
}


void nsm_pattern_with_params::substitute_in_results()
{
	// Iterate over search results.
	sc_iterator *search_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, results, SC_A_CONST|SC_POS, 0), true);
	sc_for_each (search_it) {
		sc_addr result = search_it->value(2);

		// Iterate over pairs "pattern element - searched value" in current search result.
		sc_iterator *conn_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, result, SC_A_CONST|SC_POS, 0), true);
		sc_for_each (conn_it) {
			sc_addr conn = conn_it->value(2);

			sc_addr copy_arc = 0;
			sc_addr copy = sc_tup::at(s, conn, N1_, &copy_arc);

			addr2addr_map::iterator c2o_it = copy2original.find(copy);
			// TODO: assert(c2o_it != copy2original.end());
			if (c2o_it != copy2original.end()) {
				s->set_end(copy_arc, c2o_it->second);
				s->erase_el(copy);
			}
		}
	}
}