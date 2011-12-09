
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
 * @file segment_ops.cpp
 * @brief Implementation SCP-operators for working with SC-segments.
 */

#include "precompiled_p.h"

#include "scp_segment_utils_p.h"

SCP_OPERATOR_IMPL(scp_oper_print_opened_segments)
{
	const segment_set &ss = info->session->get_segment_set();
	sc_segment *seg;
	printf("Printing all open segments:\n");
	FOR_ALL_SEGMENTS(seg,ss) {
		printf("%s\n",seg->get_full_uri().c_str());
	}
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_open_segment_uri)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_ASSIGN(info, 2, i2);

	sc_retval rv = scp_open_segment_uri(info,i1.value,i2.opnd);
	if (RV_IS_ERR(rv)) {
		return RV_ERR_GEN;
	}

	scp_set_state(info->process, rv ? SCP_STATE_ELSE : SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_open_segment)
{
	SCP_OPND_REQ_FIXED(info, 1, sign_opnd);

	sc_retval rv = scp_open_segment(info, sign_opnd.value);

	if (RV_IS_ERR(rv)) {
		return RV_ERR_GEN;
	}

	scp_set_state(info->process, rv ? SCP_STATE_ELSE : SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_close_segment)
{
	SCP_OPND_REQ_FIXED(info, 1, sign_opnd);
	return scp_close_segment(info, sign_opnd.value);
}

SCP_OPERATOR_IMPL(scp_oper_create_segment)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_ASSIGN(info, 2, i2);

	sc_retval rv = scp_create_segment(info,i1.value,i2.opnd);
	if (RV_IS_ERR(rv)) {
		return RV_ERR_GEN;
	}

	scp_set_state(info->process, rv ? SCP_STATE_ELSE : SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_set_default_segment)
{
	SCP_OPND_REQ_FIXED(info, 1, sign_opnd);

	sc_segment *seg = sign2seg(info->session, sign_opnd.value);
	if (!seg) {
		return RV_ERR_GEN;
	}

	scp_process_info::set_default_seg(info->process, sign_opnd.value);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_get_default_segment)
{
	SCP_OPND_REQ_VAR(info, 1, out_sign_opnd);

	if (out_sign_opnd.value) {
		scp_clear_variable(info, out_sign_opnd.opnd);
	}

	scp_set_variable(info, out_sign_opnd.opnd, info->defaultseg_sign);

	return RV_OK;
}


SCP_OPERATOR_IMPL(scp_oper_unlink)
{
	SCP_OPND_REQ_FIXED(info, 1, sign_opnd);
	sc_retval rv = info->session->unlink(info->session->sign2uri(sign_opnd.value));
	scp_set_state(info->process, rv ? SCP_STATE_ELSE : SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_get_autoseg)
{
	SCP_OPND_REQ_VAR(info, 1, out_sign_opnd);
	scp_set_variable(info, out_sign_opnd.opnd, info->auto_dirent);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_spin_segment)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 2, i2);
	SCP_OPND_OPT_FIXED(info, 3, maxlev_opnd);

	int maxlev = -1;

	if (maxlev_opnd.opnd) {
		Content cont = info->session->get_content(maxlev_opnd.value);
		if (cont.type() == TCINT) {
			maxlev = Cont(cont).i;
		} else if (cont.type() == TCREAL) {
			creal r = Cont(cont).r;
			if (floor(r) != r) {
				SCP_ERROR(info->process, "Operand 3_: '" << SC_ID(maxlev_opnd.opnd) << "' has real content with fractional part.");
				return RV_ERR_GEN;
			}
			maxlev = (int)r;
		} else {
			SCP_ERROR(info->process, "Operand 3_: '" << SC_ID(maxlev_opnd.opnd) << "' must have int or real content.");
			return RV_ERR_GEN;
		}
	}
	sc_retval rv = scp_span_segments(info, i1.value, i2.value, maxlev);
	scp_set_state(info->process, rv ? SCP_STATE_ELSE : SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_opendir_uri)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_ASSIGN(info, 2, i2);

	sc_retval rv = scp_opendir_uri(info,i1.value,i2.opnd);
	if (RV_IS_ERR(rv))
		return RV_ERR_GEN;

	scp_set_state(info->process,rv?SCP_STATE_ELSE:SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_opendir)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_ASSIGN(info, 2, i2);

	sc_retval rv = scp_opendir(info,i1.value,i2.opnd);

	if (RV_IS_ERR(rv))
		return RV_ERR_GEN;

	scp_set_state(info->process,rv?SCP_STATE_ELSE:SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_getloc)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_ASSIGN(info, 2, i2);

	sc_addr sign = seg2sign(info->session,i1.value->seg);
	if (!sign) {
		scp_set_state(info->process,SCP_STATE_ELSE);
		return RV_OK;
	}
	if (scp_set_variable(info,i2.opnd,sign))
		return RV_ERR_GEN;
	scp_set_state(info->process,SCP_STATE_THEN);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_create_tmp_segment)
{
	SCP_OPND_REQ_ASSIGN(info, 1, i1);

	sc_segment *seg = create_unique_segment(info->session,"/tmp/genby_scp");
	if (!seg)
		return RV_ERR_GEN;
	sc_addr sign = seg2sign(info->session,seg);
	if (!sign) {
		fprintf(stderr,"Cannot get sign of segment. Weird!\n");
		return RV_ERR_GEN;
	}
	if (scp_set_variable(info,i1.opnd,sign))
		return RV_ERR_GEN;
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_move_elements)
{
	SCP_OPND_REQ_FIXED(info, 1, i1);
	SCP_OPND_REQ_FIXED(info, 1, i2);

	sc_segment *seg = is_segment(info->session,i2.value);
	if (!seg)
		return RV_ERR_GEN;
	bool flag = false;

	sc_iterator *iter = info->session->create_iterator(sc_constraint_new(
			CONSTR_3_f_a_a,
			i1.value,
			0,
			0
		),true);
	for (;!iter->is_over();iter->next()) {
		sc_addr val;
		if ((val = iter->value(2)) == i1.value) {
			flag = true;
			continue;
		}
		if (info->session->__move_element(val,seg)) {
			dprintf("move_elements: __move_element failed!\n");
			delete iter;
			return RV_ERR_GEN;
		}
	}
	delete iter;
	if (flag) {
		if (info->session->__move_element(i1.value,seg)) {
			dprintf("move_elements: __move_element failed!\n");
			return RV_ERR_GEN;
		}
	}
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_create_unique_segment)
{
	SCP_OPND_REQ_ASSIGN(info, 1, i1);

	sc_retval rv = scp_create_unique_segment(info, i1.opnd);
	if (RV_IS_ERR(rv)) {
		return RV_ERR_GEN;
	}

	return RV_OK;
}
