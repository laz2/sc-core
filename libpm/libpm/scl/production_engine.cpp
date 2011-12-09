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
 * @file production_engine.cpp
 * @brief Implementation of production engine subsystem.
 * @author Dmitry Lazurkin
 */

#include "libpm_precompiled_p.h"

using namespace scl;

sc_session *session;
ScOperation *productions_op;

sc_retval scl::init_productions_engine(sc_session *s) // FIXME: Why i must specify SCL explicit?
{
	session = s;

	scl_keynodes_segment = session->create_segment("/proc/scl/keynode");
	create_keynodes(s, scl_keynodes_count, scl_keynodes_uris, scl_keynodes);

	productions_op = new productions_operation(session);
	productions_op->registerOperation();

	return RV_OK;
}

productions_activity::productions_activity(sc_session *_s, sc_segment *_seg, sc_addr _productions) : s(_s), segment(_seg), 
	count_not_applied(0), cur_index(0)
{
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, _productions, SC_A_CONST|SC_POS, 0), true);
	sc_for_each(it) productions.push_back(it->value(2));
}

productions_activity::~productions_activity()
{
	pm_sched_remove_agent(get_sign());
	s->unlink(segment->get_full_uri());
}

sc_retval productions_activity::activate(sc_session *s, sc_addr _this, sc_addr prm1, sc_addr prm2, sc_addr prm3)
{
	if (cur_index == productions.size()) {
		if (count_not_applied == productions.size()) {
			delete this;
			return RV_OK;
		}

		cur_index = 0;
	}

	sc_addr production = productions[cur_index];
	
	sc_addr if_part = sc_tup::at(s, production, IMPL_IF_);
	sc_addr then_part = sc_tup::at(s, production, IMPL_THEN_);

	if (if_part && then_part) {
		nsm_pattern if_pattern(s, if_part, segment);
		if (if_pattern.search()) {
			nsm_pattern_with_params then_pattern(s, then_part, production->seg); // FIXME: Working in pattern segment

			sc_addr search_results = if_pattern.get_results();

			// Iterate over search results.
			sc_iterator *search_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, search_results, SC_A_CONST|SC_POS, 0), true);
			sc_for_each(search_it) {
				sc_addr result = search_it->value(2);

				// Iterate over pairs "pattern element - searched value" in current search result.
				sc_iterator *conn_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, result, SC_A_CONST|SC_POS, 0), true);
				sc_for_each(conn_it) {
					sc_addr conn = conn_it->value(2);

					sc_addr first = sc_tup::at(s, conn, N1_);
					sc_addr second = sc_tup::at(s, conn, N2_);
					
					if (search_3_f_cpa_f(s, then_part, 0, first) == RV_OK)
						then_pattern.get_params_map()[first] = second;
				}

				break; // TODO: Handle more then one result 
			}

			then_pattern.gen();
		} else {
			count_not_applied++;
		}
	} else {
		s->erase_el(production);
	}

	cur_index++;

	return RV_OK;
}

bool productions_operation::activateImpl(sc_addr paramSystem, sc_segment *tmpSegment)
{
	sc_addr productions = sc_tup::at(s, PRODUCTION_ENGINE, paramSystem);

	if (s->get_out_qnt(productions) != 0) {
		productions_activity *activity = new productions_activity(s, tmpSegment, productions);
		sc_addr sign = s->create_el(tmpSegment, SC_N_CONST);
		s->reimplement(sign, activity);
		activity->init(sign);

		pm_sched_add_agent(sign, SCHED_CLASS_NORMAL);
	} else {
		s->unlink(tmpSegment->get_full_uri());
	}

	return true;
}
