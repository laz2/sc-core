
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
 * @file std_constraints.h
 * @brief Стандартные типы шаблонов поиска.
 * @ingroup libsc
 */

#ifndef __SC_STD_CONSTRAINTS_H__
#define __SC_STD_CONSTRAINTS_H__

#include "sc_types.h"

extern LIBSC_API sc_uint
	__constr_all_input,
	__constr_all_output,
	__constr_arc_type_input,
	__constr_arc_type_output,
	__constr_3faa,
	__constr_3faf,
	__constr_3aaf,
	__constr_3dfd,
	__constr_5faaaa,
	__constr_5faaaf,
	__constr_5fafaa,
	__constr_5fafaf,
	__constr_5aaaaf,
	__constr_5aafaa,
	__constr_5aafaf,
	__constr_ord_bin_conn1,
	__constr_ord_bin_conn1_def,
	__constr_ord_bin_conn2,
	__constr_3l2_faaaf,
	__constr_5_3_aaaafaf,
	__constr_in_set,
	__constr_sely3_p1,
	__constr_sely3_u1p2,
	__constr_sely3_u1u2p3,
	__constr_sely5_p1,
	__constr_sely5_u1p2,
	__constr_sely5_u1u2p3,
	__constr_sely5_u1u2u3p4,
	__constr_sely5_u1u2u3u4p5,
	__constr_intersect2,
	__constr_3l2_5faaaf,
	__constr_bin_conn_unord1,
	__constr_4ln5_faaaaaffa,
	__constr_4l5_faaaaaffa,
	__constr_ord_bin_conn1a,
	__constr_on_segment;

/**
 * @name Стандартные типы шаблонов поиска.
 * Стандартные типы шаблонов поиска для задания конкретных шаблонов поиска.
 * При задании конкретного шаблона поиска указывается определенное количество параметров.
 * Параметры бывают следующих типов:
 * - #sc_addr - адрес sc-элемента, который подставляется в шаблон поиска.
 * - #sc_type - маска типа sc-элемента, который ограничивает область поиска. #SC_EMPTY указывает на отсутствие ограничений.
 *
 * При описании параметров для типа шаблона поиска, сначала указывается тип параметра, а затем описание его семантики. 
 */
// @{

/**
 * @brief Тип шаблона поиска по всем входящим дугам для @p e1.
 * @param e1 #sc_addr.
 */
#define CONSTR_ALL_INPUT	__constr_all_input

/**
 * @brief Тип шаблона поиска по всем выходящим дугам для @p e1.
 * @param e1 #sc_addr.
 */
#define CONSTR_ALL_OUTPUT	__constr_all_output

/**
 * @brief Тип шаблона поиска по входящим дугам с маской типа @p t1 для @p e1.
 * @param t1 #sc_type, маска типа sc-дуги c указанием #SC_ARC.
 * @param e1 #sc_addr.
 */
#define CONSTR_ARC_TYPE_INPUT	__constr_arc_type_input

/**
 * @brief Тип шаблона поиска по выходящим дугам с маской типа @p t1 для @p e1.
 * @param t1 #sc_type, маска типа sc-дуги c указанием #SC_ARC.
 * @param e1 #sc_addr.
 */
#define CONSTR_ARC_TYPE_OUTPUT	__constr_arc_type_output

/**
 * @brief Ограничение поиска по "тройкам" с фиксированным @p e1.
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_type.
 */
#define CONSTR_3_f_a_a		__constr_3faa

/**
 * @brief Ограничение поиска по "тройкам" с фиксированным @p e1 и @p e3.
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_addr.
 */
#define CONSTR_3_f_a_f		__constr_3faf

/**
 * @brief Ограничение поиска по "тройкам" с фиксированным @p e3.
 * @param e1 #sc_type.
 * @param e2 #sc_type.
 * @param e3 #sc_addr.
 */
#define CONSTR_3_a_a_f		__constr_3aaf
#define CONSTR_3_d_f_d		__constr_3dfd

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным первым элементом.
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_type.
 * @param e4 #sc_type.
 * @param e5 #sc_type.
 */
#define CONSTR_5_f_a_a_a_a	__constr_5faaaa

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным первым и пятым элементами.
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_type.
 * @param e4 #sc_type.
 * @param e5 #sc_addr.
 */
#define CONSTR_5_f_a_a_a_f	__constr_5faaaf

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным первым и третьим элементами.
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_addr.
 * @param e4 #sc_type.
 * @param e5 #sc_type.
 */
#define CONSTR_5_f_a_f_a_a	__constr_5fafaa

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным первым, третьим и пятым элементами.
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_addr.
 * @param e4 #sc_type.
 * @param e5 #sc_addr.
 */
#define CONSTR_5_f_a_f_a_f	__constr_5fafaf

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным пятым элементом.
 * @param e1 #sc_type.
 * @param e2 #sc_type.
 * @param e3 #sc_type.
 * @param e4 #sc_type.
 * @param e5 #sc_addr.
 */
