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

#include "precompiled_p.h"

#include <libsc/std_constraints.h>

namespace RGP {

#define DECLARE_CONSTR(name) \
	{ RGP_ ## name , name}

const char *rgp_command_arg_type_names[] = {
	"UNKNOWN",       // RGP_ARG_UNKNOWN
	"SC_TYPE",       // RGP_ARG_SC_TYPE
	"SC_SEGMENT",    // RGP_ARG_SC_SEGMENT
	"SC_ADDR",       // RGP_ARG_SC_ADDR
	"SC_ITERATOR",   // RGP_ARG_SC_ITERATOR
	"SC_CONTENT",    // RGP_ARG_SC_CONTENT
	"SC_ACTIVITY",   // RGP_ARG_SC_ACTIVITY
	"SC_WAIT_TYPE",  // RGP_ARG_SC_WAIT_TYPE
	"SC_WAIT",       // RGP_ARG_SC_WAIT
	"SC_RETVAL",     // RGP_ARG_SC_RETVAL
	"SC_CONSTRAINT", // RGP_ARG_SC_CONSTRAINT
	"SC_INT16",      // RGP_ARG_SC_INT16
	"SC_INT32",      // RGP_ARG_SC_INT32
	"SC_BOOLEAN",    // RGP_ARG_SC_BOOLEAN
	"SC_STRING"      // RGP_ARG_SC_STRING
};

rgp_constraint_info rgp_constraint_infos[] = {
	{ RGP_CONSTR_UNKNOWN, -1 },
	DECLARE_CONSTR(CONSTR_ALL_INPUT),
	DECLARE_CONSTR(CONSTR_ALL_OUTPUT),
	DECLARE_CONSTR(CONSTR_3_f_a_a),
	DECLARE_CONSTR(CONSTR_3_f_a_f),
	DECLARE_CONSTR(CONSTR_3_a_a_f),
	DECLARE_CONSTR(CONSTR_5_f_a_a_a_a),
	DECLARE_CONSTR(CONSTR_5_f_a_a_a_f),
	DECLARE_CONSTR(CONSTR_5_f_a_f_a_a),
	DECLARE_CONSTR(CONSTR_5_f_a_f_a_f),
	DECLARE_CONSTR(CONSTR_5_a_a_a_a_f),
	DECLARE_CONSTR(CONSTR_5_a_a_f_a_a),
	DECLARE_CONSTR(CONSTR_5_a_a_f_a_f),
	DECLARE_CONSTR(CONSTR_3l2_f_a_a_a_f),
	DECLARE_CONSTR(CONSTR_ON_SEGMENT),
	DECLARE_CONSTR(CONSTR_ORD_BIN_CONN1),
	DECLARE_CONSTR(CONSTR_ORD_BIN_CONN2)
};

#define DECLARE_CMD(command_id, arg_count, ...) \
	{ command_id, #command_id , arg_count, false, -1, { __VA_ARGS__ } }

#define DECLARE_CMD_WITH_REST(command_id, arg_count, max_rest_arg_count, ...) \
	{ command_id, #command_id, arg_count, true, max_rest_arg_count, { __VA_ARGS__ } }

rgp_command_info rgp_command_infos[] = {
	// Requests
	DECLARE_CMD(RGP_REQ_LOGIN, 1, RGP_ARG_SC_BOOLEAN),
	DECLARE_CMD(RGP_REQ_CLOSE, 0),

	DECLARE_CMD(RGP_REQ_CREATE_SEGMENT, 1, RGP_ARG_SC_STRING),
	DECLARE_CMD(RGP_REQ_OPEN_SEGMENT, 1, RGP_ARG_SC_STRING),

	DECLARE_CMD(RGP_REQ_GET_IDTF, 1, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_SET_IDTF, 2, RGP_ARG_SC_ADDR, RGP_ARG_SC_STRING),
	DECLARE_CMD(RGP_REQ_ERASE_IDTF, 1, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_FIND_BY_IDTF, 2, RGP_ARG_SC_STRING, RGP_ARG_SC_SEGMENT),

	DECLARE_CMD(RGP_REQ_CREATE_EL, 2, RGP_ARG_SC_SEGMENT, RGP_ARG_SC_TYPE),
	DECLARE_CMD(RGP_REQ_GEN3_F_A_F, 4, RGP_ARG_SC_ADDR, RGP_ARG_SC_SEGMENT, RGP_ARG_SC_TYPE, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_GEN5_F_A_F_A_F, 7, RGP_ARG_SC_ADDR, RGP_ARG_SC_SEGMENT, RGP_ARG_SC_TYPE, RGP_ARG_SC_ADDR, RGP_ARG_SC_SEGMENT, RGP_ARG_SC_TYPE, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_ERASE_EL, 1, RGP_ARG_SC_ADDR),

	DECLARE_CMD(RGP_REQ_MERGE_EL, 2, RGP_ARG_SC_ADDR, RGP_ARG_SC_ADDR),

	DECLARE_CMD(RGP_REQ_SET_EL_BEGIN, 2, RGP_ARG_SC_ADDR, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_SET_EL_END, 2, RGP_ARG_SC_ADDR, RGP_ARG_SC_ADDR),

	DECLARE_CMD(RGP_REQ_GET_EL_BEGIN, 1, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_GET_EL_END, 1, RGP_ARG_SC_ADDR),

	DECLARE_CMD(RGP_REQ_GET_EL_TYPE, 1, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_CHANGE_EL_TYPE, 2, RGP_ARG_SC_ADDR, RGP_ARG_SC_TYPE),

	DECLARE_CMD(RGP_REQ_GET_EL_CONTENT, 1, RGP_ARG_SC_ADDR),
	DECLARE_CMD(RGP_REQ_SET_EL_CONTENT, 2, RGP_ARG_SC_ADDR, RGP_ARG_SC_CONTENT),

	DECLARE_CMD_WITH_REST(RGP_REQ_CREATE_ITERATOR, 1, 11, RGP_ARG_SC_CONSTRAINT),
	DECLARE_CMD_WITH_REST(RGP_REQ_NEXT_ITERATOR, 1, 5, RGP_ARG_SC_ITERATOR),
	DECLARE_CMD(RGP_REQ_ERASE_ITERATOR, 1, RGP_ARG_SC_ITERATOR),

	DECLARE_CMD_WITH_REST(RGP_REQ_ATTACH_WAIT, 1, 3, RGP_ARG_SC_WAIT_TYPE),
	DECLARE_CMD(RGP_REQ_DETACH_WAIT, 1, RGP_ARG_SC_WAIT),

	DECLARE_CMD(RGP_REQ_REIMPLEMENT, 1, RGP_ARG_SC_ADDR),

	DECLARE_CMD(RGP_REQ_ACTIVATE, 4, RGP_ARG_SC_ADDR, RGP_ARG_SC_ADDR, RGP_ARG_SC_ADDR, RGP_ARG_SC_ADDR),

	DECLARE_CMD(RGP_REQ_GET_SEG_INFO, 1, RGP_ARG_SC_SEGMENT),
	DECLARE_CMD(RGP_REQ_GET_EL_INFO, 1, RGP_ARG_SC_ADDR),

	DECLARE_CMD_WITH_REST(RGP_REQ_ACTIVATE_WAIT, 2, 3, RGP_ARG_SC_WAIT, RGP_ARG_SC_WAIT_TYPE),

	// Replies
	DECLARE_CMD_WITH_REST(RGP_REP_RETURN, 1, -1, RGP_ARG_SC_RETVAL)
};

} // namespace RGP
