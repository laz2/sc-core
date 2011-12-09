
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
 * @file idtf_gen.h
 * @brief Файл описания функций для работы с uid-генератором.
 * @ingroup libsc
 */

#ifndef __IDTF_GEN_H__
#define __IDTF_GEN_H__

#include "sc_types.h"

/**
 * @brief Тип UID-генератора.
 */
typedef sc_string (*uid_generator_func)();

/**
 * @brief Функция установки uid-генератора.
 */
void LIBSC_API set_uid_generator(uid_generator_func);

/**
 * @brief Функция генерации uid'a для репозитория.
 */
sc_string LIBSC_API generate_uid();

#endif // __IDTF_GEN_H__
