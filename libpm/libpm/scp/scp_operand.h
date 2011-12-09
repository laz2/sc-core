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
 * @addtogroup libpm
 * @{
 */

/**
 * @file scp_operand.h
 * @brief SCP-operand and utils for working with them.
 */

#ifndef __SCP_OPERANDS_H__
#define __SCP_OPERANDS_H__

#include <libsc/sc_types.h>
#include <libsc/sc_filter.h>

#include "scp_core.h"

/// Info structure about SCP-operand.
class scp_opnd_info
{
public:
	scp_process_info *pi;
	int index;          /**< @brief Поряковый индекс */
	sc_addr opnd_arc;   /**< @brief Адрес дуги, связывающей оператор с операндом */
	sc_addr opnd;       /**< @brief Адрес операнда */
	sc_addr value;      /**< @brief Значение операнда */
	int var:1;          /**< @brief Если установлен, то флаг указывает на то, что операнд - переменная, иначе - константа */
	int assign:1;       /**< @brief Флаг указывает на то, что операнд уточнен атрибутом <code>assign_</code> */
	int fixed:1;        /**< @brief Флаг указывает на то, что операнд уточнен атрибутом <code>fixed_</code> */
	int set:1;

	/** 
	 * @brief Маска типа, полученная на основе атрибутов, 
	 * уточняющих структурный тип и признаки константности и нечеткости. 
	 */
	sc_type type_mask;

	scp_opnd_info() : pi(NULL), index(0), opnd_arc(0), opnd(0),
		value(0), var(0), assign(0), fixed(0), set(0), type_mask(0), name(NULL)
	{}

	~scp_opnd_info()
	{
		if (name)
			delete []name;
	}

	bool search_ord(scp_process_info *pi, int index_);

	bool search_set(scp_process_info *pi, int index_);

	bool search_ord(scp_process_info *pi, int index_, sc_addr opnd_arc_);

	bool search_set(scp_process_info *pi, int index_, sc_addr opnd_arc_);

	sc_retval create_element();

	const char* get_name() const
	{
		if (name == NULL)
			form_name();
		return name;
	}

private:
	void form_name() const;

	bool unpack();

	void unpack_arc_flags();

	void retrieve();

private:
	mutable char *name; ///< Full name of this operand.
};

/**
 * @brief Функция получения сегмента, знак котрого являеться segc_-операндом с номером @p index.
 * @param pi информация о scp-процессе.
 * @param index порядковый номер segc_-операнда.
 * @param[out] segment указатель на место, куда будет записан возвращаемый сегмент.
 * @return sc-код возврата.
 */
sc_retval get_single_segment(scp_process_info *pi, int index, sc_segment **segment);

/**
 * @brief Функция получения segc_-операнда с номером @p index.
 * @param pi информация о scp-процессе.
 * @param index порядковый номер segc_-операнда.
 * @param[out] segc указатель на место, куда будет записан знак операнда.
 * @return sc-код возврата.
 */
sc_retval get_single_segc(scp_process_info *pi, int index, sc_addr *segc);

/**
 * @brief Функция получения фильтра по сегментам, сформированного из segc_-множества с номером @p index.
 * @param pi информация о scp-процессе.
 * @param index порядковый номер segc_-множества.
 * @param[out] filter указатель на место, куда будет записан сформированный фильтр.
 * @return sc-код возврата.
 */
sc_retval get_multi_segc_filter(scp_process_info *pi, int index, sc_single_filter **filter);

/**
 * @brief Функция позволяет заполнить структуру @p opnd_info данными для порядкового операнда с индексом @p index.
 * @param pi информация о scp-процессе.
 * @param index порядковый номер порядкового операнда.
 * @param[in,out] opnd_info указатель на стуруктуру информации об операнде, которая будет заполнена.
 * @param flags не используется.
 * @return sc-код возврата.
 */
sc_retval scp_fill_opndinfo(scp_process_info *pi, int index, scp_opnd_info *opnd_info, unsigned flags = 0);

/**
 * @brief Функция позволяет заполнить структуру @p opnd_info данными для set-операнда с индексом @p index.
 * @param pi информация о scp-процессе.
 * @param index порядковый номер set-операнда.
 * @param[in,out] opnd_info указатель на стуруктуру информации об set-операнде, которая будет заполнена.
 * @return sc-код возврата.
 */
sc_retval scp_fill_set_opndinfo(scp_process_info *pi, int index, scp_opnd_info *opnd_info);

#endif // __SCP_OPERANDS_H__

/**@}*/
