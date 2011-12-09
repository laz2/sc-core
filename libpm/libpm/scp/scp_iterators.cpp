
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
 * @file scp_iterators.cpp
 *
 * Реализация scp-итераторов по различным структурам.
 **/

#include "precompiled_p.h"

#include <libsc/sc_filter.h>

/**
 * Класс итератора, метод is_over которого всегда возвращает истину.
 **/
class	f2_s5_iterator: public sc_iterator
{
private:
	sc_iterator *iter;
	bool sink;
	sc_addr e1, e3;

public:

	f2_s5_iterator(sc_addr _e1, sc_addr _e3, sc_iterator *_iter, bool _sink)
	{
		iter = _iter;
		sink = _sink;
		e1 = _e1;
		e3 = _e3;
	}

	~f2_s5_iterator()
	{
		if (sink) {
			delete iter;
		}
	}

	sc_retval next()
	{
		return iter->next();
	}

	bool	is_over()
	{
		return iter->is_over();
	}

	sc_addr value(sc_uint num)
	{
		if (num == 0) {
			return e1;
		} else if (num == 1) {
			return iter->value(2);
		} else if (num == 2) {
			return e3;
		}

		return iter->value(4 - num);
	}
};

/**
 * Класс итератора, который итерирует по одной пяти элементной конструкции.
 **/
class onetuple5_iterator: public sc_iterator
{
private:
	sc_addr elems[5];
	bool over;

public:

	/**
	 * @param _e1 первый элемент пяти элеметной конструкции.
	 * @param _e2 второй элемент пяти элеметной конструкции.
	 * @param _e3 третий элемент пяти элеметной конструкции.
	 * @param _e4 четвертый элемент пяти элеметной конструкции.
	 * @param _e5 пятый элемент пяти элеметной конструкции.
	 */
	onetuple5_iterator(sc_addr _e1, sc_addr _e2, sc_addr _e3, sc_addr _e4, sc_addr _e5)
	{
		elems[0] = _e1;
		elems[1] = _e2;
		elems[2] = _e3;
		elems[3] = _e4;
		elems[4] = _e5;
		over = false;
	}

	~onetuple5_iterator() {}
	
	sc_retval next()
	{
		if (over) {
			return RV_ELSE_GEN;
		}

		over = true;
		return RV_OK;
	}

	sc_addr value(sc_uint num)
	{
		if (num > 4) {
			return NULL;
		}

		return elems[num];
	}

	bool is_over() 
	{
		return over;
	}
};

inline sc_retval indexate(scp_process_info *info, scp_opnd_info &opnd, sc_param params[], int __index, int &index, int ind_value)
{
	sc_session *s = info->session;

	if (opnd.fixed) {
		index |= ind_value;
		
		if (opnd.value->seg != SC_SET_SEG && !s->is_segment_opened(opnd.value->seg)) {
			SCP_ERROR(info->process, "Value of operand " << opnd.index << "_: '" << SC_ID(opnd.opnd) << "' is not accessible!");
			return RV_ERR_GEN;
		}

		params[__index].addr = opnd.value;
	} else {
		params[__index].type = opnd.type_mask;
	}

	return RV_OK;
}

/**
 * @param opnd_info операнд.
 * @param __index индекс параметра для поиска.
 * @param ind_value 
 */
#define INDEXATE(opnd_info, __index, ind_value) \
	if(indexate(info, opnd_info, params, __index, index, ind_value)) { \
		rv = RV_ERR_GEN; \
		return 0; \
	}

sc_iterator *create_search3_iterator(
	scp_process_info *info,
	scp_opnd_info &opnd1,
	scp_opnd_info &opnd2,
	scp_opnd_info &opnd3,
	sc_single_filter *f1,
	sc_single_filter *f2,
	sc_single_filter *f3,
	sc_retval &rv) 
{
	sc_session *session = info->session;
	rv = RV_OK;
	opnd2.type_mask |= SC_ARC_ACCESSORY;

	sc_iterator *iter;
	if (opnd2.fixed) {
		iter = session->create_iterator(sc_constraint_new(
					CONSTR_3_d_f_d,
					opnd1.value,
					opnd1.fixed,
					opnd1.type_mask,
					opnd2.value,
					opnd2.type_mask,
					opnd3.value,
					opnd3.fixed,
					opnd3.type_mask
				), true);
	} else {
		static int dummy[] = {0, 1, 2};
		static sc_uint *constrs[] = {
			0,	/* aaa */
			&CONSTR_3_f_a_a,
			&CONSTR_3_a_a_f,
			&CONSTR_3_f_a_f
		};
		int index = 0; // индекс constraint'a в constrs
		sc_param params[3]; // параметры поиска

		params[1].type = opnd2.type_mask;
		INDEXATE(opnd1, 0, 1);
		INDEXATE(opnd3, 2, 2);
		
		if (!index) {
			rv = RV_ERR_GEN;
			return 0;
		}

		iter = session->create_iterator(sc_constraint_newv(constrs[index][0], params, 3, dummy), true);
	}
	
	sc_iterator_complex_filter<3> *filter = new sc_iterator_complex_filter<3>(iter, true);
	filter->set_filter(0, f1);
	filter->set_filter(1, f2);
	filter->set_filter(2, f3);

	sc_match_filter<3> *match = new sc_match_filter<3>(filter, true);

	// В приципе поиск по таким операндам не имеет смысла.
	if (opnd1.assign && opnd2.assign && opnd1.opnd == opnd2.opnd) {
		match->require_match(0, 1);
	}
	
	// ищем узел с рефликсивной дугой
	if (opnd1.assign && opnd3.assign && opnd1.opnd == opnd3.opnd) {
		match->require_match(0, 2);
	}

	// ищем саморефлексивную дугу
	if (opnd2.assign && opnd3.assign && opnd2.opnd == opnd3.opnd) {
		match->require_match(1, 2);
	}

	match->begin();
	
	return match;
}

