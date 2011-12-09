
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
 * @file act_on_act.cpp
 * @author Alexey Rukin
 */

#include "libpm_precompiled_p.h"

#include "act_on_act_p.h"

#include "trace_p.h"

/**
 * @def ADD_TO_DATA(a) Генерирует дугу из data в а, если такой дуги не существует.
 */
#define ADD_TO_DATA(a) \
	if (RV_OK!=search_oneshot(nsm_session, sc_constraint_new(CONSTR_3_f_a_f, data, 0, a), true, 0)) { \
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, a); \
	}

/**
 * @def ADD_TO_DATA3(x,y) Помещает в множество data трехэлементную конструкцию х->y.
 */
#define ADD_TO_DATA3(x,y) \
	ADD_TO_DATA(x); \
	ADD_TO_DATA(y); \
	{ \
		nsm_session->gen3_f_a_f(x, &tt, res_seg, SC_A_CONST, y); \
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, tt); \
	}

/**
 * @brief Вызывается при наличии в очереди активной цели.
 */
sc_retval Sact_on_active_::activate(sc_session *,sc_addr, sc_addr, sc_addr, sc_addr)
{
	static LISTSC_ADDR ret_elems;
	static MAPSC_ADDR replace_map;

	std::list<Sact_goals_msg>::iterator it = active_goals.begin();

	for (; it != active_goals.end(); it++) {
		sc_addr activeGoal = it->active_goal;
		
		if (search_3_f_cpa_f(nsm_session, NSM_GOALS, 0, nsm_session->get_beg(activeGoal)) == RV_OK)  {
			sc_addr confirmArc;
			sc_addr command;

			if (search_3_f_cpa_f(nsm_session, NSM_CONFIRM_, &confirmArc, activeGoal) == RV_OK) {
				nsm_session->erase_el(confirmArc);
				ret_elems.clear();
				activity_on_confirm_->add_query(activity_on_confirm_->queries, activeGoal, 0, 0, 0, ret_elems, replace_map, false);
				pm_sched_wakeup(NSM_CONFIRM_);
			} else if (search_3l2_f_cpa_cna_cpa_f(nsm_session, NSM_COMMAND, 0, &command, 0, activeGoal) == RV_OK) {
				/// Если это команда навигации, 
				/// то переделываем её в запрос на поиск конструкции по шаблону..
				sc_addr commandImpl = sc_tup::at(nsm_session, command, NSM_COMMAND_);
				if (commandImpl) {
					sc_addr comment = sc_tup::at(nsm_session, commandImpl, NSM_COMMAND_COMMENT_);
					sc_addr show_content = sc_tup::at(nsm_session, commandImpl, NSM_SHOW_CONTENT_);
					sc_addr layout = sc_tup::at(nsm_session, commandImpl, NSM_COMMAND_LAYOUT_);

					sc_segment * seg = it->getResSeg();

					bool bRet;
					copy_pattern(nsm_session->get_end(activeGoal), commandImpl, 0, ret_elems, bRet, seg, replace_map);
					act_on_confirm_::add_query(activity_on_confirm_->queries, 
						activeGoal, 
						comment, 
						show_content,
						layout,
						ret_elems, 
						replace_map,
						bRet);

					dprintf("[NSM] Wake up confirm_!!!\n");

					pm_sched_wakeup(NSM_CONFIRM_);
				}
			} else {
				// TODO: не CONFIRM и не nsm-комманда
			}
		} else {
			// TODO: в очереди не цель
		}

		active_goals.erase(it);
		break;
	}

	if (active_goals.empty()) {
		pm_sched_put_to_sleep(NSM_SEARCH_);
		dprintf("[NSM] activity on search_: put to sleep!\n");
	}

	return RV_OK;
}



/**
* @brief Считывает упорядоченное множество элементов
* @param ordered_set упорядоченное множество
* @param return_set возвращаемая карта (номер <-> значение)
*/
void Sact_on_active_::read_ordered_set(sc_addr ordered_set, std::map<int, sc_addr> &return_set)
{
	SYSTRACE("read_ordered_set:\n");
	sc_addr elem;

	return_set.clear();

	sc_iterator*iter = nsm_session->create_iterator(sc_constraint_new(
					CONSTR_5_f_a_a_a_a,
					ordered_set, 0, 0, 0, 0),true);
	for (;!iter->is_over();iter->next()) {
		for (int i=0; i<NUMATTR_CNT; i++)
			// this is big question
			if (pm_numattr[i]==iter->value(4)) {
				SYSTRACE2(i, "<===\n");
				return_set.insert(std::map<int, sc_addr>::value_type(i, iter->value(2)));
				break;
			}
	}
	delete iter;

	if (return_set.empty()) {       // Значит подменяется только один элемент...
		if (RV_OK!=search_oneshot(nsm_session,
									sc_constraint_new(CONSTR_3_f_a_a,
									ordered_set, 0, 0, 0),
								true,1, 2, &elem)) 
		{
			TRACE_ELEM_1("[NSM] copy_pattern: Can't find Elems in ordered_set! :", ordered_set);
			return;
		}   
		return_set.insert(std::map<int, sc_addr>::value_type(1, elem));
	}
}



