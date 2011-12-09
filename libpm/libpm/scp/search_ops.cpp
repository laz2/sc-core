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
 * @file search_ops.cpp
 * @brief Implementation SCP-operators for elemntary searching SC-constructions.
 **/

#include "precompiled_p.h"

#include "scp_iterators_p.h"

/**
 * @brief Реализация scp-оператора searchElByIdtf поиска элемента по его идентификатору.
 * @code
 * searchElByIdtf -> {
 *   1_: (assign_: | fixed_) result,
 *   2_: fixed_: idtf, // элемент со строковым содержимым
 *   (then_ | goto_): then_operator, // следующий оператор в случае успешного поиска
 *   else_: else_operator // следующий оператор в случае безуспешного поиска
 * }
 * @endcode
 */
SCP_OPERATOR_IMPL(scp_oper_searchElByIdtf)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, result_opnd);
	if (!result_opnd.var || result_opnd.fixed) {
		return RV_ERR_GEN;
	}

	SCP_OPND_REQ_FIXED(info, 2, idtf_opnd);

	sc_segment *segment;
	if (RV_IS_ERR(get_single_segment(info, 1, &segment))) {
		return RV_ERR_GEN;
	}

	Content cont = info->session->get_content(idtf_opnd.value);
	if (cont.type() != TCSTRING) {
		return RV_ERR_GEN;
	}

	sc_addr result = info->session->find_by_idtf(Cont(cont).d.ptr, segment);

	if (result_opnd.assign) {
		if (!result) {
			scp_clear_variable(info, result_opnd.opnd);
		} else {
			scp_set_variable(info, result_opnd.opnd, result);
		}
	}

	if (result) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}
	
	return RV_OK;
}

#define ASSIGN_VALUE(opinfo, iter, index) \
	{ \
		if (opinfo.assign) \
			if (scp_set_variable(info, opinfo.opnd, iter->value(index))) \
				return RV_ERR_GEN; \
			if (!info->session->is_segment_opened(iter->value(index)->seg)) \
				SC_ABORT(); \
	}

SCP_OPERATOR_IMPL(scp_oper_searchElStr3)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_MULTI_SEGC_FILTER_OPT(info, 1, f1);
	SCP_MULTI_SEGC_FILTER_OPT(info, 2, f2);
	SCP_MULTI_SEGC_FILTER_OPT(info, 3, f3);

	SCP_CHECK_OPND_EXCESSIVE(info, N4_);
	SCP_CHECK_OPND_EXCESSIVE(info, N5_);

	sc_retval rv;
	sc_iterator *iter = create_search3_iterator(info, opnd1, opnd2, opnd3, f1, f2, f3, rv);

	if (!iter)
		return rv;

	SC_ITERATOR_PTR_DELETER(iter);

	if (iter->is_over()) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		ASSIGN_VALUE(opnd1, iter, 0);
		ASSIGN_VALUE(opnd2, iter, 1);
		ASSIGN_VALUE(opnd3, iter, 2);

		scp_set_state(info->process, SCP_STATE_THEN);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_searchElStr5)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 4, opnd4);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 5, opnd5);
	SCP_MULTI_SEGC_FILTER_OPT(info, 1, f1);
	SCP_MULTI_SEGC_FILTER_OPT(info, 2, f2);
	SCP_MULTI_SEGC_FILTER_OPT(info, 3, f3);
	SCP_MULTI_SEGC_FILTER_OPT(info, 4, f4);
	SCP_MULTI_SEGC_FILTER_OPT(info, 5, f5);

	sc_retval rv;
	sc_iterator *iter = create_search5_iterator(info, opnd1, opnd2, opnd3, opnd4, opnd5, f1, f2, f3, f4, f5, rv);

	if (!iter)
		return rv;

	SC_ITERATOR_PTR_DELETER(iter);

	if (iter->is_over()) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		ASSIGN_VALUE(opnd1, iter, 0);
		ASSIGN_VALUE(opnd2, iter, 1);
		ASSIGN_VALUE(opnd3, iter, 2);
		ASSIGN_VALUE(opnd4, iter, 3);
		ASSIGN_VALUE(opnd5, iter, 4);

		scp_set_state(info->process, SCP_STATE_THEN);
	}

	return RV_OK;
}

/**
 * @brief Включает элемент в множество.
 * @param *s sc_session.
 * @param set знак множества, в которое будет производиться включение.
 * @param el элемент для включения.
 * @param segsign адрес сегмента.
 */
sc_retval include_in_set(sc_session *s, sc_addr set, sc_addr el, sc_addr segsign)
{
	sc_segment *seg = s->find_segment(s->sign2uri(segsign));
	
	if (!seg) {
		return RV_ERR_GEN;
	}

	return s->gen3_f_a_f(set, 0, seg, SC_A_CONST|SC_POS, el);
}

/**
 * @brief Удаляет значение set-операнда в searchSet* операторах, если для операнда указан атрибут assign_.
 * @param *process_info информация о scp-процессе.
 * @param &set_opnd set-операнд.
 */
inline void remove_set_if_created(scp_process_info *process_info, scp_opnd_info &set_opnd)
{
	if (set_opnd.assign) {
		process_info->session->erase_el(set_opnd.value);
	}
}

