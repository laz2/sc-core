
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
 * @file sc_atoms.h
 * @brief Интерфейс для создания ключевых sc-элементов (sc-атомов) в сторонних программах.
 * @ingroup libsc
 */

#ifndef __SC_ATOMS_H__
#define __SC_ATOMS_H__

#include "libsc.h"

/**
 * @brief URI sc-сегмента, где находсять все sc-атомы.
 */
#define SC_ATOMS_SEGNAME "/proc/keynode"

/**
 * @brief SC-сегмент, в котором находяться все sc-атомы.
 */
LIBSC_API extern sc_segment *sc_atoms_segment;

/**
 * @brief Функция инициализации подсистемы sc-атомов.
 */
LIBSC_API void sc_atoms_init();

/**
 * @brief Функция создания sc-атома с именем @p name в sc-памяти.
 */
LIBSC_API sc_addr sc_atom_intern(const char *name);

struct sc_atom;

/**
 * @brief Функция создания sc-атома в sc-памяти.
 */
LIBSC_API void sc_atom_intern_special(sc_atom *);

/**
 * @brief Структура определяющая sc-атом.
 */
struct LIBSC_API sc_atom {
	/**
	 * @brief имя sc-атома.
	 */
	const char *name;

	/**
	 * @brief sc-адрес, соответствующий данному sc-атому.
	 */
	sc_addr value;

	/**
	 * @brief следующий sc-атом в списке.
	 */
	sc_atom *next;

	/**
	 * @brief Конструктор создания sc-атом с именем @p name.
	 */
	sc_atom(const char *_name)
	{
		name = _name;
		value = 0;
		sc_atom_intern_special(this);
	}
};

/**
 * @brief Макрос для объявления sc-атома @p name в заголовочном файле.
 */
#define DECLARE_SC_ATOM(name) \
extern sc_atom __sc_atom_##name

/**
 * @brief Макрос для определения sc-атома @p name в файле реализации.
 */
#define DEFINE_SC_ATOM(name) \
sc_atom __sc_atom_##name(#name)

/**
 * @brief Макрос для получения sc-адреса, соответствущего sc-атому c именем @p name.
 */
#define SC_ATOM(name) ((__sc_atom_##name).value)

#endif // __SC_ATOMS_H__
