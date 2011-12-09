
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

/*
 *  SCPx processing module project.
 *
 *  scp_operator.cpp - operator implementation framework
 */

#include "precompiled_p.h"

sc_segment *operators_segment;
sc_session *operators_session;

sc_retval scp_operator_activity::activate(sc_session *, sc_addr _this, sc_addr process, sc_addr, sc_addr)
{
	scp_process_info info;
	if (scp_fill_process_info(process, &info))
		return RV_ERR_GEN;

	scp_operator this_op(info.active_op);
	if (unpack_op(info, this_op))
		return RV_ERR_GEN;

	scp_set_state(info.process, SCP_STATE_THEN);
	return interpret(this_op, &info);
}

sc_retval scp_operator_activity::unpack_op(scp_process_info &info, scp_operator &op)
{
	static const sc_addr* keynode_arrays[] = { pm_numattr, pm_num_set_attrs };
	static const size_t keynode_arrays_len = sizeof(keynode_arrays) / sizeof(sc_addr *);

	sc_addr* results[keynode_arrays_len] = { op.num_opnd_arcs, op.set_opnd_arcs };

	sc_session *s = system_session;

	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(CONSTR_5_f_a_a_a_a, op.sign, SC_A_CONST|SC_POS, 0, SC_A_CONST|SC_POS, 0), true);
	sc_for_each (iter) {
		sc_addr e2 = iter->value(1);
		sc_addr e5 = iter->value(4);

		if (e5) {
			const Content *c = s->get_content_const(e5);
			if (c->type() == TCINT) {
				size_t index = Cont(*c).i - 1;

				if (0 <= index && index < op.get_pre_unpack_count()) {

					for (size_t attr_index = 0; attr_index < keynode_arrays_len; ++attr_index) {

						if (keynode_arrays[attr_index][index + 1] == e5) {
							if (results[attr_index][index]) {
								SCP_ERROR(info.process, "Operator has not-unique attribute " << SC_ID(e5) << ".");
								return RV_ERR_GEN;
							}

							results[attr_index][index] = e2;
							break;
						}
					}
				}
			}
		}
	}

	return RV_OK;
}

sc_retval register_operator(sc_activity * activity, const char *idtf)
{
	sc_addr addr = operators_session->create_el(operators_segment, SC_N_CONST);
	
	if (!addr)
		return RV_ERR_GEN;

	if (operators_session->reimplement(addr, activity))
		return RV_ERR_GEN;

	if (operators_session->set_idtf(addr, idtf))
		return RV_ERR_GEN;

	if (operators_session->gen3_f_a_f(SCP_OPERATOR_TYPE, 0, operators_segment, SC_A_CONST | SC_POS, addr))
		return RV_ERR_GEN;

	return RV_OK;
}

