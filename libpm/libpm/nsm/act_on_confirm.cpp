
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
 * @file act_on_confirm.cpp
 * @author Alexey Rukin
 */

#include "libpm_precompiled_p.h"

#include "act_on_confirm_p.h"
#include "trace_p.h"

#define OUT

void get_options(sc_addr goal_arc, SIsomQuery &iq);

/**
 * @brief Возвращает продолжение дуги. Т.е. элемент, противоположный el.
 * @param arc - Дуга, продолжение которой нужно найти.
 * @param el - Известный элемент дуги (начало или конец её).
 * @return Неизвестный элемент дуги.
 */
sc_addr act_on_confirm_::get_adj(sc_addr arc, sc_addr el)
{
	sc_addr tmp = nsm_session->get_beg(arc);
	if (tmp==el)
		return nsm_session->get_end(arc) ;
	else
		return tmp;
}



/**
 * @brief Ищет дугу, которая входит в элемент elem.
 * @param set_of_arc - Множество дуг.
 * @param elem - Элемент, ищется дуга в который.
 * @return \e NULL, если дуга не найдена, <br> в противном случае дуга,
 *			которая входит в elem.
 */
sc_addr act_on_confirm_::search_in_arc(
				LISTSC_ADDR set_of_arc, sc_addr elem)
{
	LISTSC_ADDR::const_iterator it=set_of_arc.begin();
	for (; it!=set_of_arc.end(); it++) {
		if (nsm_session->get_end(*it)==elem)
			return *it;
	}

	return NULL;
}


/**
 * @brief Ищет дугу, которая выходит из элемента elem.
 * @param set_of_arc - Множество дуг.
 * @param elem - Элемент, ищется дуга из которого.
 * @return \e NULL, если дуга не найдена, <br> в противном случае дуга, которая
 *         выходит из elem.
 */
sc_addr act_on_confirm_::search_out_arc(
				LISTSC_ADDR set_of_arc, sc_addr elem)
{
	LISTSC_ADDR::const_iterator it=set_of_arc.begin();
	for (; it!=set_of_arc.end(); it++) {
		if (nsm_session->get_beg(*it)==elem)
			return *it;
	}
	
	return NULL;
}

/**
 * @brief Ищет входящую или выходящую дугу.
 * @param set_of_arc - Множество дуг.
 * @param elem - Элемент, ищется дуга в который/из которого.
 * @return \e NULL, если дуга не найдена, <br> в противном случае дуга,
 *			которая входит/выходит в/из elem.
*/
sc_addr act_on_confirm_::search_in_or_out_arc(
				  LISTSC_ADDR set_of_arc, sc_addr elem)
{
	sc_addr a = search_in_arc(set_of_arc, elem);
	if (!a)
		return search_out_arc(set_of_arc, elem);
	else
		return a;
}


/**
 * @brief Добавляет запрос к текущему множеству.
 * @param queries	- Множество запросов.
 * @param arc_for_gen_confirmed_ - Дуга, в которую проводится дуга из confirmed_
 * @param coment	- В содержимом данного узла валяется коментарий к команде.
 * @param show_content - Множество элементов, для которых нужно отображать содержимое.
 * @param layout	- Разметка
 * @param ret_elms	- Множество элементов, соответствия которым нужно вернуть(исключить)
 *        в(из) результирующего множества.
 * @param bRet - Индикатор того, что в ret_elms содержится множество элементов, 
 *        соответствия которым нужно вернуть(true) или исключить(false)
 *        в(из) результирующего множества.
 */
sc_retval act_on_confirm_::add_query(std::list<SIsomQuery> &queries,
					sc_addr arc_for_gen_confirmed_,
					sc_addr coment,
					sc_addr show_content,
					sc_addr layout,
					LISTSC_ADDR	&ret_elms,
					MAPSC_ADDR &replace_map,
					bool bRet)
{
	SIsomQuery iq;
	SIsomVariant iv;
	sc_addr pattern = nsm_session->get_end(arc_for_gen_confirmed_);
	iq.arc_for_gen_confirmed_ = arc_for_gen_confirmed_;
	iq.replace_map = replace_map;
	iq.comment = coment;
	iq.show_content = show_content;
	iq.layout = layout;
	
	if (bRet) {
		iq.returned_elems = ret_elms;
	} else {
		iq.not_returned_elems = ret_elms;
	}

	get_options(arc_for_gen_confirmed_, iq);
	
	sc_iterator *iter = nsm_session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, pattern, 0, 0), true);
	
	sc_type tmp_type;
	int cnt=0;
	for (; !iter->is_over(); iter->next(), cnt++) {
		sc_addr tmp = iter->value(2);
		tmp_type = nsm_session->get_type(tmp);
		R_TRACE_ELEM(tmp)
		R_SYSTRACE("\n")
			
		iq.patt.push_back(tmp);
		if ((tmp_type & SC_ARC) && !(tmp_type & SC_CONST)) {
			iv.analArcs.push_back(tmp);
			continue;
		}
		
		//	Во фронт включаем константные элементы 
		// инцидентные переменным и метапеременным дугам шаблона.
		if (tmp_type & SC_CONST) {
			TRACE_ELEM_1("const:\t", tmp);
			if ((RV_OK==search_oneshot(nsm_session, sc_constraint_new(CONSTR_5_f_a_a_a_f, tmp,SC_VAR|SC_METAVAR,0,0,pattern),true,0)) ||
				(RV_OK==search_oneshot(nsm_session, sc_constraint_new(CONSTR_5_a_a_f_a_f, 0,SC_VAR|SC_METAVAR,tmp,0,pattern),true,0)))
			{
				iv.front.push_back(tmp);
				iv.corr.insert(MAPSC_ADDR::value_type(tmp, tmp));
			}
		}
	}
	delete iter;

	// Если фронт пуст, то запрос не найден...
	iq.variants.push_back(iv);
	queries.push_back(iq);


	if (iv.front.empty() || iv.analArcs.empty()) {
		TRACE_ERROR("front is empty!");
		sc_addr data;
		data = nsm_session->create_el(create_unique_segment(nsm_session, "/tmp/nsm"), SC_N_CONST);
		std::list<SIsomQuery>::iterator cur_q = queries.end();
		cur_q--;
		end_isom(cur_q, data, arc_for_gen_confirmed_->seg, 0);
		queries.erase(cur_q);
		return 1;
	}
	// TRACE_INFO("Pattern added (" << (int)iv.front.size() << '/' << cnt << ").");