// very ugly
sc_iterator *create_search5_iterator(
	scp_process_info *info,
	scp_opnd_info &i1,
	scp_opnd_info &i2,
	scp_opnd_info &i3,
	scp_opnd_info &i4,
	scp_opnd_info &i5,
	sc_single_filter *f1,
	sc_single_filter *f2,
	sc_single_filter *f3,
	sc_single_filter *f4,
	sc_single_filter *f5,
	sc_retval &rv
) {
	sc_session *session = info->session;
	sc_iterator *iter;
	sc_iterator_complex_filter<5> *filter;
	sc_match_filter<5> *match;
	static int dummy[] = {0,1,2,3,4};
	static sc_uint *constrs[] = {
		0,	/* aaaaa */
		&CONSTR_5_f_a_a_a_a,
		&CONSTR_5_a_a_f_a_a,
		&CONSTR_5_f_a_f_a_a,
		&CONSTR_5_a_a_a_a_f,
		&CONSTR_5_f_a_a_a_f,
		&CONSTR_5_a_a_f_a_f,
		&CONSTR_5_f_a_f_a_f
	};
	int index = 0;
	sc_param params[5];

	rv = RV_OK;
	i2.type_mask |= SC_ARC_ACCESSORY;
	i4.type_mask |= SC_ARC_ACCESSORY;
	if (i2.fixed) {
		if (i2.set) {
			dprintf("5 element search with set_ on operand 2 is not supported. Sorry.\n");
			return 0;
		}
		sc_addr e1,e3;
		iter = create_search3_iterator(info,i5,i4,i2,f5,f4,f2,rv);
		if (rv)
			return 0;
		f5 = f4 = f2 = 0;
		if (search_oneshot(session,sc_constraint_new(
					CONSTR_3_d_f_d,
					i1.value,
					i1.fixed,
					i1.type_mask,
					i2.value,
					i2.type_mask,
					i3.value,
					i3.fixed,
					i3.type_mask
				),true,2,0,&e1,2,&e3)) {
			delete iter;
			return new over_iterator;
		}
		iter = new f2_s5_iterator(e1, e3, iter, true);
	} else if (i4.fixed) {
		if (i4.set) {
			dprintf("5 element search with set_ on operand 4 is not supported. Sorry.\n");
		}
		// very not optimal
		sc_addr e1,e2,e3,e5;
		if (search_oneshot(session,sc_constraint_new(
					CONSTR_3_d_f_d,
					i5.value,
					i5.fixed,
					i5.type_mask,
					i4.value,
					i4.type_mask,
					i2.value,
					i2.fixed,
					i2.type_mask
				),true,2,0,&e5,2,&e2)) {
			printf("%s", SC_IDc(session->get_beg(i4.value)));
			rv = RV_ERR_GEN;
			return 0;
		}
		if (search_oneshot(session,sc_constraint_new(
					CONSTR_3_d_f_d,
					i1.value,
					i1.fixed,
					i1.type_mask,
					e2,
					i2.type_mask,
					i3.value,
					i3.fixed,
					i3.type_mask
				),true,2,0,&e1,2,&e3)) {
			rv = RV_ERR_GEN;
			return 0;
		}
		iter = new onetuple5_iterator(e1,e2,e3,i4.value,e5);
	} else {
		params[1].type = i2.type_mask;
		params[3].type = i4.type_mask;
		INDEXATE(i1,0,1);
		INDEXATE(i3,2,2);
		INDEXATE(i5,4,4);
		if (!index) {
			rv = RV_ERR_GEN;
			return 0;
		}
		iter = session->create_iterator(sc_constraint_newv(constrs[index][0],params,5,dummy),true);
	}
	filter = new sc_iterator_complex_filter<5>(iter,true);
	filter->set_filter(0,f1);
	filter->set_filter(1,f2);
	filter->set_filter(2,f3);
	filter->set_filter(3,f4);
	filter->set_filter(4,f5);
	match = new sc_match_filter<5>(filter,true);
	if (i1.assign && i2.assign && i1.opnd == i2.opnd) // this cannot actually happen
		match->require_match(0,1);
	if (i1.assign && i3.assign && i1.opnd == i3.opnd)
		match->require_match(0,2);
	if (i1.assign && i4.assign && i1.opnd == i4.opnd) // this cannot actually happen
		match->require_match(0,3);
	if (i1.assign && i5.assign && i1.opnd == i5.opnd)
		match->require_match(0,4);
	if (i2.assign && i5.assign && i2.opnd == i3.opnd)
		match->require_match(1,2);
	if (i2.assign && i4.assign && i2.opnd == i4.opnd)
		match->require_match(1,3);
	if (i2.assign && i4.assign && i2.opnd == i5.opnd)
		match->require_match(1,4);
	if (i3.assign && i4.assign && i3.opnd == i4.opnd)
		match->require_match(2,3);
	if (i3.assign && i5.assign && i3.opnd == i5.opnd)
		match->require_match(2,4);
	if (i4.assign && i5.assign && i4.opnd == i5.opnd)
		match->require_match(3,4);
	match->begin();
	return match;
}