sc_retval scp_operators_init(sc_session * system)
{
	operators_session = system;
	operators_segment = system->open_segment(KEYNODE_SEGURI);

	SCP_REGISTER_OPERATOR(scp_oper_genEl, "genEl");
	SCP_REGISTER_OPERATOR(scp_oper_genElStr3, "genElStr3");
	SCP_REGISTER_OPERATOR(scp_oper_genElStr5, "genElStr5");

	SCP_REGISTER_OPERATOR(scp_oper_searchElStr3, "searchElStr3");
	SCP_REGISTER_OPERATOR(scp_oper_searchElStr5, "searchElStr5");
	SCP_REGISTER_OPERATOR(scp_oper_searchElByIdtf,"searchElByIdtf");
	SCP_REGISTER_OPERATOR(scp_oper_searchSetStr3, "searchSetStr3");
	SCP_REGISTER_OPERATOR(scp_oper_searchSetStr5, "searchSetStr5");
	
	SCP_REGISTER_OPERATOR(scp_oper_selectYStr3, "selectYStr3");
	SCP_REGISTER_OPERATOR(scp_oper_selectNStr3, "selectNStr3");
	SCP_REGISTER_OPERATOR(scp_oper_selectYStr5, "selectYStr5");
	SCP_REGISTER_OPERATOR(scp_oper_selectNStr5, "selectNStr5");
	
	SCP_REGISTER_OPERATOR(scp_oper_eraseEl, "eraseEl");
	SCP_REGISTER_OPERATOR(scp_oper_eraseSetStr3, "eraseSetStr3");
	SCP_REGISTER_OPERATOR(scp_oper_eraseElStr3, "eraseElStr3");
	SCP_REGISTER_OPERATOR(scp_oper_eraseElStr5, "eraseElStr5");

	SCP_REGISTER_OPERATOR(scp_oper_ifType, "ifType");
	SCP_REGISTER_OPERATOR(scp_oper_ifEq, "ifEq");
	SCP_REGISTER_OPERATOR(scp_oper_ifGr, "ifGr");
	SCP_REGISTER_OPERATOR(scp_oper_ifGrEq, "ifGrEq");
	SCP_REGISTER_OPERATOR(scp_oper_ifCoin, "ifCoin");
	SCP_REGISTER_OPERATOR(scp_oper_ifFormCont, "ifFormCont");
	SCP_REGISTER_OPERATOR(scp_oper_ifFormIdtf, "ifFormIdtf");
	SCP_REGISTER_OPERATOR(scp_oper_ifVarAssign, "ifVarAssign");
	SCP_REGISTER_OPERATOR(scp_oper_ifNumber,"ifNumber");
	SCP_REGISTER_OPERATOR(scp_oper_ifString,"ifString");

	SCP_REGISTER_OPERATOR(scp_oper_varAssign, "varAssign");
	SCP_REGISTER_OPERATOR(scp_oper_varErase, "varErase");
	SCP_REGISTER_OPERATOR(scp_oper_contErase, "contErase");
	SCP_REGISTER_OPERATOR(scp_oper_contAssign, "contAssign");
	SCP_REGISTER_OPERATOR(scp_oper_add, "add");
	SCP_REGISTER_OPERATOR(scp_oper_sub, "sub");
	SCP_REGISTER_OPERATOR(scp_oper_mult, "mult");
	SCP_REGISTER_OPERATOR(scp_oper_div, "div");
	SCP_REGISTER_OPERATOR(scp_oper_sin, "sin");
	SCP_REGISTER_OPERATOR(scp_oper_asin, "asin");
	SCP_REGISTER_OPERATOR(scp_oper_cos, "cos");
	SCP_REGISTER_OPERATOR(scp_oper_acos, "acos");
	SCP_REGISTER_OPERATOR(scp_oper_pow, "pow");
	SCP_REGISTER_OPERATOR(scp_oper_floor, "floor");
	SCP_REGISTER_OPERATOR(scp_oper_ceil, "ceil");
	SCP_REGISTER_OPERATOR(scp_oper_to_str, "to_str");
	SCP_REGISTER_OPERATOR(scp_oper_gsub, "gsub");
	SCP_REGISTER_OPERATOR(scp_oper_sys_merge_element, "sys_merge_element");

	SCP_REGISTER_OPERATOR(scp_oper_idtfMove, "idtfMove");
	SCP_REGISTER_OPERATOR(scp_oper_idtfAssign, "idtfAssign");
	SCP_REGISTER_OPERATOR(scp_oper_idtfErase, "idtfErase");

	SCP_REGISTER_OPERATOR(scp_oper_call,"call");
	SCP_REGISTER_OPERATOR(scp_oper_callReturn,"callReturn");
	SCP_REGISTER_OPERATOR(scp_oper_callExtReturn,"callExtReturn");
	SCP_REGISTER_OPERATOR(scp_oper_waitReturn,"waitReturn");
	SCP_REGISTER_OPERATOR(scp_oper_return, "return");
	SCP_REGISTER_OPERATOR(scp_oper_nop,"nop");

	SCP_REGISTER_OPERATOR(scp_oper_print,"print");
	SCP_REGISTER_OPERATOR(scp_oper_printNl,"printNl");
	SCP_REGISTER_OPERATOR(scp_oper_printEl,"printEl");

	SCP_REGISTER_OPERATOR(scp_oper_print_opened_segments,"print_opened_segments");
	SCP_REGISTER_OPERATOR(scp_oper_open_segment,"sys_open_segment");
	SCP_REGISTER_OPERATOR(scp_oper_open_segment_uri,"sys_open_segment_uri");
	SCP_REGISTER_OPERATOR(scp_oper_create_segment,"sys_create_segment");
	SCP_REGISTER_OPERATOR(scp_oper_close_segment,"sys_close_segment");
	SCP_REGISTER_OPERATOR(scp_oper_set_default_segment,"sys_set_default_segment");
	SCP_REGISTER_OPERATOR(scp_oper_get_default_segment,"sys_get_default_segment");
	SCP_REGISTER_OPERATOR(scp_oper_get_autoseg,"sys_get_autosegment");
	SCP_REGISTER_OPERATOR(scp_oper_unlink,"sys_unlink");
	SCP_REGISTER_OPERATOR(scp_oper_spin_segment,"sys_spin_segment");
	SCP_REGISTER_OPERATOR(scp_oper_opendir,"sys_open_dir");
	SCP_REGISTER_OPERATOR(scp_oper_opendir_uri,"sys_open_dir_uri");
	SCP_REGISTER_OPERATOR(scp_oper_getloc,"sys_get_location");
	SCP_REGISTER_OPERATOR(scp_oper_create_tmp_segment,"sys_create_tmp_segment");
	SCP_REGISTER_OPERATOR(scp_oper_create_unique_segment,"sys_create_unique_segment");
	SCP_REGISTER_OPERATOR(scp_oper_move_elements,"sys_set_location");

	SCP_REGISTER_OPERATOR(scp_oper_setevhandler,"sys_set_event_handler");
	SCP_REGISTER_OPERATOR(scp_oper_delevhandler,"sys_delete_event_handler");
	SCP_REGISTER_OPERATOR(scp_oper_halt,"halt");

	SCP_REGISTER_OPERATOR(scp_oper_sys_search, "sys_search");
	SCP_REGISTER_OPERATOR(scp_oper_sys_gen, "sys_gen");

	SCP_REGISTER_OPERATOR(scp_oper_sys_wait, "sys_wait");

	SCP_REGISTER_OPERATOR(scp_oper_sys_breakpoint, "sys_breakpoint");

	return RV_OK;
}

void scp_operators_done()
{
}
