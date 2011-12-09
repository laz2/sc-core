
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
 * @file act_on_gen.h 
 * @author Alexey Rukin
 * @brief Описание активностИ, которая выполняется при наличии активных запросов на генерации по образцу.
 */

#ifndef __ACTIVITY_ON_GENERATE_H__
#define __ACTIVITY_ON_GENERATE_H__

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif


#include <libsc/std_constraints.h>

#ifndef NSM_DONT_USE_SCHEDULER
#	include <libpm/pm_sched.h>
#endif //NSM_DONT_USE_SCHEDULER

#include <libsc/pm_keynodes.h>
#include <list>
#include <map>
#include "nsm_keynodes.h"
#include "act_on_confirm_p.h"

/**
* проверяем список активных целей (ищем начало и конец дуги) и помещаем 
*  в соответствующую очередь.
**/
struct Sact_on_gen_ : public sc_activity 
{
	typedef std::list<SIsomQuery> ListIsomQuery;
	typedef std::map<sc_addr, ListIsomQuery> addr2query_map;
	addr2query_map deny_queries;
	
	typedef std::list<Sact_goals_msg> goal_list;

	goal_list active_goals; /// List of arcs from NSM_GENERATE_
	
	sc_retval init(sc_addr _this) {return RV_OK;};
	void done() {delete this;};
	
	sc_retval activate(sc_session *,sc_addr _this,sc_addr param1,sc_addr,sc_addr);

	/**
	 * @brief Generate construction for pattern at the end of @p arc_goal.
	 * @note Use pattern segment for generating.
	 * @param arc_goal arc with pattern query
	 * @param params parameters mapping from pattern element to parameter
	 * @param[out] return_list if present then contains gened elements
	 */
	void do_confirm_(sc_addr arc_goal, const addr2addr_map &params, addr_list *return_list=0);

	bool do_deny_(sc_addr query, LISTSC_ADDR *ret_lst = 0);

	//! For @p t returns corresponding type for generating.
	inline sc_type get_corresponding_type(sc_type t)
	{
		sc_type res = t & (SC_SYNTACTIC_MASK|SC_FUZZYNESS_MASK); //arc, node, undef, pos, neg, fuz

		if (t & SC_VAR)
			res |= SC_CONST;
		else if (t & SC_METAVAR)
			res |= SC_VAR;

		return res;
	}
};

extern Sact_on_gen_ *act_on_gen_;

#endif // __ACTIVITY_ON_GENERATE_H__