#define CHECK_OPND_AND_CORRESP_SET(opndIndex) \
	if (opnd##opndIndex.value && set_opnd##opndIndex.value) { \
		SCP_ERROR(info->process, "Error while operator has operand " << opndIndex << "_ and corresponding operand set" << opndIndex << "_"); \
		return RV_ERR_GEN; \
	}

SCP_OPERATOR_IMPL(scp_oper_searchSetStr3)
{
	sc_session *session = info->session;
	pm_transaction *t = new pm_transaction(session);
	info->session = t;

	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 1, set_opnd1);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 2, set_opnd2);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 3, set_opnd3);

	SCP_CHECK_OPND_EXCESSIVE(info, N4_);
	SCP_CHECK_OPND_EXCESSIVE(info, N5_);
	SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[4]);
	SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[5]);

	// Передан fixed_ операнд и соответсвующее ему множество.
	// В данной реализации расценимаем это как ошибку.
	CHECK_OPND_AND_CORRESP_SET(1);
	CHECK_OPND_AND_CORRESP_SET(2);
	CHECK_OPND_AND_CORRESP_SET(3);

	SCP_MULTI_SEGC_FILTER_OPT(info, 1, f1);
	SCP_MULTI_SEGC_FILTER_OPT(info, 2, f2);
	SCP_MULTI_SEGC_FILTER_OPT(info, 3, f3);

	bool is_result_present = false;

	if (set_opnd1.value || set_opnd2.value || set_opnd3.value) {
		sc_retval rv;
		sc_iterator *iter = create_search3_iterator(info, opnd1, opnd2, opnd3, f1, f2, f3, rv);
		if (!iter)
			return rv;

		if (!iter->is_over()) {
			is_result_present = true;

			sc_for_each (iter) {
				// Добавляем найденные элементы в соответствующие множества.
				if ((set_opnd1.value && include_in_set(info->session, set_opnd1.value, iter->value(0), info->defaultseg_sign)) ||
					(set_opnd2.value && include_in_set(info->session, set_opnd2.value, iter->value(1), info->defaultseg_sign)) ||
					(set_opnd3.value && include_in_set(info->session, set_opnd3.value, iter->value(2), info->defaultseg_sign))) {
					return RV_ERR_GEN;
				}
			}
		}
	}

	if (!is_result_present) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else { 
		t->commit();
		scp_set_state(info->process, SCP_STATE_THEN);
	}

	info->session = session;

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_searchSetStr5)
{
	sc_session *session = info->session;
	pm_transaction *t = new pm_transaction(session);
	info->session = t;

	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 4, opnd4);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 5, opnd5);

	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 1, set_opnd1);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 2, set_opnd2);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 3, set_opnd3);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 4, set_opnd4);
	SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(info, 5, set_opnd5);

	// Передан fixed_ операнд и соответсвующее ему множество.
	// В данной реализации расценимаем это как ошибку.
	CHECK_OPND_AND_CORRESP_SET(1);
	CHECK_OPND_AND_CORRESP_SET(2);
	CHECK_OPND_AND_CORRESP_SET(3);
	CHECK_OPND_AND_CORRESP_SET(4);
	CHECK_OPND_AND_CORRESP_SET(5);

	SCP_MULTI_SEGC_FILTER_OPT(info, 1, f1);
	SCP_MULTI_SEGC_FILTER_OPT(info, 2, f2);
	SCP_MULTI_SEGC_FILTER_OPT(info, 3, f3);
	SCP_MULTI_SEGC_FILTER_OPT(info, 4, f4);
	SCP_MULTI_SEGC_FILTER_OPT(info, 5, f5);

	bool is_result_present = false;

	if (set_opnd1.value || set_opnd2.value || set_opnd3.value || set_opnd4.value || set_opnd5.value) {
		sc_retval rv;
		sc_iterator *iter = create_search5_iterator(info, opnd1, opnd2, opnd3, opnd4, opnd5, f1, f2, f3, f4, f5, rv);
		if (!iter)
			return rv;

		if (!iter->is_over()) {
			is_result_present = true;

			sc_for_each (iter) {
				// Добавляем найденные элементы в соответствующие множества.
				if ((set_opnd1.value && include_in_set(info->session,set_opnd1.value,iter->value(0),info->defaultseg_sign)) ||
					(set_opnd2.value && include_in_set(info->session,set_opnd2.value,iter->value(1),info->defaultseg_sign)) ||
					(set_opnd3.value && include_in_set(info->session,set_opnd3.value,iter->value(2),info->defaultseg_sign)) ||
					(set_opnd4.value && include_in_set(info->session,set_opnd4.value,iter->value(3),info->defaultseg_sign)) ||
					(set_opnd5.value && include_in_set(info->session,set_opnd5.value,iter->value(4),info->defaultseg_sign))) {
					return RV_ERR_GEN;
				}
			}
		}
	}

	if (!is_result_present) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		scp_set_state(info->process, SCP_STATE_THEN);
		t->commit();
	}

	info->session = session;

	return RV_OK;
}

