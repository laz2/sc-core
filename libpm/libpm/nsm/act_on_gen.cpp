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

#include "act_on_gen_p.h"
#include "trace_p.h"

sc_retval Sact_on_gen_::activate(sc_session *,sc_addr _this,sc_addr param1,sc_addr __prm2,sc_addr __prm3) 
{
	if (!active_goals.empty()) {
		goal_list::iterator it = active_goals.begin();
		
		sc_addr goal = nsm_session->get_beg(it->active_goal);

		if (sc_set::is_in(nsm_session, goal, NSM_GOALS)) {
			//
			// This is NSM goal, check it 
			//

			//
			// Find and unpack parameters relation.
			//
			sc_addr params_rel = sc_tup::at(nsm_session, goal, NSM_PARAMETERS_);
			addr2addr_map params;
			if (params_rel)
				sc_rel::unpack(nsm_session, params_rel, params);

			sc_addr tmp;
			if (search_3_f_cpa_f(nsm_session, NSM_CONFIRM_, &tmp, it->active_goal) == RV_OK) {
				//nsm_session->erase_el(tmp); // удаляем дугу из confirm_.??

				do_confirm_(it->active_goal, params);
				gen3_f_cpa_f(nsm_session, it->active_goal->seg, NSM_GENERATED_, 0, it->active_goal);
				active_goals.erase(it); // do_confirm_ for one time unit
			} else 	if (search_3_f_cpa_f(nsm_session, NSM_DENY_, &tmp, it->active_goal) == RV_OK) {
				//nsm_session->erase_el(tmp); // удаляем дугу из deny_.

				for (int steps_count = 0; steps_count < ISOM_STEPS_PERQUANT; steps_count++) {
					if (do_deny_(it->active_goal)) {
						nsm_session->gen3_f_a_f(NSM_GENERATED_, 
											&tmp, it->active_goal->seg, SC_A_CONST,
											it->active_goal);
						active_goals.erase(it);
						break;
					}
				}
			} else {
				TRACE_ERROR("Activity on generate_: deny_ or confirm_ requer");
			}
		} else {
			active_goals.erase(it);
		}
	}

#ifndef NSM_DONT_USE_SCHEDULER
	if (active_goals.empty())
		pm_sched_put_to_sleep(NSM_GENERATE_);
#endif // NSM_DONT_USE_SCHEDULER

	return RV_OK;
}

void Sact_on_gen_::do_confirm_(sc_addr arc_goal, const addr2addr_map &params, addr_list *return_list)
{
	addr2addr_map gen_corr;  // Pattern to gened element corresponding
	addr_list not_gened;     // Yet not gened pattern elements

	sc_addr pattern = nsm_session->get_end(arc_goal); // Pattern for generating
	sc_segment *result_seg = pattern->seg;            // Segment for generating result

	#define DENY_DUPLICATED_PATTERN_EL(el, container) \
		if (contains(container, el)) { \
			std::cerr << "[NSM::GEN] In pattern " << SC_QURI(pattern) << " duplicated pattern element " \
				<< SC_QURI(el) << "." << std::endl; \
			return; \
		}

	//
	// Read pattern elements and generate non-arcs elements.
	// Collect arcs in not_gened.
	//
	sc_iterator *iter = nsm_session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, pattern, 0, 0), true);
	sc_for_each(iter) {
		sc_addr pattern_el = iter->value(2);
		sc_type pattern_type = nsm_session->get_type(pattern_el);

		if (pattern_type & SC_CONST) {
			// Constant corresponds itself
			DENY_DUPLICATED_PATTERN_EL(pattern_el, gen_corr);
			gen_corr.insert(addr2addr_map::value_type(pattern_el, pattern_el));
		} else {
			//
			// Pattern element isn't constant.
			// Check may be it has value from @c params.
			//
			addr2addr_map::const_iterator prm_it = params.find(pattern_el);
			if (prm_it != params.end()) {
				// Yep, yet pattern element corresponds parameter value
				sc_addr prm = prm_it->second;

				SC_ASSERTF(get_corresponding_type(pattern_type) & nsm_session->get_type(prm),
					"Parameter type of %s isn't correspond pattern type of %s.", SC_SHORTIc(prm), SC_SHORTIc(pattern_el));

				DENY_DUPLICATED_PATTERN_EL(pattern_el, gen_corr);
				gen_corr.insert(addr2addr_map::value_type(pattern_el, prm));
			} else {
				//
				// We need generate element for pattern element.
				// Here generate non-arcs elements.
				//
				if (!(pattern_type & SC_ARC)) {
					// Generate SC_NODE or SC_UNDF
					sc_type corr_type = get_corresponding_type(pattern_type);
					sc_addr gened_el = nsm_session->create_el(result_seg, corr_type);

					DENY_DUPLICATED_PATTERN_EL(pattern_el, gen_corr);
					gen_corr.insert(addr2addr_map::value_type(pattern_el, gened_el));
				} else {
					DENY_DUPLICATED_PATTERN_EL(pattern_el, not_gened);
					not_gened.push_back(pattern_el);
				}
			}
		}
	}

	//
	// Generate for all pattern arcs from not_gened.
	//
	while (!not_gened.empty()) {
		addr_list::iterator not_gened_it = not_gened.begin();
		while (not_gened_it != not_gened.end()) {
			sc_addr pattern_el = *not_gened_it;
			sc_type pattern_type = nsm_session->get_type(pattern_el);

			SC_ASSERTF(pattern_type & SC_ARC, "Non-arc element %s in not_gened", SC_SHORTIc(pattern_el));

			sc_addr pattern_beg = nsm_session->get_beg(pattern_el);
			sc_addr pattern_end = nsm_session->get_end(pattern_el);

			sc_addr gened_beg = gen_corr[pattern_beg];
			sc_addr gened_end = gen_corr[pattern_end];

			// If begin and end not gened yet then suspend arc generation
			if (gened_beg && gened_end) {
				sc_type corr_type  = get_corresponding_type(pattern_type);
				sc_type const_type = corr_type & SC_CONSTANCY_MASK;

				if (!(pattern_type & SC_FUZ)) {
					// For generating SC_POS arc: erase all SC_NEG|SC_CONST arcs between gened begin and end
					// For generating SC_NEG arc: erase all SC_POS|SC_CONST arcs between gened begin and end
					sc_type denied_type = (pattern_type & SC_POS) ? SC_NEG : SC_POS;

					sc_iterator *iter = nsm_session->create_iterator(
						sc_constraint_new(CONSTR_3_f_a_f, gened_beg, denied_type|SC_ARC_ACCESSORY|SC_CONST, gened_end), true);
					sc_for_each(iter) nsm_session->erase_el(iter->value(1));
				}

				sc_addr gened_arc = nsm_session->create_el(result_seg, corr_type);
				nsm_session->set_beg(gened_arc, gened_beg);
				nsm_session->set_end(gened_arc, gened_end);

				gen_corr[pattern_el] = gened_arc;

				not_gened_it = not_gened.erase(not_gened_it);
			} else {
				++not_gened_it;
			}
		}
	}

	//
	// Create NSM results for generating.
	//
	sc_generator g(nsm_session, result_seg);
	g.element(NSM_RESULT);
	g.arc();
	g.enter_set("");
	g.element(arc_goal);
		g.enter_set("");
			for(addr2addr_map::iterator corr_iter = gen_corr.begin(); corr_iter != gen_corr.end(); ++corr_iter) {
				g.enter_set("");
					g.attr(N1_);
					g.element(corr_iter->first);
					g.attr(N2_);
					g.element(corr_iter->second);
				g.leave_set();

				if (return_list)
					return_list->push_back(corr_iter->second);
			}
		g.leave_set();
	g.leave_set();
	g.finish();
}

