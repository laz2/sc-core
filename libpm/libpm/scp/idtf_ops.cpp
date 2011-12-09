
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
 * @file idtf_ops.cpp
 * @brief Implementation SCP-operators for working with identificators of SC-elements.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

SCP_OPERATOR_IMPL(scp_oper_idtfMove)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, target_opnd);
	SCP_OPND_REQ_FIXED(info, 2, source_opnd);

	sc_string str = info->session->get_idtf(source_opnd.value);
	Cont _cont;
	_cont.d.ptr = const_cast<char *>(str.c_str()); // при установке содержимого эта память будет скопирована
	_cont.d.size = str.length() + 1;

	Content cont(_cont, TCSTRING);
	return info->session->set_content(target_opnd.value, cont);
}

SCP_OPERATOR_IMPL(scp_oper_idtfAssign)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, target_opnd);
	SCP_OPND_REQ_FIXED(info, 2, idtf_cont_opnd);

	Content cont = info->session->get_content(idtf_cont_opnd.value);
	if (cont.type() != TCSTRING) {
		SCP_ERROR_F(info->process, "Operand %s must have string content", idtf_cont_opnd.get_name());
		return RV_ERR_GEN;
	}

	if (info->session->set_idtf(target_opnd.value, Cont(cont).d.ptr)) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		scp_set_state(info->process, SCP_STATE_THEN);
	}
	
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_idtfErase)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	info->session->erase_idtf(opnd1.value);
	return RV_OK;
}