/**
 * @brief Подставляет шаблон команды в запрос.
 *        Т.е. подменяет команду запросом на изоморфный поиск.
 * @param query запрос, в который нужно подставить шаблон.
 * @param command Команда.
 * @param language	Язык.
 * @param ret_elms [out] Множество возвращаемых/невозвращаемых элементов
 *        (те, соответствия которым нужно найти или
 *        те, соответствия которых нужно исключить из результирующего множества).
 * @param bRet [out] \e true - в \b ret_elms содержится множество элементов,
 *                   соответствия которым нужно найти.
 *        <br> \e false - в \b ret_elms содержится множество элементов,
 *                   соответствия которым исключить из результирующего множества.
 * @param res_seg		Результирующий сегмент.
 * @param replace_map	Карта соответствий, (элемент шаблона<->элемент запроса)
 *						пока будем генерировать её тока для узлов. FIXIT.
 */
void Sact_on_active_::copy_pattern(sc_addr query,
								   sc_addr command,
								   sc_addr language,		// FIXIT. язык, который использовать.
						/*out*/LISTSC_ADDR &ret_elms,
							   /*out*/bool &bRet,
								sc_segment *res_seg,
								MAPSC_ADDR &replace_map)
{   
	sc_addr c_pattern;          //!< Шаблон команды
	sc_addr command_elem_set;   //!< Множество подменяемых элементов команды.
	sc_addr ret_elems;

	std::map<int, sc_addr> replace_elem;    //!< Множество подменяемых элементов команды.
	std::map<int, sc_addr> elems_4_replace; //!< Множество элементов запроса для подмены.

	std::map<int, sc_addr>::iterator it_re_beg;
	std::map<int, sc_addr>::iterator it_re_end;
	std::map<int, sc_addr>::const_iterator c_it;

	ret_elms.clear();
	replace_map.clear();

	//hack_print_el(query);
	
	TRACE_ELEM_1("[NSM] copy_pattern: command: ", command)

	if (RV_OK!=search_oneshot(nsm_session,
				sc_constraint_new(CONSTR_5_f_a_a_a_f,
				command, 0, SC_N_CONST, 0, NSM_COMMAND_PATTERN_
		),true,1, 2, &c_pattern)) 
	{
		TRACE_ELEM_1("[NSM] copy_pattern: ERROR! Can't find command pattern! :", command);
		return;
	}
	
	if (RV_OK!=search_oneshot(nsm_session,
		sc_constraint_new(CONSTR_5_f_a_a_a_f,
							command, 0, 0, 0, NSM_COMMAND_ELEM_
							),true,1, 2, &command_elem_set)) 
	{
		TRACE_ELEM_1("[NSM] ERROR ! can't find nsm_command_elem_\n", command);
		return;
	}

	bRet=false;
	ret_elems = NULL;
	if (RV_OK!=search_oneshot(nsm_session,
		sc_constraint_new(CONSTR_5_f_a_a_a_f,
							command, 0, 0,0, NSM_NOT_RETURNED_ELEMS_
							),true,1, 2, &ret_elems)) 
	{
		bRet = true;
		TRACE_ELEM_1("[NSM] can't find nsm_not_returned_elems_\n", command);
		if (RV_OK!=search_oneshot(nsm_session,
				sc_constraint_new(CONSTR_5_f_a_a_a_f,
								command, 0, 0, 0, NSM_RETURNED_ELEMS_
								),true,1, 2, &ret_elems)) 
		{
			TRACE_ELEM_1("[NSM] can't find nsm_returned_elems_\t Return All!!!", command);
		}
	}

	// Считываем подменяемые элементы
	read_ordered_set(command_elem_set, replace_elem);
	if (replace_elem.empty())   return;
	read_ordered_set(query, elems_4_replace);
	if (replace_elem.empty())   return; //SRE: maybe check elems_4_replace??!!

	it_re_beg = elems_4_replace.begin();
	for (; it_re_beg!=elems_4_replace.end(); it_re_beg++)
	{
		if (RV_OK==search_oneshot(nsm_session,
			sc_constraint_new(CONSTR_5_f_a_f_a_f,
			query, 0, it_re_beg->second, 0, NSM_USE_MAIN_SYNONIM_),true,0))
		{
			it_re_beg->second = act_on_confirm_::genMainSynonim (it_re_beg->second);
		} else {
			it_re_beg->second = it_re_beg->second;
		}

		it_re_end = replace_elem.find(it_re_beg->first);

		// replace_elem <-> elems_4_replace
		if (it_re_end!=replace_elem.end())
			replace_map.insert(std::make_pair(it_re_end->second, it_re_beg->second));
	}

	sc_iterator *iter=NULL;
	if (ret_elems) {
		TRACE_ELEM_1("nsm_return_elems_:", ret_elems);
		iter = nsm_session->create_iterator(sc_constraint_new(
			CONSTR_3_f_a_a,
			ret_elems, 0, 0),true);
		for (;!iter->is_over();iter->next()) {
			ret_elms.push_back(iter->value(2));
			TRACE_ELEM_1("[NSM] ret_elem: ", iter->value(2));
		}
		delete iter;
	}       

	TRACE_MAP_INT_SC_ADDR("[NSM] copy_pattern: replace_elem:", replace_elem);
	TRACE_MAP_INT_SC_ADDR("[NSM] copy_pattern: elems_4_replace:", elems_4_replace);
	TRACE_ELEM_1("[NSM] copy_pattern: query:", query);

	/// Теперь скопируем шаблон...
	MAPSC_ADDR lst;
	MAPSC_ADDR::iterator it, it_tmp;
	sc_addr t, t1, tmp;
	sc_addr beg;
	sc_addr end;
	sc_type type;
	int count=0;
	iter = nsm_session->create_iterator(sc_constraint_new(
		CONSTR_3_f_a_a,
		c_pattern, 0, SC_ARC_ACCESSORY),true);
	for (;!iter->is_over();iter->next()) {
		tmp = iter->value(2);
//      TRACE_ELEM_1("[NSM] copy_pattern: elem:", tmp);
		lst.insert(MAPSC_ADDR::value_type(tmp, SCADDR_NIL));
		count++;
	}
	delete iter;

	it = lst.begin();
	while (count) {
		tmp = it->first;
		it_tmp = it /*lst.find(tmp)*/;
		if (it_tmp->second)
			goto NEXT;
		
		TRACE_ELEM_1("[NSM] copy_pattern: tmp:", tmp);

		type = nsm_session->get_type(tmp);

		if (type & SC_CONST) {
			nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, tmp);
		}

		beg = nsm_session->get_beg(tmp);
		end = nsm_session->get_end(tmp);
		assert(beg);
		assert(end);

		if (nsm_session->get_type(end) & SC_ARC_ACCESSORY) {
			if (!lst[end])
				goto NEXT;
		}

		it_re_beg = find_key_by_value(replace_elem.begin(), replace_elem.end(), beg);
		it_re_end = find_key_by_value(replace_elem.begin(), replace_elem.end(), end);

		
		if (replace_elem.end()!=it_re_beg) {		// Начало нужно подменить
			//TRACE_ELEM_1("it_re_beg->first:", it_re_beg->first);
			//TRACE_ELEM_1("it_re_beg->second:", it_re_beg->second);
			tmp = nsm_session->create_el(res_seg, type);	// создали дугу

			c_it = elems_4_replace.find(it_re_beg->first);
			if (c_it!=elems_4_replace.end()) {
				nsm_session->set_beg(tmp, c_it->second);
			} else { // Не на что подменять?
				std::ostringstream s;
				s << "Ошибка! Не соответствуют индексы элементов в шаблоне поиска и запросе !!! ";
				s << /*ADDR_OR_IDTF(*/it_re_beg->first/*)*/;

				fprintf(stderr, s.str().c_str());
				SC_ABORT_MESSAGE(s.str().c_str());
			}

			nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, tmp);
			//TRACE_ELEM_1("tmp",tmp)

			if (replace_elem.end()!=it_re_end)
				nsm_session->set_end(tmp, elems_4_replace[it_re_end->first]);
			else if (nsm_session->get_type(end)&SC_ARC_ACCESSORY) {
				sc_addr x = lst[end];
				nsm_session->set_end(tmp, x);
				if (RV_OK!=search_oneshot(nsm_session,
					sc_constraint_new(CONSTR_3_f_a_f,
					query, 0, x),true,0))
					nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, x);
			} else {
				nsm_session->set_end(tmp, end);
				if (RV_OK!=search_oneshot(nsm_session,
					sc_constraint_new(CONSTR_3_f_a_f,
					query, 0, end),true,0))
					nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, end);
			}

			it->second = tmp;
			count--;

		}
		else if (replace_elem.end()!=it_re_end)
		{
			tmp = nsm_session->create_el(res_seg, type);
			nsm_session->set_end(tmp, elems_4_replace[it_re_end->first]);
			nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, tmp);

			if (replace_elem.end()!=it_re_beg)
				nsm_session->set_beg(tmp, elems_4_replace[it_re_beg->first]);
			else
				nsm_session->set_beg(tmp, beg);

			it->second = tmp;
			count--;
			if (RV_OK!=search_oneshot(nsm_session,
				sc_constraint_new(CONSTR_3_f_a_f,
				query, 0, beg),true,0))
				nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, beg);
		} else {
			if (nsm_session->get_type(end)&SC_ARC_ACCESSORY) {
				it_tmp = lst.find(end);
				if (it_tmp->second) {
					// add begin
					if (RV_OK!=search_oneshot(nsm_session,
						sc_constraint_new(CONSTR_3_f_a_f,
						query, 0, beg),true,0))
						nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, beg);
					
					if (it_tmp->second==end) {
						// add end
						if (RV_OK!=search_oneshot(nsm_session,
							sc_constraint_new(CONSTR_3_f_a_f,
							query, 0, end),true,0))
							nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, end);
						// add arc
						nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, tmp);
						it->second = tmp;
					} else {
						nsm_session->gen3_f_a_f(beg, &t1, res_seg, type, it_tmp->second);
						nsm_session->gen3_f_a_f(query, &t, res_seg, SC_A_CONST, t1);
						it->second = t1;
					}
					count--;
				}
			} else {
				// add begin
					if (RV_OK!=search_oneshot(nsm_session,
						sc_constraint_new(CONSTR_3_f_a_f,
						query, 0, beg),true,0))
						nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, beg);
				// add end
					if (RV_OK!=search_oneshot(nsm_session,
						sc_constraint_new(CONSTR_3_f_a_f,
						query, 0, end),true,0))
						nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, end);
				// add arc
					if (RV_OK!=search_oneshot(nsm_session,
						sc_constraint_new(CONSTR_3_f_a_f,
						query, 0, tmp),true,0))
						nsm_session->gen3_f_a_f(query, &t1, res_seg, SC_A_CONST, tmp);
					it->second = tmp;
					count--;
			}
		}
