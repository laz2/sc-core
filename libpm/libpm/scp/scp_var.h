
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
 * @file scp_var.h
 * @brief Интерфейс для работы с scp-пременными.
 */

#ifndef __SCP_VARS_H__
#define __SCP_VARS_H__

#include <libsc/sc_types.h>
#include "scp_core.h"

/**
 * @brief Функция инициализации подсистемы scp-переменных.
 */
sc_retval scp_vars_init(sc_session *system);

/**
 * @brief Функция получения значения scp-переменной.
 * @param info информация о scp-процессе, программе которого принадлежит scp-переменная.
 * @param var знак scp-переменной, значение которой необходимо получить.
 * @return значение scp-переменной @p var или 0 в случае неуспешного выполнения.
 */
sc_addr scp_get_var_value(scp_process_info *info, sc_addr var);

/**
 * @brief Более низкоуровневая функция получения значения scp-переменной, чем #scp_get_var_value.
 * @param info информация о scp-процессе, программе которого принадлежит scp-переменная.
 * @param var знак scp-переменной, значение которой необходимо получить.
 * @param[out] conn найденная связка отношения "переменная-значение".
 * @param[out] value значение scp-переменной @p var или 0 в случае неуспешного выполнения.
 * @param[out] value_arc дуга, связывающая найденные @p conn и @p value.
 * @return значение scp-переменной @p var или 0 в случае неуспешного выполнения.
 */
sc_retval get_var_value(scp_process_info *info, sc_addr var, sc_addr *conn, sc_addr *value, sc_addr *value_arc = 0);

/**
 * @brief Функция установки значения scp-переменной.
 * @param info информация о scp-процессе, программе которого принадлежит scp-переменная.
 * @param var знак scp-переменной, значение которой необходимо установить.
 * @param value знак, который будет установлен в качестве значения @p var.
 * @return sc-код возврата
 */
sc_retval scp_set_variable(scp_process_info *info, sc_addr var, sc_addr value);

/**
 * @brief Устанавливает значение scp-переменной с учетом того, что данная переменная не имеет еще значения.
 * @param info информация о scp-процессе.
 * @param var знак переменной.
 * @param value знак значения для установки.
 * @return sc-код возврата.
 */
sc_retval __set_variable(scp_process_info *info,sc_addr var,sc_addr value);

/**
 * @brief Проверяет, является ли переданный знак, знаком переменной scp-процесса, для которого переданна информация.
 * @param info информация о scp-процессе.
 * @param cand знак для проверки. 
 * @return true - переданный знак является знаком переменной данного scp-процесса.
 * @return false - переданный знак не является знаком переменной данного scp-процесса.
 */
bool is_variable(scp_process_info *info, sc_addr cand);

/**
 * @brief Ощищает переменную.
 * @param info информация о scp-процессе.
 * @param var знак переменной.
 */
void scp_clear_variable(scp_process_info *info, sc_addr var);

/**
 * @brief Очищает все scp-переменные, которые имеют в качестве значения @p value.
 * @param var_value знак отношения "переменная-значения, в котором будут очищаться переменные.
 * @param value значение, для которого будут очищаться переменные.
 */
void scp_clear_this_value_variables(sc_addr var_value, sc_addr value);


/**
 * @brief Удаления кэша значений переменных для переданного знака отношения "переменная-значение".
 * @param var_value знак отношения "переменная-значения".
 */
void dispose_cache(sc_addr var_value);

/**
 * @brief Создает кэш значений переменных для переданного знака отношения "переменная-значение". 
 * @param var_value знак отношения "переменная-значения".
 */
void init_cache(sc_addr var_value);

sc_retval __get_var_value(sc_addr var_value, sc_addr var, sc_addr *conn, sc_addr *value, sc_addr *valueArc = 0);

sc_retval __set_variable(sc_addr var_value, sc_addr var, sc_addr value);

void __clear_variable(sc_addr var_value, sc_addr var);

#endif // __SCP_VARS_H__
