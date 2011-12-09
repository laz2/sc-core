
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
 * @file act_on_act.h 
 * @author Alexey Rukin
 * @brief Описание активностИ, которая выполняется при появлении активной NSM-цели.
 */

#ifndef ___ACTIVITY_ON_ACTIVE__H__
#define ___ACTIVITY_ON_ACTIVE__H__

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include <libsc/std_constraints.h>

#ifndef NSM_DONT_USE_SCHEDULER
#	include <libpm/pm_sched.h>
#endif //NSM_DONT_USE_SCHEDULER

#include <libsc/pm_keynodes.h>
#include <libsc/libsc.h>
#include <list>

#include "nsm_keynodes.h"
#include "act_on_confirm_p.h"

/**
* @brief Активность для поиска активных целей.
* Во время активизации осуществляет проверку списока активных целей 
* на наличие начала и конца дуги и помещает цель в соответствующую очередь.
**/
struct Sact_on_active_ : public sc_activity {

	// TODO: Хранить карту <дуга, структура описывающая цель>
    std::list<Sact_goals_msg> active_goals; //<active_goals - список дуг, "только что" вышедших из active_ и их концов.
	//sc_segment *res_seg;
	
	sc_retval init(sc_addr _this) {return RV_OK;};
	void done() {delete this;};

	sc_retval activate(sc_session *,sc_addr _this,sc_addr param1,sc_addr,sc_addr);

	static void read_ordered_set(sc_addr ordered_set,  std::map<int, sc_addr> &return_set);

	void get_synonims(sc_addr query, sc_addr src_maj, sc_addr src_min, sc_segment *res_seg, sc_addr);

	void copy_pattern(sc_addr query,  sc_addr command,
						sc_addr language,		// FIXIT. язык, который использовать.
				  /*out*/LISTSC_ADDR &ret_elms, 
				  /*out*/bool &bRet, sc_segment *res_seg,
				  /*out*/MAPSC_ADDR &replace_map);
};

/** Указатель на активность */
extern Sact_on_active_ *activity_on_active_;

#endif //___ACTIVITY_ON_ACTIVE__H__
