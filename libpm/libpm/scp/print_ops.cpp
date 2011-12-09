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
 * @file print_ops.cpp
 * @brief Implementation SCP-operators for printing SC-elements.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

SCP_OPERATOR_IMPL(scp_oper_print)
{
	for (size_t j = 1; j < 16; ++j) {
		SCP_OPND_OPT_FIXED(info, j, opnd);

		if (!opnd.opnd)
			break;

		if (print_content(*info->session->get_content_const(opnd.value)))
			return RV_ERR_GEN;
	}

	fflush(stdout);

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_printNl)
{
	if (scp_oper_print::interpret(_this, info))
		return RV_ERR_GEN;

	putchar('\n');

	fflush(stdout);

	return RV_OK;
}

bool no_print_el;

SCP_OPERATOR_IMPL(scp_oper_printEl)
{
	if (no_print_el)
		return RV_OK;

	for (size_t i = 1; i < 16; ++i) {
		SCP_OPND_OPT_FIXED(info, i, opnd);

		if (!opnd.opnd)
			break;

		print_el(info->session, opnd.value);
	}

	return RV_OK;
}
