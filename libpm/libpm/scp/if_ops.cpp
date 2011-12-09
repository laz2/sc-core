
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
 * @file if_ops.cpp
 * @brief Implementation conditionally SCP-operators.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

SCP_OPERATOR_IMPL(scp_oper_ifType)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	if (check_sc_type(info->session->get_type(opnd1.value), opnd1.type_mask)) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifEq)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr element1 = opnd1.value;
	sc_addr element2 = opnd2.value;

	if (element1 == element2) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		Content cont1 = info->session->get_content(element1);
		Content cont2 = info->session->get_content(element2);
		
		if (cont1 == cont2) {
			scp_set_state(info->process, SCP_STATE_THEN);
		} else {
			scp_set_state(info->process, SCP_STATE_ELSE);
		}
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifGr)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr element1 = opnd1.value;
	sc_addr element2 = opnd2.value;

	if (element1 == element2) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		Content cont1 = info->session->get_content(element1);
		Content cont2 = info->session->get_content(element2);

		if (cont1 > cont2) {
			scp_set_state(info->process, SCP_STATE_THEN);
		} else {
			scp_set_state(info->process, SCP_STATE_ELSE);
		}
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifGrEq)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr element1 = opnd1.value;
	sc_addr element2 = opnd2.value;

	if (element1 == element2) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		Content cont1 = info->session->get_content(element1);
		Content cont2 = info->session->get_content(element2);

		if (cont1 > cont2 || cont1 == cont2) {
			scp_set_state(info->process, SCP_STATE_THEN);
		} else {
			scp_set_state(info->process, SCP_STATE_ELSE);
		}
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifCoin)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr element1 = opnd1.value;
	sc_addr element2 = opnd2.value;

	if (element1 == element2) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifFormCont)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	Content cont = info->session->get_content(opnd1.value);
	
	if (cont.type() == TCEMPTY) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		scp_set_state(info->process, SCP_STATE_THEN);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifFormIdtf)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	if (info->session->get_idtf(opnd1.value).size() != 0) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifVarAssign)
{
	SCP_OPND_REQ_VAR(info, 1, opnd1);

	if (opnd1.value) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifNumber)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	Content cont = info->session->get_content(opnd1.value);

	if (cont.type() == TCINT || cont.type() == TCREAL) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_ifString)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	Content cont = info->session->get_content(opnd1.value);

	if (cont.type() == TCSTRING) {
		scp_set_state(info->process, SCP_STATE_THEN);
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
	}

	return RV_OK;
}
