
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
 * @file sc_keynodes.h
 * @brief Системные ключевые узлы библиотеки libsc.
 * @ingroup libsc
 */

#ifndef __SC_KEYNODES_H__
#define __SC_KEYNODES_H__

#include "sc_types.h"

/**
 * @brief Массив системных ключевых узлов библиотеки.
 */
extern LIBSC_API sc_addr sc_keynodes[];

/**
 * @brief Множество знаков директорий.
 */
#define DIRENT sc_keynodes[0]

#endif // __SC_KEYNODES_H__
