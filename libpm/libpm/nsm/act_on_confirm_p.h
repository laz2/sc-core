
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
 * @file act_on_confirm.h 
 * @author Alexey Rukin
 * @brief Описание активностИ, которая выполняется при наличии активных запросов на изоморфный поиск.
 */

#ifndef __NSM_ISOM_ACTIVITY_ON_CONFIRM_H__
#define __NSM_ISOM_ACTIVITY_ON_CONFIRM_H__

#define OUT

/** @def Максимальное кол-во шагов изоморфного поиска за квант времени. */
#define ISOM_STEPS_PERQUANT 30 

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif


/// Карта соответствий sc элементов.
typedef std::map<sc_addr, sc_addr>	MAPSC_ADDR;

/**
* @brief Находит ключ в хэше по значению
*/
template<class InIt, class T> InIt find_key_by_value(InIt first, InIt last, const T&val)
{
	for (; first!=last; first++)
		if (first->second == val)
			break;
	return first;
}

/**
* @brief Находит ключ в хэше по одному из двух значений.
*/
template<class InIt, class T> InIt find_key_by_value(InIt first, InIt last, const T&val, const T&val2)
{
	for (; first!=last; first++)
		if ((first->second == val) || (first->second == val2))
			break;
	return first;
}

/**
 * @brief Описывает вариант поиска.
 */
struct SIsomVariant {
	LISTSC_ADDR	front;			//!< Фронт(эл-ты, опираясь на которые ведется поиск).
	LISTSC_ADDR	analArcs;		//!< Дуги, к-е нужно найти.
	MAPSC_ADDR	corr;			//!< Соответствия.

	/** @brief Конструктор копирования. */
	SIsomVariant (const SIsomVariant &iv) {
		front = iv.front;
		analArcs = iv.analArcs;
		corr = iv.corr;
	}

	SIsomVariant () {}; //!< Конструктор.
};

/**
* Запрос на изоморфный поиск.
*/
struct SIsomQuery {
	LISTSC_ADDR	patt;				// Шаблон поиска.
	std::list<SIsomVariant>	variants;		// Множество вариантов.
	std::list<MAPSC_ADDR>	result; 		// Множество результатов.
	LISTSC_ADDR	not_returned_elems;			// Множество элементов, которые НЕ нужно 
											//включать в результирующее множество
	LISTSC_ADDR	returned_elems;			// Множество элементов, которые НУЖНО 
										//включать в результирующее множество
	MAPSC_ADDR replace_map;				// Карта подмененных элементов.

	sc_addr arc_for_gen_confirmed_; 	// Дуга, в которую проводится дуга из confirmed_

	sc_addr comment;					// Коментарии к команде.
	sc_addr show_content;				// Показать содержимое узлов.

	sc_addr layout;						// Разметка.

	bool bAddUiInfo;					// добавить информацию для пользовательского интерфейса
	bool bOnlyOne;						//  Достаточно одного найденного варианта.
	bool bStoreInOne;					// 	Результат - в одно множество.
	bool bReturnOnly;					//  Результирующее множество не создается. 
										//Результат запаковывается и отсылается обратно.


	std::set<sc_segment*> search_in_seg;          // Искать тока в этом сегименте.

	/*
	 *	@brief Copy constructor.
	 */
	SIsomQuery (const SIsomQuery& iq) {
		patt		= iq.patt;
		variants	= iq.variants;
		arc_for_gen_confirmed_	= iq.arc_for_gen_confirmed_;
		comment		= iq.comment;
		show_content	= iq.show_content;
		layout		= iq.layout;
		bOnlyOne	= iq.bOnlyOne;
		bStoreInOne	= iq.bStoreInOne;
		bReturnOnly	= iq.bReturnOnly;
		not_returned_elems	= iq.not_returned_elems;
		returned_elems	= iq.returned_elems;
		replace_map = iq.replace_map;
		bAddUiInfo	= iq.bAddUiInfo;
		search_in_seg	= iq.search_in_seg;
	}

	/*
	 *	@brief Default constructor.
	 */
	SIsomQuery () {
		arc_for_gen_confirmed_ = NULL;
		comment	= 0;
		show_content	= 0;
		layout		= 0;
		bOnlyOne	= false;
		bStoreInOne	= false;
		bReturnOnly	= false;
		bAddUiInfo	= false;
		//search_in_seg	= NULL;           // search in ALL segments by default.
	}
};



struct Sact_goals_msg 
{
	sc_addr active_goal;

	sc_segment *res_seg;

	Sact_goals_msg (const sc_addr ag, sc_segment *seg):
				active_goal(ag), res_seg(seg){};

	Sact_goals_msg (const Sact_goals_msg &msg){
		active_goal = msg.active_goal;
		res_seg = msg.res_seg;
	}

	inline sc_segment* getResSeg() {
		if (!res_seg) {
			res_seg = nsm_session->get_end(active_goal)->seg;
		}

		return res_seg;
	}
};

/***
* Данный агент активизируется при наличии активных запросов 
* на изоморфный поиск.
*/
class act_on_confirm_ : public sc_activity {
	std::list<SIsomQuery>::iterator cur_query;	// текущий запрос.

public:
	std::list<SIsomQuery> queries;		// множество активных запросов.
	sc_retval init(sc_addr _this) {cur_query = queries.begin(); return RV_OK;};
	void done() {delete this;};

	static
	sc_addr get_adj(sc_addr arc, sc_addr el);

	/**
	 *	@brief Ищет дугу, которая входит в элемент elem.
	 */
	static
	sc_addr search_in_arc(LISTSC_ADDR set_of_arc, sc_addr elem);