/*TODO    systrace << "[NSM] pattern: "
		<< (char*)nsm_session->get_idtf(pattern).c_str()
		<< " added ! " << (int)iv.front.size() << '/' << cnt << " (front/total)\n";*/

	return RV_OK;
}


bool act_on_confirm_::is_corresponding_type(sc_addr pattern, sc_addr elem)
{
	sc_type pt = nsm_session->get_type(pattern);
	sc_type et = nsm_session->get_type(elem);

	// Узлу должен соответствовать узел!
	if ((pt & SC_NODE) && !(et & SC_NODE))
		return false;

	// Дуге должена соответствовать дуга!
	if ((pt & SC_ARC) && !(et & SC_ARC))
		return false;

	// Константе должена соответствовать самой себе
	if ((pt & SC_CONST) && (et & SC_CONST))
		return (pattern==elem);

	// Переменной соответствовать константа
	if ((pt & SC_VAR) && !(et & SC_CONST))
		return false;

	//if ((pt&(SC_FUZ|SC_METAVAR|SC_ARC)) && (et&SC_ARC))
	//    return true;

	// Метапеременной - переменная.
	//if ((pt&SC_METAVAR) && !(et&SC_VAR))
	//    return false;

	// неопределенный элемент - всему
	if (pt & SC_UNDF)
		return true;

	if ((pt & SC_POS) && !(et & SC_POS))
		return false;

	if ((pt & SC_NEG) && !(et & SC_NEG))
		return false;

	if (pt & SC_FUZ)
		return true;

	return true;
}

/**
* @brief проверяет подходит ли элемент elem под шаблонный элемент pattern.
* @param q - Запрос в рамках которого осуществляется поиск.
* @param pattern - шаблон
* @param elem - предполагаемое соответствие.
* @return true - соответствие возможно
*/
bool act_on_confirm_::can_corresponding(const SIsomQuery &q,
										sc_addr pattern, sc_addr elem)
{
	if (elem->dead || elem->seg->ring_0)
		return false;
	// Если искать нужно в опр. сегменте, то сегменты должны совпадать...
	if ((!q.search_in_seg.empty()) && (q.search_in_seg.find(elem->seg) == q.search_in_seg.end()))
		return false;

	bool correspond = false;

	// Check types corresponding
	if (correspond = is_corresponding_type(pattern, elem)) {
		// If pattern element has content then check content corresponding
		const Content *pc = nsm_session->get_content_const(pattern);
		if (pc->type() != TCEMPTY) {
			const Content *ec = nsm_session->get_content_const(elem);
			correspond = *ec == *pc;
		}
	}

	return correspond;
}


/**
 * @brief Проверяет должен ли elem быть добавлен в результирующее множество.
 */
bool act_on_confirm_::must_included_in_result(
					sc_addr elem, SIsomQuery &q)
{
	if (!q.not_returned_elems.empty()) {
		return (std::find(q.not_returned_elems.begin(), q.not_returned_elems.end(), elem)==
										q.not_returned_elems.end());
	}
	else if (!q.returned_elems.empty()) {
		return (std::find(q.returned_elems.begin(), q.returned_elems.end(), elem)!=
									   q.returned_elems.end());
	}

	return true;
}


/**
 * @brief - Генерирует результат.
 * @param data - Множество данных сообщения.
 * @param res_seg - Результирующий сегмент.
 * @param i - Количество найденных вариантов.
 */
