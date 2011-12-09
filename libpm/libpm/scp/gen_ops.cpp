
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
 * @file gen_ops.cpp
 * @brief Implementation SCP-operators for generating SC-constructions.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

SCP_OPERATOR_IMPL(scp_oper_genEl)
{
	SCP_OPND_REQ_ASSIGN(info, 1, opnd1);

	// Если не указан класс элемента, то создаем узел.
	if (!(opnd1.type_mask & SC_SYNTACTIC_MASK)) {
		opnd1.type_mask |= SC_NODE;
	}

	return opnd1.create_element();
}

/**
 * @brief Обрабатывает случай, когда второй элемент из тройки уже создан. 
 * @note При необходимости преобразовывает элемент неопределенного типа в дугу.
 *
 * @param *info информация о scp-процессе.
 * @param &opnd1 операнд, который представляет первый элемент тройки.
 * @param &opnd2 операнд, который представляет второй элемент тройки.
 * @param &opnd3 операнд, который представляет третий элемент тройки.
 */
sc_retval handle_fixed_arc(scp_process_info *info, scp_opnd_info &opnd1, scp_opnd_info &opnd2, scp_opnd_info &opnd3)
{
	static const sc_type MASK = SC_FUZZYNESS_MASK|SC_PERMANENCY_MASK;

	sc_type type = info->session->get_type(opnd2.value);
	if ((type & SC_SYNTACTIC_MASK) == SC_UNDF) {
		// элемент неопределенного типа преобразуем в дугу
		opnd2.type_mask = sc_type_add_default(opnd2.type_mask, SC_FUZZYNESS_MASK, SC_FUZ);
		opnd2.type_mask = sc_type_add_default(opnd2.type_mask, SC_PERMANENCY_MASK, SC_PERMANENT);
		type = (type & ~(SC_SYNTACTIC_MASK | MASK)) | SC_ARC_MAIN | (opnd2.type_mask & MASK);
		info->session->change_type(opnd2.value, type);
	}

	sc_addr from = info->session->get_beg(opnd2.value);
	sc_addr to = info->session->get_end(opnd2.value);

	if ((from && from != opnd1.value) || (to && to != opnd3.value))
		dprintf("Warning: changing of arc's ends is not permitted\n");

	if (info->session->set_end(opnd2.value, opnd3.value) ||
		info->session->set_beg(opnd2.value, opnd1.value)) {
		SCP_ERROR(info->process, "Cannot set begin and end for arc in handle_fixed_arc");
		return RV_ERR_GEN;
	}

	return RV_OK;
}

/**
 * @brief Указывает на недопустимую эквивалентность операндов при генерации констркуций.
 * @note Возвращает управление из вызывающего констекста с RV_ERR_GEN.
 * @param info информация о scp-процессе.
 * @param opnd_index1 индекс операнда.
 * @param opnd_index2 индекс операнда.
 */
#define BAD_OPERANDS_EQUAL(info, opnd_index1, opnd_index2) \
	if (opnd##opnd_index1.opnd == opnd##opnd_index2.opnd) { \
		SCP_ERROR(info->process, "Operand " << opnd_index1 << "_ and operand " << opnd_index2 << "_ equal"); \
		return RV_ERR_GEN; \
	}

