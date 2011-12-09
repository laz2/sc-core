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
 * @file control_ops.cpp
 * @brief Implementation SCP-operators for control SCP-process.
 * @ingroup libpm
 */

#include "precompiled_p.h"

SCP_OPERATOR_IMPL(scp_oper_call)
{
	SCP_OPND_REQ_FIXED(info, 1, program_opnd);
	SCP_OPND_REQ_FIXED(info, 2, params_opnd);
	SCP_OPND_OPT_ASSIGN_FIXED_CREATE(info, 3, descr_opnd);

	sc_addr descr = descr_opnd.value;

	sc_session *sonsession = info->session->__fork();
	sc_addr process = 0;

	if (scp_create_process(sonsession, program_opnd.value, info->process, params_opnd.value, &process, descr))
		return RV_ERR_GEN;

	pm_sched_add_agent(process, SCHED_CLASS_NORMAL);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_callReturn)
{
	SCP_OPND_REQ_FIXED(info, 1, program_opnd);
	SCP_OPND_REQ_FIXED(info, 2, params_opnd);

	if (scp_call_program(info, program_opnd.value, params_opnd.value) != RV_OK) {
		return RV_ERR_GEN;
	} else {
		scp_set_state(info->process, SCP_STATE_REPEAT);
		return RV_OK;
	}
}

SCP_OPERATOR_IMPL(scp_oper_callExtReturn)
{
	SCP_OPND_REQ_FIXED(info, 1, program_opnd);
	SCP_OPND_REQ_FIXED(info, 2, params_opnd);
	return scp_call_ext_program(info, program_opnd.value, params_opnd.value);
}

SCP_OPERATOR_IMPL(scp_oper_waitReturn)
{
	SCP_OPND_OPT_FIXED(info, 1, process_opnd);

	sc_addr wait_on = ALL_CHILDS;

	if (process_opnd.opnd)
		wait_on = process_opnd.value;

	return scp_sleep_on_child(info->process, wait_on);
}

SCP_OPERATOR_IMPL(scp_oper_return)
{
	return scp_process_return(info);
}

SCP_OPERATOR_IMPL(scp_oper_return_then)
{
	return scp_process_return(info, SCP_STATE_THEN);
}

SCP_OPERATOR_IMPL(scp_oper_return_else)
{
	return scp_process_return(info, SCP_STATE_ELSE);
}

SCP_OPERATOR_IMPL(scp_oper_halt)
{
	pm_sched_put_to_sleep(info->process);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_nop)
{
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_sys_breakpoint)
{
#ifdef _MSC_VER
	_asm { int 3 }
#else
	__asm__("int $3");
#endif
	return RV_OK;
}