void act_on_confirm_::create_report(sc_addr data,
									sc_segment *res_seg,
									int &i)
{
	sc_addr result = 0;
	sc_addr result1 = 0;
	sc_addr el_fr = 0;
	sc_addr el_fr_c = 0;
	sc_addr da = 0;
	sc_addr arc_gen_conf = cur_query->arc_for_gen_confirmed_;
	sc_segment *free_seg;

	if (data)
		free_seg = data->seg;
	else
		free_seg = res_seg;

	std::list<MAPSC_ADDR>::iterator it;
	MAPSC_ADDR::iterator map_it;

								// Если не надо сохранять результат, то
								// data = 0 и дуги не должны генерироваться...
	if (!cur_query->bReturnOnly) {
								// Ничего не нашли...
								// Генерируем, denied_.
		if (cur_query->result.empty()) {
			/*nsm_session->gen3_f_a_f(DENIED_, &da,
									nsm_segment, SC_A_CONST,
									cur_query->arc_for_gen_confirmed_);
			nsm_session->gen3_f_a_f(data, &el_fr, nsm_segment, SC_A_CONST, da);*/
			return;
		}
	}


			// data = [ Result -> result1 -> arc_for_gen_confirmed_; ];
	result1 = nsm_session->create_el(res_seg, SC_N_CONST);
	nsm_session->gen3_f_a_f(NSM_RESULT, &el_fr, res_seg, SC_A_CONST, result1);
	nsm_session->gen3_f_a_f(data, &el_fr_c, free_seg, SC_A_CONST, el_fr);
	nsm_session->gen3_f_a_f(data, &el_fr, free_seg, SC_A_CONST, NSM_RESULT);
	nsm_session->gen3_f_a_f(data, &el_fr, free_seg, SC_A_CONST, result1);
	nsm_session->gen3_f_a_f(result1, &el_fr, res_seg, SC_A_CONST, arc_gen_conf);
	nsm_session->gen3_f_a_f(data, &el_fr_c, free_seg, SC_A_CONST, el_fr);

	MAPSC_ADDR layout_map;
	sc_addr layout = cur_query->layout;
	if (cur_query->bReturnOnly) {
				// data = [ result1 -> result ];
		result = nsm_session->create_el(res_seg, SC_N_CONST);
		nsm_session->gen3_f_a_f(result1, &el_fr, res_seg, SC_A_CONST, result);
		nsm_session->gen3_f_a_f(data, &el_fr_c, free_seg, SC_A_CONST, el_fr);
		nsm_session->gen3_f_a_f(data, &el_fr_c, free_seg, SC_A_CONST, result);

		if (layout) {
			sc_addr markup = gen_layout_pattern (data, cur_query->layout, res_seg, free_seg, &layout_map, cur_query->replace_map);
			if (! markup) markup = layout;
			TRACE_MAP_SC_ADDR(nsm_session, layout_map);
			//	data = [
			//		markup_	-> {
			//			1_:	result,
			//			2_:	layout
			//		};
			//	];
			GEN_PREPARE();
			sc_generator g(nsm_session, res_seg);
			GEN_SYSTEM(g, data) {
				g.element(MARKUP_);
				g.arc();
				GEN_SET(g, "") {
					g.attr(N1_);
					g.element(result);
					g.attr(N2_);
					g.element(markup);
				};
			};
		}
	}

	sc_addr res_show_content = 0;
	if (cur_query->show_content) {
		res_show_content = nsm_session->create_el(free_seg, SC_N_CONST);
		nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, res_show_content);
		addToData(data, free_seg, NSM_SHOW_CONTENT_);

		if (data)
			nsm_session->gen5_f_a_f_a_f(NSM_SHOW_CONTENT_, 0, free_seg, SC_A_CONST, res_show_content,
											0, free_seg, SC_A_CONST, data);
	}

									// Перебираем соответствия...
	it=cur_query->result.begin();
	for (i=1; it!=cur_query->result.end(); it++, i++) 
	{
								/// генерируем результат...
		R_SYSTRACE ("\nQuery:")
		R_SYSTRACE ((char*)nsm_session->get_idtf(nsm_session->get_beg(arc_gen_conf)).c_str())
		R_SYSTRACE ("\tVariant №");
		R_SYSTRACE (i);
		R_SYSTRACE ("\n");

		if (!cur_query->bStoreInOne && !cur_query->bReturnOnly) {
					// data = [ result1 -> result ];
			result = nsm_session->create_el(res_seg, SC_N_CONST);
			nsm_session->gen3_f_a_f(data, &el_fr, free_seg, SC_A_CONST, result);
			nsm_session->gen3_f_a_f(result1, &el_fr, res_seg, SC_A_CONST, result);
			nsm_session->gen3_f_a_f(data, &el_fr_c, free_seg, SC_A_CONST, el_fr);
		}


		for (map_it=it->begin(); map_it!=it->end(); map_it++) {
			R_SYSTRACE ("\n");
			R_TRACE_ELEM(map_it->first);
			R_SYSTRACE ("<=>");
			R_TRACE_ELEM(map_it->second);

			if (!must_included_in_result(map_it->first, *cur_query)) {
				R_SYSTRACE("\t skiped !\n")
				continue;
			}
			store_result(res_seg, &layout_map,
							cur_query->bStoreInOne, 
							cur_query->bReturnOnly,
							cur_query->show_content, res_show_content,
							result,	data,
							map_it->first, map_it->second);

			//// Теперь все время добавляем информацию для отображения...
			R_SYSTRACE("\n")
			if (data)
				add_ui_info(map_it->second, data, res_seg);
		}
		R_SYSTRACE("\n")
	}


	printf("[NSM] find %d variants\n", i-1);
	nsm_session->gen3_f_a_f(NSM_CONFIRMED_, &da, res_seg, SC_A_CONST, arc_gen_conf);
}


/**
 *		Шаг поиска по образцу...
 *
 * @param q - Текущий запрос.
 * @param variants - Множество вариантов.
 * @param variant  - Текущий вариант.
 * @param result   - Результат
 * @param bOnlyOne - Искать только один вариант?
 *
 * @retval 1 - требуется еще шаг
 * @retval 0 - поиск ок
 * @retval 2 - denied_!
 *
 */
