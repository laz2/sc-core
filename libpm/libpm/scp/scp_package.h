
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
 * Интерфейс для работы с пакетами scp-программ и текстами scp-программ.
 */

#ifndef __SCP_PACKAGE_H__
#define __SCP_PACKAGE_H__

#include <libsc/libsc.h>

LIBPM_API sc_retval parse_package(sc_segment *seg, const sc_string &place);
LIBPM_API sc_addr spawn_package(const sc_string &place);
LIBPM_API sc_addr spawn_package_ex(const sc_string &place, sc_addr owner_agent);

LIBPM_API sc_retval run_package(const sc_string &place);

LIBPM_API void scp_package_init(sc_session*);

LIBPM_API sc_retval scp_package_locate_program_text(sc_session *s, sc_addr program, addr_set &a_set, addr_set *operators);

LIBPM_API sc_retval scp_package_run_program(sc_session *parent, sc_addr program, sc_addr parent_process, sc_addr params, sc_addr *process_);

#endif // __SCP_PACKAGE_H__