SCP_OPERATOR_IMPL(scp_oper_genElStr3)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);

	BAD_OPERANDS_EQUAL(info, 1, 2);

	if (opnd1.var && !opnd1.fixed) {
		// Первый элемент тройки всегда узел.
		if (!(opnd1.type_mask & SC_SYNTACTIC_MASK)) {
			opnd1.type_mask |= SC_NODE;
		}

		if (opnd1.create_element())
			return RV_ERR_GEN;

		if (opnd1.opnd == opnd3.opnd) {
			// Рефлексивная дуга
			opnd3.value = opnd1.value;
			opnd3.fixed = 1;
		}
	}

	if (opnd3.var && !opnd3.fixed) {
		if (opnd3.create_element())
			return RV_ERR_GEN;

		if (opnd3.opnd == opnd2.opnd) {
			// Саморефлексивная дуга
			opnd2.value = opnd3.value;
			opnd2.fixed = 1;
		}
	}

	if (opnd2.var && !opnd2.fixed) {
		// Второй элемент тройки всегда дуга.
		if (!(opnd2.type_mask & SC_SYNTACTIC_MASK)) {
			opnd2.type_mask |= SC_ARC_ACCESSORY;
		}

		if (opnd2.create_element())
			return RV_ERR_GEN;

		if (info->session->set_beg(opnd2.value, opnd1.value) ||
			info->session->set_end(opnd2.value, opnd3.value)) {
			SCP_ERROR(info->process, "Cannot set begin and end for operand 2_: '" << SC_ID(opnd2.value) << "'");
			return RV_ERR_GEN;
		}
	} else {
		return handle_fixed_arc(info, opnd1, opnd2, opnd3);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_genElStr5)
{
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 4, opnd4);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 5, opnd5);

	BAD_OPERANDS_EQUAL(info, 1, 2); // первый элемент не может быть дугой.
	BAD_OPERANDS_EQUAL(info, 1, 4); // первый элемент не может быть дугой.
	BAD_OPERANDS_EQUAL(info, 5, 4); // пятый элемент не может быть дугой.

	if (opnd1.var && !opnd1.fixed) {
		// Первый элемент пятерки всегда узел.
		if (!(opnd1.type_mask & SC_SYNTACTIC_MASK)) {
			opnd1.type_mask |= SC_NODE;
		}

		if (opnd1.create_element())
			return RV_ERR_GEN;

		if (opnd1.opnd == opnd3.opnd) {
			// Рефлексивная дуга.
			opnd3.value = opnd1.value;
			opnd3.fixed = 1;
		}

		if (opnd1.opnd == opnd5.opnd) {
			// opnd1 одновременно и атрибут.
			opnd5.value = opnd1.value;
			opnd5.fixed = 1;
		}
	}

	if (opnd3.var && !opnd3.fixed) {
		if (opnd3.create_element())
			return RV_ERR_GEN;

		if (opnd2.opnd == opnd3.opnd) {
			// Саморефлексивная дуга
			opnd2.value = opnd3.value;
			opnd2.fixed = 1;
		}

		if (opnd4.opnd == opnd3.opnd) {
			// Дуга, входящая в дугу атрибута.
			opnd4.value = opnd3.value;
			opnd4.fixed = 1;
		}

		if (opnd5.opnd == opnd3.opnd) {
			// Дуга, входящая в атрибут.
			opnd5.value = opnd3.value;
			opnd5.fixed = 1;
		}
	}

	if (opnd5.var && !opnd5.fixed) {
		// Пятый элемент пятерки всегда узел.
		if (!(opnd5.type_mask & SC_SYNTACTIC_MASK)) {
			opnd1.type_mask |= SC_NODE;
		}
	
		if (opnd5.create_element())
			return RV_ERR_GEN;
	}

	if (opnd2.var && !opnd2.fixed) {
		// Второй элемент пятерки всегда дуга.
		if (!(opnd2.type_mask & SC_SYNTACTIC_MASK)) {
			opnd2.type_mask |= SC_ARC_ACCESSORY;
		}

		if (opnd2.create_element())
			return RV_ERR_GEN;

		if (info->session->set_beg(opnd2.value, opnd1.value) ||
			info->session->set_end(opnd2.value, opnd3.value)) {
			SCP_ERROR(info->process, "Cannot set begin and end for operand 2_: '" << SC_ID(opnd2.value) << "'");
			return RV_ERR_GEN;
		}

		if (opnd2.opnd == opnd4.opnd) {
			opnd4.value = opnd2.value;
			opnd4.fixed = 1;
		}
	} else {
		if (handle_fixed_arc(info, opnd1, opnd2, opnd3)) {
			return RV_ERR_GEN;
		}
	}

	if (opnd4.var && !opnd4.fixed) {
		// Четвертый элемент пятерки всегда дуга.
		if (!(opnd4.type_mask & SC_SYNTACTIC_MASK)) {
			opnd4.type_mask |= SC_ARC_ACCESSORY;
		}

		if (opnd4.create_element())
			return RV_ERR_GEN;

		if (info->session->set_beg(opnd4.value, opnd5.value)) {
			SCP_ERROR(info->process, "Cann't set begin for operand 4_: '" << SC_URI(opnd4.value) << "' as '" << SC_URI(opnd5.value) << "'");
			return RV_ERR_GEN;
		}

		if (info->session->set_end(opnd4.value, opnd2.value)) {
			SCP_ERROR(info->process, "Cann't set end for operand 4_: '" << SC_URI(opnd4.value) << "' as '" << SC_URI(opnd2.value) << "'");
			return RV_ERR_GEN;
		}
	} else {
		return handle_fixed_arc(info, opnd5, opnd4, opnd2);
	}

	return RV_OK;
}