int act_on_confirm_::isom_do_step(const SIsomQuery &q,
								  std::list<SIsomVariant> &variants,
								  const std::list<SIsomVariant>::iterator &variant,
								  std::list<MAPSC_ADDR> &result,
								  bool bOnlyOne)
{
	sc_addr el_fr;          // элемент фронта.
	sc_addr d_an;           // дуга из analArcs.
	sc_addr d_an_c;         // Элемент, соответствующий d_an.
	sc_addr el_fr_c=0;      // элемент, соответствующий el_fr.
	sc_addr el_adj;         // продолжение дуги d_an.
	sc_addr el_adj_c=0;     // элемент, соответствующий el_adj.

	sc_addr da;
	sc_addr da_end;
	sc_addr da_beg;
	sc_addr da_c;
	sc_addr da_c_end;
	sc_addr da_c_beg;

	sc_iterator *sc_iter_tmp;

	MAPSC_ADDR::iterator map_it;
	MAPSC_ADDR::iterator map_it2;

	LISTSC_ADDR	m_d_an_c; // множество элементов, которые 
							//могут быть поставлены в соответствие d_an.
	LISTSC_ADDR::const_iterator it_tmp;// Для временного поиска.

	int minArcCnt=INT_MAX;
//  static int corrrr = 6;

	/*
	 * 8.1. ЕСЛИ множество analArcs текущего варианта var_any пусто, 
	 *  ТО исключить текущий вариант из variants и занести 
	 *  его во множество результатов result.
	 */
	if (variant->analArcs.empty() /*|| variant->front.empty()*/) {
		result.push_back(variant->corr);
		goto END;
	}

	if (variant->front.empty())
		goto END;

	it_tmp=variant->front.begin();

	el_fr = 0;
	sc_addr el_fr_tmp, el_fr_c_tmp, d_an_tmp;
	for (int c=INT_MAX; it_tmp!=variant->front.end(); it_tmp++)
	{
		el_fr_tmp = *it_tmp;
		d_an_tmp = search_in_arc(variant->analArcs, el_fr_tmp);
		el_fr_c_tmp = variant->corr.find(el_fr_tmp)->second;

		if (d_an_tmp) {
			c = nsm_session->get_in_qnt(el_fr_c_tmp);
		} else {
			c = nsm_session->get_out_qnt(el_fr_c_tmp);
			d_an_tmp = search_out_arc(variant->analArcs, el_fr_tmp);
		}

		if (c < minArcCnt) {
			minArcCnt = c;
			el_fr = el_fr_tmp;
			el_fr_c = el_fr_c_tmp;
			d_an = d_an_tmp;
		}
		//break;
	}
	TRACE_LIST_SC_ADDR("\nFront2", variant->front);
	TRACE_LIST_SC_ADDR("analArcs2", variant->analArcs);

	TRACE_ELEM_1("el_fr", el_fr);

	assert(d_an); // Нет ни входящих ни выходящих дуг из el_fr в множестве AnalArcs
	assert(el_fr);

	el_adj  = get_adj(d_an, el_fr);


	TRACE_ELEM_1("\nd_an", d_an);
	TRACE_ELEM_1("el_fr_c", el_fr_c);
#ifdef TRACE_ISOM/*
	systrace << ">>>>>>>>>>>>>>>>>\n";
	sc_iter_tmp =  nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_f_a_a, el_fr_c,0,0), true);
	for (; !sc_iter_tmp->is_over(); sc_iter_tmp->next())
	{
		TRACE_ELEM_1 ("", sc_iter_tmp->value(2));
	}
	delete sc_iter_tmp;
	systrace << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
	sc_iter_tmp =  nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_a_a_f, 0,0, el_fr_c), true);
	for (; !sc_iter_tmp->is_over(); sc_iter_tmp->next())
	{
		TRACE_ELEM_1 ("", sc_iter_tmp->value(0));
	}
	delete sc_iter_tmp;*/
#endif //TRACE_ISOM
	TRACE_ELEM_1("el_adj", el_adj);
	TRACE_MAP_SC_ADDR("corr2", variant->corr);


								// Если ищется дуга между двумя константами,
								// то, если в шаблоне дуга pos/neg, а 
								// существует дуга neg/pos, то denied_
	if ((nsm_session->get_type(el_fr)&SC_CONST) &&
		(nsm_session->get_type(el_adj)&SC_CONST))
	{
		sc_type ArcType = nsm_session->get_type(d_an)&SC_POS ? SC_NEG : SC_POS;

		if (nsm_session->get_beg(d_an)==el_fr) {
			if (RV_OK == search_oneshot(nsm_session,
					sc_constraint_new(CONSTR_3_f_a_f,
					el_fr, ArcType, el_adj),true,0))
			{
				return 2;
			}
		} else {
			if (RV_OK == search_oneshot(nsm_session,
					sc_constraint_new(CONSTR_3_f_a_f,
					el_adj, ArcType, el_fr),true,0))
			{
				return 2;               
			}
		}
	}

	it_tmp = std::find(variant->front.begin(), variant->front.end(), el_adj);
	if (it_tmp!=variant->front.end()) {
		el_adj_c = variant->corr.find(el_adj)->second;

								// 8.4.2. ЕСЛИ между элементами 
								// el_adj_с и el_fr_c существует SC-дуга 
								// d_an_c, "ориентированная" относительно 
								// el_fr_c точно таким же  образом, 
								// как d_an - относительно el_fr (т.е. обе 
								// эти дуги - либо входящие, либо выходящие), 
								// и не  поставленная  в варианте var_any 
								// никакой SC-дуге.
		if (nsm_session->get_beg(d_an)==el_fr)
			sc_iter_tmp =  nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_f_a_f,
									el_fr_c,
									0,
									el_adj_c), true);
		else
			sc_iter_tmp =  nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_f_a_f,
									el_adj_c,
									0,
									el_fr_c), true);

		TRACE_ELEM_1("el_fr_c:", el_fr_c);
		TRACE_ELEM_1("el_adj_c:", el_adj_c);        