#define CONSTR_5_a_a_a_a_f	__constr_5aaaaf

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным третим элементом.
 * @param e1 #sc_type.
 * @param e2 #sc_type.
 * @param e3 #sc_addr.
 * @param e4 #sc_type.
 * @param e5 #sc_type.
 */
#define CONSTR_5_a_a_f_a_a	__constr_5aafaa

/**
 * @brief Тип шаблона поиска по "пятеркам" с фиксированным третим и пятым элементами.
 * @param e1 #sc_type.
 * @param e2 #sc_type.
 * @param e3 #sc_addr.
 * @param e4 #sc_type.
 * @param e5 #sc_addr.
 */
#define CONSTR_5_a_a_f_a_f	__constr_5aafaf

/**
 * @brief Тип шаблона поиска по связкам бинарного отношения с фиксированным третим и пятым элементами:
 *
 * @image html CONSTR_ORD_BIN_CONN1.jpg
 * 
 * @param relation #sc_addr, знак бинарного отношения.
 * @param e2 #sc_type, маска типа дуги, связывающей знак отношение со знаком связки.
 * @param conn #sc_type, маска типа связки отношения.
 * @param e4 #sc_type, маска типа дуги, связывающей связку отношения с элементом, уточненным атрибутом @p val1_attr.
 * @param val1 #sc_type, маска типа элемента, который уточнен атрибутом @p val1_attr.
 * @param e6 #sc_type, маска типа дуги, связывающей связку отношения с элементом, уточненным атрибутом @p val2_attr.
 * @param val2 #sc_type, маска типа элемента, который уточнен атрибутом @p val2_attr.
 * @param e8 #sc_type, маска типа дуги, связывающей @p val1_attr c e4.
 * @param val1_attr #sc_addr, знак уточняющего атрибута для @p val1.
 * @param e10 #sc_type, маска типа дуги, связывающей @p val1_attr c e4.
 * @param val2_attr #sc_addr, знак уточняющего атрибута для @p val1.
 */
#define CONSTR_ORD_BIN_CONN1	__constr_ord_bin_conn1
#define CONSTR_ORD_BIN_CONN1_DEF	__constr_ord_bin_conn1_def
#define CONSTR_ORD_BIN_CONN2	__constr_ord_bin_conn2

/**
 * @brief Тип шаблона поиска по определенным конструкциям с фиксированным первым и пятым элементами.
 *
 * Данный тип шаблона поиска для конструкций вида: <br>
 * @p e1 -- @p e2 --> @p e3 -- @p e4 --> @p e5
 * 
 * @param e1 #sc_addr.
 * @param e2 #sc_type.
 * @param e3 #sc_type.
 * @param e4 #sc_type.
 * @param e5 #sc_addr.
 */
#define CONSTR_3l2_f_a_a_a_f	__constr_3l2_faaaf
#define CONSTR_5_3_a_a_a_a_f_a_f __constr_5_3_aaaafaf
#define CONSTR_in_set		__constr_in_set
#define CONSTR_sely3_p1		__constr_sely3_p1
#define CONSTR_sely3_u1p2	__constr_sely3_u1p2
#define CONSTR_sely3_u1u2p3	__constr_sely3_u1u2p3
#define CONSTR_sely5_p1		__constr_sely5_p1
#define CONSTR_sely5_u1p2	__constr_sely5_u1p2
#define CONSTR_sely5_u1u2p3	__constr_sely5_u1u2p3
#define CONSTR_sely5_u1u2u3p4	__constr_sely5_u1u2u3p4
#define CONSTR_sely5_u1u2u3u4p5	__constr_sely5_u1u2u3u4p5
#define CONSTR_intersect2	__constr_intersect2
#define CONSTR_3l2_5faaaf	__constr_3l2_5faaaf
#define CONSTR_bin_conn_unord1	__constr_bin_conn_unord1
#define CONSTR_4ln5_faaaaaffa	__constr_4ln5_faaaaaffa
#define CONSTR_4l5_faaaaaffa	__constr_4l5_faaaaaffa
#define CONSTR_ORD_BIN_CONN1A	__constr_ord_bin_conn1a
// }@

/// Constraint for iterate over elements in specified segment.
/// @param seg #sc_segment, which constraints elements.
/// @param foreign bool, if false, then skip links to elements in other segments.
#define CONSTR_ON_SEGMENT __constr_on_segment

/**
 * @brief Функция, которая проводит последующую инициализацию после первичной инициализации подсистемы шаблонов поиска.
 */
LIBSC_API void __postinit_std_constraints(sc_session *system);

/**
 * @brief Функция, которая проводит первичную инициализацию подсистемы шаблонов поиска.
 */
LIBSC_API void __init_std_constraints();

#endif // __SC_STD_CONSTRAINTS_H__