	/**
	 *	@brief Ищет дугу, которая выходит из элемента elem.
	 */
	static
	sc_addr search_out_arc(LISTSC_ADDR set_of_arc, sc_addr elem);


	/**
	 *	@brief Ищет входящую или выходящую дугу.
	 */
	static
	sc_addr search_in_or_out_arc(LISTSC_ADDR set_of_arc, sc_addr elem);

	/**
	* @brief Добавляет запрос к текущему множеству.
	* @param queries	- Множество запросов.
	* @param arc_for_gen_confirmed_ - Дуга, в которую проводится дуга из confirmed_
	* @param coment	- В содержимом данного узла валяется коментарий к команде.
	* @param show_content - Множество элементов, для которых нужно отображать содержимое.
	* @param layout	- Разметка
	* @param ret_elms	- Множество элементов, соответствия которым нужно вернуть(исключить)
	*        в(из) результирующего множества.
	* @param replace_map Карта подмененных элементов.
	* @param bRet - Индикатор того, что в ret_elms содержится множество элементов, 
	*        соответствия которым нужно вернуть(true) или исключить(false)
	*        в(из) результирующего множества.
	*/
	static
	sc_retval /* act_on_confirm_:: */add_query(std::list<SIsomQuery> &queries,
								sc_addr arc_for_gen_confirmed_,
								sc_addr coment,
								sc_addr show_content,
								sc_addr layout,
								LISTSC_ADDR	&ret_elms,
								MAPSC_ADDR  &replace_map,
								bool bRet);


	/**
	* @brief проверяет подходит ли элемент elem под шаблонный элемент pattern.
	* @param q - Запрос в рамках которого осуществляется поиск.
	* @param pattern - шаблон
	* @param elem - предполагаемое соответствие.
	* @return true - соответствие возможно
	*/
	static
	bool can_corresponding(const SIsomQuery &q, sc_addr pattern, sc_addr elem);

	/// Check if type of @p eleme correspond pattern type of @p pattern.
	static bool is_corresponding_type(sc_addr pattern, sc_addr elem);

	/**
	* @brief удаляет элемент elem из фронта, если к нему нет больше обращений
	* @param variant - вариант поиска.
	* @param elem  - кандидат на исключение из множества front.
	*/
	inline static void clear_front
		(
			const std::list<SIsomVariant>::iterator &variant,
			sc_addr elem
		)
	{
		if (!search_in_or_out_arc(variant->analArcs, elem))
			variant->front.remove(elem);
	}

	/**
	* @brief добавляет элемент elem во фронт, если в него входят или выходят
	* 		 дуги из analArcs.
	* @param variant - вариант поиска.
	* @param elem  - кандидат на добавление во множество front.
	*/
	inline static void add_front(const std::list<SIsomVariant>::iterator &variant, sc_addr elem)
	{
		if (search_in_or_out_arc(variant->analArcs,elem))
//			if (find(variant->front.begin(), variant->front.end(), elem)==variant->front.end())
				variant->front.push_back(elem);
	}


	bool must_included_in_result(sc_addr elem, SIsomQuery &q);

	/**
	* @param node - узел, для которого ищем.
	* @param bIn - true - кол-во входящих дуг
	*				false - выходящих
	*/
	inline int get_arc_count(sc_addr node, bool bIn)
	{
		return bIn?nsm_session->get_in_qnt(node):nsm_session->get_out_qnt(node);
	}


	inline bool isInArc(sc_addr node, sc_addr arc)
	{
		return (nsm_session->get_beg(arc)==node);
	}
	/**
	* @brief Шаг изоморфного поиска.
	*/
	sc_retval activate(sc_session *,sc_addr _this,sc_addr param1,sc_addr,sc_addr);

	static
	void end_isom(std::list<SIsomQuery>::iterator, sc_addr data, sc_segment *res_seg, int i);

	void create_report(sc_addr data, sc_segment *res_seg, int &i);

	static int isom_do_step(const SIsomQuery &q,
							std::list<SIsomVariant> &variants,
							const std::list<SIsomVariant>::iterator &variant,
							std::list<MAPSC_ADDR> &result,
							bool bOnlyOne);

	/**
	* @brief сохраняет соответствие в результате, учитывая опции...
	*/
	inline void store_result(sc_segment *res_seg,
					MAPSC_ADDR *layout_map,
					bool bStoreInOne, bool bReturnOnly,
					sc_addr show_content, sc_addr res_show_content,
					sc_addr result,   sc_addr data,
					sc_addr first,    sc_addr second);

	void add_ui_info(sc_addr data, sc_addr, sc_segment *res_seg);

	static
	sc_addr gen_layout_pattern (sc_addr data, sc_addr layout,
								sc_segment *res_seg, sc_segment *free_seg,
								OUT MAPSC_ADDR *layout_map,
								MAPSC_ADDR &replace_map);

	/**
	* @brief Добавляет в почтовое сообщение элемент elem только один раз.
	* @param data	- множество, в которое добавляется элемент elem
	* @param res_seg	- сегмент, в котором генерировать дугу
	* @param elem	- элемент, который нужно добавить в множество.
	* @return arc - сгенерированная дуга.
	*/
	static	sc_addr addToData(sc_addr data, sc_segment *res_seg, sc_addr elem);

	static	sc_addr genMainSynonim (sc_addr el);
};


extern act_on_confirm_ *activity_on_confirm_;

#endif //__NSM_ISOM_ACTIVITY_ON_CONFIRM_H__