/*
#ifdef TRACE_ISOM
		if (sc_iter_tmp->is_over()) {
			sc_iterator *it = nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_f_a_a, el_fr_c, 0, 0), true);
			for (; !it->is_over(); it->next()) {
				TRACE_ELEM_1("el_fr_c >>:", it->value(1))
			}
			delete it;
			
			it = nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_a_a_f, 0, 0, el_adj_c), true);
			for (; !it->is_over(); it->next()) {
				TRACE_ELEM_1("el_adj_c <<:", it->value(1))
			}
			delete it;
		}
#endif //TRACE_ISOM
*/
		for (; !sc_iter_tmp->is_over(); sc_iter_tmp->next()) {
			d_an_c = sc_iter_tmp->value(1);

			map_it = find_key_by_value(variant->corr.begin(),
										variant->corr.end(), 
										sc_iter_tmp->value(1));
							// 8.4.2. ЕСЛИ между элементами 
							// el_adj_с и el_fr_c существует SC-дуга 
							// d_an_c, "ориентированная" относительно 
							// el_fr_c точно таким же  образом, 
							// как d_an - относительно el_fr , и 
							// не  поставленная  в варианте variant
							// никакой SC-дуге, ТО
			if (map_it==variant->corr.end() && can_corresponding(q, d_an, d_an_c)) {
							// 8.4.2.1. Создаем новый вариант,
							// Фиксируем, что дуге d_an в 
							// соответствует    дуга d_an_c и исключаем 
							// её из множества analArcs.
				variant->analArcs.remove(d_an);
				
				variants.push_front(*variant);
				
				std::list<SIsomVariant>::iterator iiv = variants.begin();

				iiv->corr.insert(MAPSC_ADDR::value_type(d_an, d_an_c));
				
				clear_front(iiv, el_fr);
				clear_front(iiv, el_adj);
				add_front  (iiv, d_an);
			}
		}
		delete sc_iter_tmp;

		goto END;
	}

				//8.5. el_adj НЕ принадлежит множеству front (var_any)...
	int pos;
	if (nsm_session->get_beg(d_an)==el_fr) {
		sc_iter_tmp =  nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_f_a_a,
									el_fr_c,
									0,
									0), true);
		pos=2;
	}
	else {
		sc_iter_tmp =  nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_a_a_f,
									0,
									0,
									el_fr_c), true);
		pos = 0;
	}

							// Формируем множество m_d_an_c дуг, 
							// которые могут соответствовать d_an.
							// (В проверке участвует el_adj).
	for (; !sc_iter_tmp->is_over(); sc_iter_tmp->next())
	{
		if (variant->corr.end()!=find_key_by_value(variant->corr.begin(),
											variant->corr.end(), 
											el_adj_c, sc_iter_tmp->value(1)))
		{
			continue;
		}
//      TRACE_ELEM_1("sc_iter_tmp_1:", sc_iter_tmp->value(1))
		if (can_corresponding(q, d_an, sc_iter_tmp->value(1)))
			if (can_corresponding(q, el_adj, sc_iter_tmp->value(pos))) {
				m_d_an_c.push_back(sc_iter_tmp->value(1));
				TRACE_ELEM_1("can_corresponding!:", sc_iter_tmp->value(1))
			}
	}
	delete sc_iter_tmp;

	if (m_d_an_c.empty())
		goto END;

	variant->analArcs.remove(d_an);
	add_front(variant, d_an);
	add_front(variant, el_adj);
	clear_front(variant, el_fr);

/*  TRACE_LIST_SC_ADDR("\nFront2", variant->front);
	TRACE_LIST_SC_ADDR("analArcs2", variant->analArcs);
	TRACE_MAP_SC_ADDR("corr2", variant->corr);
*/

								// Перебираем все возможные соответствия
								// и формируем новые варианты...
	for (it_tmp=m_d_an_c.begin(); it_tmp!=m_d_an_c.end(); it_tmp++)
	{
		std::list<SIsomVariant>::iterator iiv;
		if (variant->analArcs.empty())
		{
			std::list<MAPSC_ADDR>::iterator i;
			result.push_front(variant->corr);
			i = result.begin();
			i->insert(MAPSC_ADDR::value_type(d_an,  *it_tmp));

#ifdef TRACE_ISOM
			TRACE_ELEM_1("insert....\nel_adj:", el_adj);
			TRACE_ELEM_1("d_an:", d_an);
			TRACE_ELEM_1("begin:", nsm_session->get_beg(*it_tmp));
			TRACE_ELEM_1("end:", nsm_session->get_end(*it_tmp));
			{
				SYSTRACE("map:\n");
				MAPSC_ADDR::const_iterator itt = i->begin();
					for (; itt!=i->end(); itt++) {
						TRACE_ELEM(itt->first);
						SYSTRACE("<=>");
						TRACE_ELEM(itt->second);
						SYSTRACE("\n");
					}
					SYSTRACE ("\n");
			}
			SYSTRACE2("pos:", pos);
//          SYSTRACE_MAP_SC_ADDR1("map:", cur_query->result);

#endif //TRACE_ISOM
			i->insert(MAPSC_ADDR::value_type(el_adj, 
						(pos==0)?nsm_session->get_beg(*it_tmp):nsm_session->get_end(*it_tmp)));
			if (bOnlyOne)
				goto END;
			
			continue;
		}
		
		variants.push_front(*variant);
		iiv = variants.begin();
		
		iiv->corr.insert(std::make_pair(d_an,  *it_tmp));
		iiv->corr.insert(std::make_pair(d_an,  *it_tmp));
		iiv->corr.insert(std::make_pair(el_adj, (pos==0)?
		nsm_session->get_beg(*it_tmp):nsm_session->get_end(*it_tmp)));

//          add_front(iiv, d_an);
//          add_front(iiv, el_adj);

		if (iiv->analArcs.empty())
			continue;

				// 8.5.7.4. ЕСЛИ el_adj - не SC-дуга, ТО перейти к ШАГУ 8.5.8.
		if (!(nsm_session->get_type(el_adj) & SC_ARC))
			continue;

		da = el_adj;                //8.5.7.5.  da := el_adj.
STEP_8_5_7_6:
		da_beg = nsm_session->get_beg(da);
		da_end = nsm_session->get_end(da);

								// 8.5.7.7. da_c = var_new (da), т.е. находим SC-дугу 
								// da_c соответствующую SC-дуге da.
		da_c = iiv->corr.find(da)->second;
		da_c_beg = nsm_session->get_beg(da_c);
		da_c_end = nsm_session->get_end(da_c);



		if (((map_it=iiv->corr.find(da_end))!=iiv->corr.end() && 
			map_it->second!=da_c_end)
			|| ((map_it2=iiv->corr.find(da_beg))!=iiv->corr.end() && 
			map_it2->second!=da_c_beg))
		{
			variants.erase(iiv);
			continue;
		}

		if (map_it==iiv->corr.end())
			iiv->corr.insert(MAPSC_ADDR::value_type(da_end, da_c_end));

		if (map_it2==iiv->corr.end())
			iiv->corr.insert(MAPSC_ADDR::value_type(da_beg, da_c_beg));

		iiv->analArcs.remove(da);
		add_front(iiv, da);
		add_front(iiv, da_beg);
		add_front(iiv, da_end);
		clear_front(iiv, da_beg);
		clear_front(iiv, da_end);

		if (iiv->analArcs.empty())
		{
			//list<MAPSC_ADDR>::iterator i;
			result.push_back(iiv->corr);
			variants.erase(iiv);

			if (bOnlyOne)
				goto END;

			continue;
		}

		if ((nsm_session->get_type(da_end) & SC_ARC) &&
			(std::find(iiv->analArcs.begin(),iiv->analArcs.end(), da_end)!=
			iiv->analArcs.end()))
		{
			da = da_end;
			TRACE_ELEM_1("da :", da);
			SYSTRACE("goto STEP_8_5_7_6");
			goto STEP_8_5_7_6;
		}
	}

