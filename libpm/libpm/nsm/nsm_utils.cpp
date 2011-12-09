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

#include "w_act_out_arc_p.h"

Sact_on_active_ *activity_on_active_;
act_on_confirm_ *activity_on_confirm_;
Sact_on_gen_ *act_on_gen_;

#define GET_OPTION(opt, var)  \
	if (RV_OK==search_oneshot(nsm_session, sc_constraint_new(CONSTR_3_f_a_f,\
									opt, 0, goal_arc),true,0))\
		var = true;\
	else {TRACE_ELEM_1("can't find option: ", opt);var = false;}

/*
*  @brief считывает опции у цели.
*  @param goal_arc - дуга в которую была проведена дуга из active_
*/
void get_options(sc_addr goal_arc, SIsomQuery &iq)
{
	if (!goal_arc) {
		return;
	}
	
	GET_OPTION(NSM_STORE_IN_ONE_, iq.bStoreInOne)
	GET_OPTION(NSM_SEARCH_ONLY_ONE_, iq.bOnlyOne)
	GET_OPTION(NSM_RETURN_ONLY_, iq.bReturnOnly)
	
	iq.bReturnOnly = false;

	sc_addr tmp;

	if (search_3_f_cpa_f(nsm_session, NSM_SEARCH_IN_SEGMENT_, &tmp, goal_arc) == RV_OK){
		// TODO: iq.search_in_seg = tmp->seg;
	} else {
		SYSTRACE("search in ALL segments (by default)\n");
	}
}