NEXT:

		it++;
		if (it==lst.end())
			it = lst.begin();
	}
	TRACE_SET_ELEM(query);
/*
#ifdef TRACE_ISOM
	it1 = ret_elms.begin();
	for (; it1!=ret_elms.end(); it1++) {
		TRACE_ELEM_1 ("[NSM]  End of copy_pattern: ret_elms: elem:  ", *it1);
	}
#endif //TRACE_ISOM
*/
}

/**
 * @brief поиск синонимичных элементов
 */
void Sact_on_active_::get_synonims(sc_addr query, sc_addr src_maj, sc_addr src_min, sc_segment *res_seg, sc_addr arc_res)
{
//TODO    systrace << "[NSM] synonims!\n";
	sc_addr param;
	sc_addr arc;
	sc_addr msg_data;

	sc_segment *free_seg = create_unique_segment(nsm_session, "/tmp/nsm");
	sc_generator gen(nsm_session, free_seg);
	msg_data = gen.enter_system("");

	if (RV_OK == search_oneshot(nsm_session,
			sc_constraint_new(CONSTR_3_f_a_a, query, 0, 0), true, 1, 2, &param))
	{
		gen.element(NSM_RESULT);
		gen.set_active_segment(res_seg);
		gen.arc();
		gen.element("");
		gen.arc();
		gen.element(arc_res);
		gen.arc();
		gen.enter_system("");
			sc_iterator *it = nsm_session->create_iterator(sc_constraint_new(
						CONSTR_3l2_f_a_a_a_f, SCON_SYNONYMY, 0, 0, 0, param),true);
			for (; !it->is_over(); it->next()) {
				gen.element(SCON_SYNONYMY);
				gen.element(it->value(1));
				gen.element(it->value(2));

				sc_iterator *iter = nsm_session->create_iterator(sc_constraint_new(
						CONSTR_3_f_a_a, it->value(2), 0, 0),true);
				for (; !iter->is_over(); iter->next()) {
					gen.element(iter->value(1));
					gen.element(iter->value(2));
					if (RV_OK == search_oneshot(nsm_session, sc_constraint_new(
							CONSTR_3_f_a_f,SCON_MAIN_, 0, iter->value(1)), true, 1, 1, &arc))
					{
						gen.element(arc);
					}
				}
				delete iter;			
			}
			delete it;
		gen.leave_system();
	}

	gen.leave_system();
	gen.finish();
}