END:
	if (bOnlyOne && !result.empty())
		variants.clear();
	else
		variants.erase(variant);

								// если перебрали все варианты, то пора 
								// создавать отчет...
	return (variants.empty())?0:1;
}


/**
* @brief Планировщик...
*/
sc_retval act_on_confirm_::activate(sc_session *, sc_addr, sc_addr, sc_addr, sc_addr)
{
	sc_addr data = 0;
	sc_addr result = 0;
	int i = 0;

	//bool bReturnMsg;
	sc_segment *res_seg;

	if (queries.empty()) {
#ifndef NSM_DONT_USE_SCHEDULER
		pm_sched_put_to_sleep(NSM_CONFIRM_);
		SYSTRACE("[NSM] activity on confirm_: nothing to do - put to sleep !!!\n");
#endif
		cur_query=queries.end();
		return RV_OK;
	};
	
	if (cur_query == queries.end())
			cur_query = queries.begin();

/*TODO	systrace << "[NSM] activity on confirm_ (ISOMORPHIUS) " 
		<< nsm_session->get_idtf(cur_query->arc_for_gen_confirmed_).c_str()
		<< ") !\n";*/

	res_seg = nsm_session->get_end(cur_query->arc_for_gen_confirmed_)->seg;

#ifdef NSM_DONT_USE_SCHEDULER
	for (int x=0; cur_query!=queries.end(); x++)
#else
	for (int x=0; (x<ISOM_STEPS_PERQUANT)&&(cur_query!=queries.end()); x++)
#endif //NSM_DONT_USE_SCHEDULER
	{
		int res = isom_do_step(*cur_query,
								cur_query->variants, cur_query->variants.begin(),
								cur_query->result, cur_query->bOnlyOne);
		switch (res)
		{
			case 2://denied_!
				//if (bReturnMsg)
				//	data = nsm_session->create_el(create_unique_segment(nsm_session, "/tmp/nsm"), SC_N_CONST);
				nsm_session->gen3_f_a_f(NSM_DENIED_, &result, res_seg, SC_A_CONST, cur_query->arc_for_gen_confirmed_);
				nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, result);
				end_isom(cur_query, data, res_seg, i);
				queries.erase(cur_query);
				cur_query=queries.begin();
				break;

			case 0:         
									// если перебрали все варианты, то пора 
									// создавать отчет...
				if (cur_query->variants.empty()) {
					int i=0;
					create_report(data, res_seg, i);
					end_isom(cur_query, data, res_seg, i);
					queries.erase(cur_query);
					cur_query=queries.begin();
				}
				break;

			default:
				break;
		}
	}


	if (cur_query != queries.end()) cur_query++;
	return RV_OK;
}


/**
 * @brief Завершающий этап изоморфного поиска:
 *		\li Добавляется множество возможных команд.
 *		\li Добавляется коментарий к команде.
 *		\li Делается пометку, что поиск завершен.
 *		\li Отсылается сообщение.
 *		\li Удаляется вариант.
 *		\li Берется следующий вариант.
 * @param cur_query  - Текущий запрос.
 * @param data       - Результирующее множество.
 * @param res_seg    - Результирующий сегмент.
 * @param i          - Количество найденных вариантов.
 */
void act_on_confirm_::end_isom(std::list<SIsomQuery>::iterator cur_query, 
							   sc_addr data, sc_segment *res_seg, int i)
{
	sc_addr da, arc;

	if (cur_query->comment) {
		// Добавляет коментарий к команде.
		//  Это нормально не отсылается :(
		/*
		nsm_session->gen5_f_a_f_a_f(data, 0, res_seg, SC_A_CONST, cur_query->comment,
								&arc, res_seg, SC_A_CONST, NSM_COMMAND_COMMENT_);
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, arc);
		*/

		da = cur_query->comment;
		nsm_session->gen3_f_a_f(NSM_COMMAND_COMMENT_, &arc, res_seg, SC_A_CONST, da);
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, arc);
	}
	
	arc = cur_query->arc_for_gen_confirmed_;

	if (i>1) { // Нашли !!!
		nsm_session->gen3_f_a_f(SC_TRUE, &da, res_seg, SC_A_CONST, arc);
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, da);
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, SC_TRUE);
	}

	nsm_session->gen3_f_a_f(NSM_SEARCHED_, &da, res_seg, SC_A_CONST, arc);
	nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, da);
	nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, NSM_SEARCHED_);
	nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, arc);

	printf("[NSM] Query: %s \t OK! search (%d) results\n",
			(char*)nsm_session->get_idtf(
					nsm_session->get_beg(arc)).c_str(), i-1);
}

