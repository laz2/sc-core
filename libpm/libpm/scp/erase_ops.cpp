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
 * @file erase_ops.cpp
 * @brief Implementation SCP-operators for erasing SC-constructions.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

#include "scp_iterators_p.h"

bool check_F_(scp_opnd_info *opnd_info)
{
	return !search_3_f_cpa_f(system_session, F_, 0, opnd_info->opnd_arc);
}

SCP_OPERATOR_IMPL(scp_oper_eraseEl)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	scp_clear_this_value_variables(info->var_value, opnd1.value);
	info->session->erase_el(opnd1.value);

	return RV_OK;
}

/// If opnd arc has attr f_, then erase opnd value. Opnd must have value.
#define ERASE_VALUE(opinfo) \
	if (check_F_(&opinfo) && !opinfo.value->is_dead()) { \
		scp_clear_this_value_variables(info->var_value, opinfo.value); \
		info->session->erase_el(opinfo.value); \
	}

SCP_OPERATOR_IMPL(scp_oper_eraseElStr3)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_MULTI_SEGC_FILTER_OPT(info, 1, f1);
	SCP_MULTI_SEGC_FILTER_OPT(info, 2, f2);
	SCP_MULTI_SEGC_FILTER_OPT(info, 3, f3);

	sc_retval rv;
	auto_sc_iterator_ptr iter(create_search3_iterator(info, opnd1, opnd2, opnd3, f1, f2, f3, rv));

	if (rv)
		return RV_ERR_GEN;

	if (iter->is_over()) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		// For ERASE_VALUE
		opnd1.value = iter->value(0);
		opnd2.value = iter->value(1);
		opnd3.value = iter->value(2);

		ERASE_VALUE(opnd2);
		ERASE_VALUE(opnd1);
		ERASE_VALUE(opnd3);
		
		scp_set_state(info->process, SCP_STATE_THEN);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_eraseElStr5)
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
	auto_sc_iterator_ptr iter(create_search5_iterator(info, opnd1, opnd2, opnd3, opnd4, opnd5, f1, f2, f3, f4, f5, rv));
	
	if (rv)
		return RV_ERR_GEN;

	if (iter->is_over()) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		// For ERASE_VALUE
		opnd1.value = iter->value(0);
		opnd2.value = iter->value(1);
		opnd3.value = iter->value(2);
		opnd4.value = iter->value(3);
		opnd5.value = iter->value(4);

		ERASE_VALUE(opnd4);
		ERASE_VALUE(opnd2);
		ERASE_VALUE(opnd1);
		ERASE_VALUE(opnd3);
		ERASE_VALUE(opnd5);

		scp_set_state(info->process, SCP_STATE_THEN);
	}

	return RV_OK;
}

/// If opnd's arc has attr f_, then set flag concatenated "need_remove_" and @p opinfo name as true, else as false.
#define NEED_REMOVE(opinfo) \
	bool need_remove_##opinfo = false; \
	if (check_F_(&opinfo)) { \
		need_remove_##opinfo = true; \
	}

SCP_OPERATOR_IMPL(scp_oper_eraseSetStr3)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_MULTI_SEGC_FILTER_OPT(info, 1, f1);
	SCP_MULTI_SEGC_FILTER_OPT(info, 2, f2);
	SCP_MULTI_SEGC_FILTER_OPT(info, 3, f3);

	sc_retval rv;
	auto_sc_iterator_ptr iter(create_search3_iterator(info, opnd1, opnd2, opnd3, f1, f2, f3, rv));

	if (rv)
		return RV_ERR_GEN;

	NEED_REMOVE(opnd1);
	NEED_REMOVE(opnd2);
	NEED_REMOVE(opnd3);

	if (need_remove_opnd1 && opnd1.fixed) {
		opnd1.value->ref();
	}

	if (need_remove_opnd2 && opnd2.fixed) {
		opnd2.value->ref();
	}

	if (need_remove_opnd3 && opnd3.fixed) {
		opnd3.value->ref();
	}

	bool is_result_present = false;
	if (!iter->is_over()) {
		is_result_present = true;
	}

	for (; !iter->is_over(); iter->next()) {
		sc_addr erased_element;

		if (need_remove_opnd2 && !opnd2.fixed && (erased_element = iter->value(1))) {
			scp_clear_this_value_variables(info->var_value, erased_element);
			info->session->erase_el(erased_element);
		}

		if (need_remove_opnd1 && !opnd1.fixed && (erased_element = iter->value(0))) {
			scp_clear_this_value_variables(info->var_value, erased_element);
			info->session->erase_el(erased_element);
		}

		if (need_remove_opnd3 && !opnd3.fixed && (erased_element = iter->value(2))) {
			scp_clear_this_value_variables(info->var_value, erased_element);
			info->session->erase_el(erased_element);
		}
	}

	if (is_result_present || (opnd1.fixed && opnd2.fixed && opnd3.fixed)) {
		if (need_remove_opnd1 && opnd1.fixed) {
			info->session->erase_el(opnd1.value);
			opnd1.value->unref();
		}

		if (need_remove_opnd2 && opnd2.fixed) {
			info->session->erase_el(opnd2.value);
			opnd2.value->unref();
		}

		if (need_remove_opnd3 && opnd3.fixed) {
			info->session->erase_el(opnd3.value);
			opnd3.value->unref();
		}
	}

	if (is_result_present) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);	
	}
	
	return RV_OK;
}