bool Sact_on_gen_::do_deny_(sc_addr arc_goal, LISTSC_ADDR *return_list)
{
	sc_addr query = nsm_session->get_end(arc_goal);

	addr2query_map::iterator it = deny_queries.find(query);
	ListIsomQuery::iterator liq;
	if (it == deny_queries.end()) {
		addr2query_map::iterator query_it = deny_queries.insert(
			addr2query_map::value_type(query, ListIsomQuery())).first;

		addr2addr_map replace_map;
		if (!return_list) {
			LISTSC_ADDR rt;
			act_on_confirm_::add_query(query_it->second, arc_goal, 0, 0, 0, rt, replace_map, true);
		} else {
			act_on_confirm_::add_query(query_it->second, arc_goal, 0, 0, 0, *return_list, replace_map, true);
		}

		return false;
	} else {
		liq = (it->second).begin();
		if (act_on_confirm_::isom_do_step(*liq, liq->variants,
					liq->variants.begin(), liq->result, false))
			return false;
	}

	// Delete variant and gen neg/pos arcs between two const elems
	std::list<MAPSC_ADDR>::iterator res_it = liq->result.begin();
	MAPSC_ADDR::iterator map_it;
	for (; res_it!=liq->result.end(); res_it++)
	{
		map_it = res_it->begin();
		for (;map_it!=res_it->end(); map_it++) {
			if (map_it->first != map_it->second) {
				R_SYSTRACE("[NSM] generate_ deny_: erase elem:");
				R_TRACE_ELEM(map_it->second);
				R_SYSTRACE("\n");
				nsm_session->erase_el(map_it->second);
			}
		}
	}

	sc_iterator *iter = nsm_session->create_iterator(sc_constraint_new(
			CONSTR_5_a_a_a_a_f, SC_CONST, 0, SC_CONST, 0, query),true);
	for (;!iter->is_over();iter->next()) {
		sc_addr tmp;
		sc_type ArcType = nsm_session->get_type(iter->value(1))&SC_POS ?
								SC_NEG|SC_ARC_ACCESSORY|SC_CONST : SC_POS|SC_ARC_ACCESSORY|SC_CONST;
		nsm_session->gen3_f_a_f(iter->value(0), &tmp, query->seg, ArcType, iter->value(2));
		if (return_list)
			return_list->push_back(tmp);
		R_SYSTRACE("[NSM] generate_ deny_ : create el:");
		R_TRACE_ELEM(tmp);
		R_SYSTRACE("\n");
	}
	delete iter;

	R_SYSTRACE("[NSM] generate_ deny_ "); R_TRACE_ELEM(query); R_SYSTRACE(" Ok\n");
	deny_queries.erase(it);
	return true;
}