inline void act_on_confirm_::store_result(sc_segment *res_seg,
				MAPSC_ADDR *layout_map,
				bool bStoreInOne, bool bReturnOnly,
				sc_addr show_content, sc_addr res_show_content,
				sc_addr result,   sc_addr data,
				sc_addr first,    sc_addr second)
{
	static sc_addr el_fr, el_fr_c, da, da_c, el_adj;
	sc_segment *free_seg;
	if (data)
		free_seg = data->seg;
	else
		free_seg = res_seg;

	da = second;

NEXT:
	el_adj = nsm_session->get_end(da);
	el_fr = nsm_session->get_beg(da);

	if (el_adj) {	// Есть конец !
		addToData(data, free_seg, el_fr);
		addToData(data, free_seg, el_adj);

		da = nsm_session->get_end(da);

		goto NEXT;
	}

	if (show_content) {
		if (RV_OK==search_oneshot(nsm_session,
			sc_constraint_new(CONSTR_3_f_a_f, show_content, 0, first),
			true, 0))
		{
			Content c = nsm_session->get_content(second);
			if (c.type() != TCEMPTY) {
				sc_addr arc;
				addToData(data, free_seg, res_show_content);
				arc = addToData(res_show_content, free_seg, second);
				if (arc)
					nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, arc);
			}
		}
	}

	if (first!=second) { // Это не константа в запросе...
		if (bStoreInOne || bReturnOnly)
			if (data &&  (RV_OK!=search_oneshot(nsm_session,
				sc_constraint_new(CONSTR_3_f_a_f, result,0, second), true, 0)))
			{
				//	Если это запрос ТМ-а, то сохраняем все в одном...
				//	data = [
				//		result -> layout_iter: second;
				//	];
				nsm_session->gen3_f_a_f(data, &el_fr, free_seg, SC_A_CONST,  second);
				nsm_session->gen3_f_a_f(result, &el_fr, res_seg, SC_A_CONST,  second);
				nsm_session->gen3_f_a_f(data, &el_fr_c, free_seg, SC_A_CONST, el_fr);
				if (!layout_map->empty()) {
					MAPSC_ADDR::const_iterator layout_iter;
					layout_iter = layout_map->find(first);
					if (layout_iter != layout_map->end())
						addToData(data, free_seg, addToData(layout_iter->second, res_seg, el_fr));
				}
				return;
			} else {
				return;
			}

					// data = [
					//    result -> {el_fr} -> 1_: first,
					//                         2_: second
					// ];
		TRACE_ELEM_1("1: ", first);
		TRACE_ELEM_1("2: ", second);
		nsm_session->gen3_f_a_f(data, &el_fr, free_seg, SC_A_CONST,  second);
		el_fr = nsm_session->create_el(res_seg, SC_N_CONST);
		TRACE_ELEM_1("svjazka: ", el_fr);
		nsm_session->gen3_f_a_f(data, &da, free_seg, SC_A_CONST, el_fr);
		nsm_session->gen3_f_a_f(result, &da, res_seg, SC_A_CONST, el_fr);
		nsm_session->gen3_f_a_f(data, &da_c, free_seg, SC_A_CONST, da);
		nsm_session->gen3_f_a_f(el_fr, &da, res_seg, SC_A_CONST,  first);
		TRACE_ELEM_1("svjazka->1: ", da);
		nsm_session->gen3_f_a_f(data, &da_c, free_seg, SC_A_CONST, da);
		nsm_session->gen3_f_a_f(N1_, &da_c, res_seg, SC_A_CONST, da);
		TRACE_ELEM_1("svjazka->1_: 1: ", da_c);
		nsm_session->gen3_f_a_f(data, &el_adj, free_seg, SC_A_CONST, da_c);
		nsm_session->gen3_f_a_f(el_fr, &da, res_seg, SC_A_CONST,  second);
		TRACE_ELEM_1("svjazka->2: ", da);
		nsm_session->gen3_f_a_f(data, &da_c, free_seg, SC_A_CONST, da);
		nsm_session->gen3_f_a_f(N2_, &da_c, res_seg, SC_A_CONST, da);
		TRACE_ELEM_1("svjazka->2_: 2: ", da_c);
		nsm_session->gen3_f_a_f(data, &el_adj, free_seg, SC_A_CONST, da_c);

	} else {
		// Если это константа шаблона, то добавляем её в ответ, но только один раз!
		addToData(data, free_seg, first);

		//!--BEGIN-- RAA	2004.11.02 возможны последствия..
		//!-- RAA   2004.12.13 последствия вылезли в решателе (константа добавляется в ответ,
		//                      но соответствия для нее не строятся.		 
		if (bStoreInOne || bReturnOnly) {       // только для "внешних" запросов. !!!
			da_c = addToData(result, res_seg, first);
			if (da_c) {
				addToData(data, free_seg, da_c);
				if (bReturnOnly) {
					//	Если это запрос ТМ-а, то сохраняем все в одном...
					//	data = [
					//		result -> layout_iter: second;
					//	];
					if (!layout_map->empty()) {
						MAPSC_ADDR::const_iterator layout_iter;
						layout_iter = layout_map->find(first);
						if (layout_iter != layout_map->end()) {
							nsm_session->gen3_f_a_f(layout_iter->second, &el_adj, res_seg, SC_A_CONST, da_c);
							nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, el_adj);
						}
					}
				}
			}
		}
		//!--END--
	}
}


