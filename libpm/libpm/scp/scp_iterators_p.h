
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



#ifndef __SCP_ITERATORS_H__
#define __SCP_ITERATORS_H__

#include <libsc/libsc.h>
#include <libsc/sc_filter.h>

#include "scp_core.h"
#include "scp_operand.h"

/**
 * Создает итератор по трех элементной структуре. 
 *
 * @param info информация о scp-процессе.
 * @param opnd1 первый операнд шаблона поиска.
 * @param opnd2 второй операнд шаблона поиска.
 * @param opnd3 третий операнд шаблона поиска.
 * @param f1 фильтр для первого элемента конструкции.
 * @param f2 фильтр для второго элемента конструкции.
 * @param f3 фильтр для третьего элемента конструкции.
 * @param rv код возврата.
 * @return указатель на итератор или NULL (в случае ошибки).
 */
sc_iterator *create_search3_iterator(
	scp_process_info *info,
	scp_opnd_info &opnd1,
	scp_opnd_info &opnd2,
	scp_opnd_info &opnd3,
	sc_single_filter *f1,
	sc_single_filter *f2,
	sc_single_filter *f3,
	sc_retval &rv
);

sc_iterator *create_search5_iterator(
	scp_process_info *info,
	scp_opnd_info &opnd1,
	scp_opnd_info &opnd2,
	scp_opnd_info &opnd3,
	scp_opnd_info &opnd4,
	scp_opnd_info &opnd5,
	sc_single_filter *f1,
	sc_single_filter *f2,
	sc_single_filter *f3,
	sc_single_filter *f4,
	sc_single_filter *f5,
	sc_retval &rv
);

#endif //__SCP_ITERATORS_H__
