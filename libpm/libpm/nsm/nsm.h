
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
 * @file nsm.h
 * @brief Интерфейс NSM-подсистемы.
 */

#ifndef __NSM_H__
#define __NSM_H__

#ifdef _MSC_VER
#	pragma warning (disable: 4786)
#	pragma warning (disable: 4996)
#endif

#include <stdio.h>
#include <limits.h>

#include <algorithm>
#include <list>
#include <map>

#include <libsc/std_constraints.h>
#include <libsc/pm_keynodes.h>
#include <libsc/libsc.h>
#include <libsc/tgf_io.h>
#include <libsc/sc_transaction.h>

#ifndef NSM_DONT_USE_SCHEDULER
#	include <libpm/pm_sched.h>
#endif //NSM_DONT_USE_SCHEDULER

typedef std::list<sc_addr>	LISTSC_ADDR;

extern sc_session *nsm_session;
extern sc_segment *nsm_segment;
extern sc_segment *nsm_keynodes_segment;

#endif //__NSM_H__