/**
 *	@brief	Добавляет информацию для правильного отображения в ТМ:
 *				\li	mime-type
 *				\li тип связки
 *
 *	@param	elem	Элемент, для которого добавляется информация
 *	@param	data	Узел данных почтового сообщения
 *	@param	res_seg	Результирующий сегмент (здесь неиспользован пока).
 */
void act_on_confirm_::add_ui_info(sc_addr elem, sc_addr data, sc_segment *res_seg)
{
	sc_addr arc, a, a2;
	sc_segment *free_seg=data->seg;

	// Добавляем тип содержимого...
	if (RV_OK==search_oneshot(nsm_session,
			sc_constraint_new(CONSTR_5_a_a_f_a_f, 0, 0, elem, 0, MIME_),
			true, 3, 0, &a, 1, &arc, 3, &a2))
	{
		printf("[NSM] Added MIME type (%s)", ADDR_OR_IDTF(a));
		printf(" to %s\n", ADDR_OR_IDTF(elem));

		nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, a);		/// Теоретически не надо...
		nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, elem);
		nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, arc);		
		nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, a2);		
	}

	static sc_addr ui_info[] = {
		ATTR, REL, CONN_BIN, SUBJ, TUPLE, SET
	};

	for (size_t i=0; i<sizeof(ui_info)/sizeof(sc_addr); i++) {
		if (RV_OK==search_oneshot(nsm_session,
				sc_constraint_new(CONSTR_3_f_a_f, ui_info[i], 0, elem),
				true, 1, 1, &arc))
		{
			nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, elem);
			nsm_session->gen3_f_a_f(data, 0, free_seg, SC_A_CONST, arc);
			return; ///?
		}
	}
}



sc_addr act_on_confirm_::gen_layout_pattern (sc_addr data, sc_addr layout,
											sc_segment *res_seg, sc_segment *free_seg,
											OUT MAPSC_ADDR *layout_map,
											MAPSC_ADDR &replace_map)
{
	if (!layout)
		return 0;

	TRACE_MAP_SC_ADDR(nsm_session, replace_map);
	
	sc_addr tmp;
	sc_addr tmp2;
	sc_addr res;

	if (RV_OK!=search_oneshot(nsm_session,
		sc_constraint_new(CONSTR_5_f_a_a_a_f, SCO_OBJECT, 0, 0, 0, layout),
		true, 1, 2, &res))
	{
		//fprintf (stderr, "Not found object element in markup\n");
		return 0;
	}

	// Генерируем копию markup-a.
	sc_addr markup = nsm_session->create_el(res_seg, SC_N_CONST);

	nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, markup);

	sc_iterator *iter = nsm_session->create_iterator(sc_constraint_new(
									CONSTR_3_f_a_a,	layout, 0, 0), true);
	for (; !iter->is_over(); iter->next()) {
		nsm_session->gen3_f_a_f(markup, &tmp, res_seg, SC_A_CONST, iter->value(2));
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, tmp);
		nsm_session->gen3_f_a_f(data, 0, res_seg, SC_A_CONST, iter->value(2));
	}
	delete iter;

	// Формируем карту layout_map:
	//  элемент шаблона -> множество в которое нужно включить его соответствия.
	iter = nsm_session->create_iterator(sc_constraint_new(
							CONSTR_5_f_a_a_a_f,
							SET, 0, 0, 0, layout), true);

	for (; !iter->is_over(); iter->next()) {
		tmp = iter->value(2);
		sc_iterator *iter2 = nsm_session->create_iterator(sc_constraint_new(
						CONSTR_5_f_a_a_a_f,
						tmp, 0, 0, 0, layout), true);
		for (; !iter2->is_over(); iter2->next()) {
			tmp2 = iter2->value(2);
			TRACE_ELEM(tmp2);
			TRACE_ELEM(tmp);
			MAPSC_ADDR::const_iterator i=replace_map.find(tmp2);
			if (i!=replace_map.end())
				layout_map->insert(std::make_pair(i->second, tmp));
			else
				layout_map->insert(std::make_pair(tmp2, tmp));
		}
		delete iter2;
	}
	delete iter;

	return res;
}

sc_addr act_on_confirm_::addToData(sc_addr data, sc_segment *res_seg, sc_addr elem)
{
	assert(elem);
	sc_addr arc=0;
	if (data && RV_OK!=search_oneshot(nsm_session,
		sc_constraint_new(CONSTR_3_f_a_f,
		data, 0, elem), true, 0)) 
	{
		nsm_session->gen3_f_a_f(data, &arc, res_seg, SC_A_CONST,  elem);
	}
	return arc;
}

sc_addr act_on_confirm_::genMainSynonim (sc_addr el)
{
	sc_addr rv = el;
	sc_iterator *it = nsm_session->create_iterator(sc_constraint_new(
							CONSTR_ORD_BIN_CONN1A,
							SCON_SYNONYMY,
							0,0,0,
							el,
							0,0,
							SCON_MAIN_,
							0), true);

	for (; !it->is_over(); it->next()) {
		if (rv!=el) {
			printf("[NSM::getMainSynonim] Error elem (%s)"
					"has more than one main synonym !!!\n", 
					(char*)nsm_session->get_idtf(nsm_session->get_beg(el)).c_str());
		}
		rv = it->value(6);
	}
	delete it;

	return rv;
}
