
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
 * @file scp_operator.h
 * @brief SCP-operators implementation framework.
 * @ingroup libpm
 */

/**
 * @addtogroup libpm
 * @{
 */

#ifndef __LIBPM_SCP_OPERATOR_H_INCLUDED__
#define __LIBPM_SCP_OPERATOR_H_INCLUDED__

#include <libsc/libsc.h>

#include <map>

#include <libpm/pm_sched.h>
#include "scp_core.h"
#include "scp_var.h"
#include "scp_operand.h"

#if 0
// TODO: rethink about segment accessebility for SCP-operands
/// Checks if segment of SCP-operand is accessible. For SCP-variables skips checking.
#define SCP_CHECK_OPND_ACCESSIBLE(info, opnd_var) \
	if (!opnd_var.var && !info->session->is_segment_opened(opnd_var.opnd->seg)) { \
		SCP_ERROR_F(info->process, "Segment \"%s\" for operand %s not opened", \
			opnd_var.opnd->seg->get_full_uri().c_str(), opnd_var.get_name()); \
		return RV_ERR_GEN; \
	}

// TODO: rethink about segment accessebility for values of SCP-operands
#define SCP_CHECK_OPND_VALUE_ACCESSIBLE(info, opnd_var) \
	if (!info->session->is_segment_opened(opnd_var.value->seg)) { \
		SCP_ERROR_F(info->process, "Not opened segment \"%s\" for value of operand %s", \
		opnd_var.value->seg->get_full_uri().c_str(), opnd_var.get_name()); \
		return RV_ERR_GEN; \
	} \

#else

#define SCP_CHECK_OPND_ACCESSIBLE(info, opnd_var)
#define SCP_CHECK_OPND_VALUE_ACCESSIBLE(info, opnd_var)

#endif

/// Checks if SCP-operand is SCP-variable.
#define SCP_CHECK_OPND_VAR(info, opnd_var) \
	if (!opnd_var.var) { \
		SCP_ERROR_F(info->process, "Operand %s must be variable.", opnd_var.get_name()); \
		return RV_ERR_GEN; \
	}

/// Checks if SCP-operand has fixed_, value and value in accessible segment.
#define SCP_CHECK_OPND_FIXED(info, opnd_var) \
	if (opnd_var.assign) { \
		SCP_ERROR_F(info->process, "Operand %s must be fixed", opnd_var.get_name()); \
		return RV_ERR_GEN; \
	} else if (!opnd_var.value) { \
		SCP_ERROR_F(info->process, "Operand %s must have value.", opnd_var.get_name()); \
		return RV_ERR_GEN; \
	} else { \
		SCP_CHECK_OPND_VALUE_ACCESSIBLE(info, opnd_var); \
	}

/// Checks if SCP-operand has assign_ and it's SCP-variable.
#define SCP_CHECK_OPND_ASSIGN(info, opnd_var) \
	if (!opnd_var.assign) { \
		SCP_ERROR_F(info->process, "Operand %s must have assign_.", opnd_var.get_name()); \
		return RV_ERR_GEN; \
	} \
	SCP_CHECK_OPND_VAR(info, opnd_var);

/// Checks if SCP-has assign_ (it's SCP-variable) or fixed_ (it has value in accessible segment).
#define SCP_CHECK_OPND_ASSIGN_FIXED(info, opnd_var) \
	if (opnd_var.assign) { \
		SCP_CHECK_OPND_VAR(info, opnd_var); \
	} else { \
		SCP_CHECK_OPND_FIXED(info, opnd_var); \
	}

#define SCP_CREATE_ASSIGN_OPND(info, opnd_var) \
	if (opnd_var.assign) { \
		if (opnd_var.create_element()) { \
			return RV_ERR_GEN; \
		} \
	}

#define SCP_CHECK_OPND_EXCESSIVE(info, opnd_attr) \
	if (sc_tup::at(info->session, info->active_op, opnd_attr)) { \
		SCP_ERROR_F(info->process, "Excessive operand %s detected.", system_session->get_idtf(opnd_attr).c_str()); \
		return RV_ERR_GEN; \
	}

/// Declare and fill info structure with name @p opnd_name for required scp-operand @p opnd_index _.
#define SCP_OPND_REQ(process_info, opnd_index, opnd_name) \
	scp_opnd_info opnd_name; \
	if (_this.get_pre_unpack_count() > opnd_index) { \
		opnd_name.search_ord(process_info, opnd_index, _this.get_num_opnd_arc(opnd_index)); \
	} else { \
		opnd_name.search_ord(process_info, opnd_index); \
	} \
	if (!opnd_name.opnd) { \
		SCP_ERROR_F(process_info->process, "Not found required operand %s", opnd_name.get_name()); \
		return RV_ERR_GEN; \
	} \
	SCP_CHECK_OPND_ACCESSIBLE(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for required scp-operand @p opnd_index _.
/// Operand must be scp-variable.
#define SCP_OPND_REQ_VAR(info, opnd_index, opnd_name) \
	SCP_OPND_REQ(info, opnd_index, opnd_name); \
	SCP_CHECK_OPND_VAR(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for required scp-operand @p opnd_index _.
/// Operand must be fixed.
#define SCP_OPND_REQ_FIXED(info, opnd_index, opnd_name) \
	SCP_OPND_REQ(info, opnd_index, opnd_name); \
	SCP_CHECK_OPND_FIXED(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for required scp-operand @p opnd_index _.
/// Operand must be assign.
#define SCP_OPND_REQ_ASSIGN(info, opnd_index, opnd_name) \
	SCP_OPND_REQ(info, opnd_index, opnd_name); \
	SCP_CHECK_OPND_ASSIGN(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for required scp-operand @p opnd_index _.
/// Operand must be fixed or assign. Does not create operand value for assign scp-operand.
#define SCP_OPND_REQ_ASSIGN_FIXED(info, opnd_index, opnd_name) \
	SCP_OPND_REQ(info, opnd_index, opnd_name); \
	SCP_CHECK_OPND_ASSIGN_FIXED(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for required scp-operand @p opnd_index _.
/// Operand must be fixed or assign. Creates operand value for assign scp-operand.
#define SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, opnd_index, opnd_name) \
	SCP_OPND_REQ_ASSIGN_FIXED(info, opnd_index, opnd_name); \
	SCP_CREATE_ASSIGN_OPND(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for optional scp-operand @p opnd_index _.
#define SCP_OPND_OPT(process_info, opnd_index, opnd_name) \
	scp_opnd_info opnd_name; \
	if (_this.get_pre_unpack_count() > opnd_index) { \
		opnd_name.search_ord(process_info, opnd_index, _this.get_num_opnd_arc(opnd_index)); \
	} else { \
		opnd_name.search_ord(process_info, opnd_index); \
	}

/// Declare and fill info structure with name @p opnd_name for optional scp-operand @p opnd_index _.
/// Operand must be scp-variable.
#define SCP_OPND_OPT_VAR(info, opnd_index, opnd_name) \
	SCP_OPND_OPT(info, opnd_index, opnd_name); \
	if (opnd_name.opnd) \
		SCP_CHECK_OPND_VAR(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for optional scp-operand @p opnd_index _.
/// Operand must be fixed.
#define SCP_OPND_OPT_FIXED(process_info, opnd_index, opnd_name) \
	SCP_OPND_OPT(process_info, opnd_index, opnd_name); \
	if (opnd_name.opnd) \
		SCP_CHECK_OPND_FIXED(process_info, opnd_name); \

/// Declare and fill info structure with name @p opnd_name for optional scp-operand @p opnd_index _.
/// Operand must be fixed or assign. Creates operand value for assign scp-operand.
#define SCP_OPND_OPT_ASSIGN_FIXED_CREATE(process_info, opnd_index, opnd_name) \
	SCP_OPND_OPT(process_info, opnd_index, opnd_name); \
	if (opnd_name.opnd) { \
		SCP_CHECK_OPND_ASSIGN_FIXED(info, opnd_name); \
		SCP_CREATE_ASSIGN_OPND(process_info, opnd_name); \
	}

/// Declare and fill info structure with name @p opnd_name for optional scp-operand set @p opnd_index _.
/// Boolean variable @p flag indicates existence of operand.
#define SCP_OPNDSET_OPT(process_info, opnd_index, opnd_name) \
	scp_opnd_info opnd_name; \
	if (_this.get_pre_unpack_count() > opnd_index) { \
		opnd_name.search_set(process_info, opnd_index, _this.get_set_opnd_arc(opnd_index)); \
	} else { \
		opnd_name.search_set(process_info, opnd_index); \
	}

/// Declare and fill info structure with name @p opnd_name for optional scp-operand set @p opnd_index _.
/// Boolean variable @p flag indicates existence of operand.
/// Operand must be fixed or assign. Does not create operand value for assign scp-operand.
#define SCP_OPNDSET_OPT_ASSIGN_FIXED(process_info, opnd_index, opnd_name) \
	SCP_OPNDSET_OPT(process_info, opnd_index, opnd_name); \
	if (opnd_name.opnd) \
		SCP_CHECK_OPND_ASSIGN_FIXED(info, opnd_name);

/// Declare and fill info structure with name @p opnd_name for optional scp-operand set @p opnd_index _.
/// Operand must be fixed or assign. Creates operand value for assign scp-operand.
#define SCP_OPNDSET_OPT_ASSIGN_FIXED_CREATE(process_info, opnd_index, opnd_name) \
	SCP_OPNDSET_OPT(process_info, opnd_index, opnd_name); \
	if (opnd_name.opnd) { \
		SCP_CHECK_OPND_ASSIGN_FIXED(info, opnd_name); \
		SCP_CREATE_ASSIGN_OPND(process_info, opnd_name); \
	}

/// Declare and fill pointer to #sc_single_filter with name @p filter for optional segc @p opnd_index _.
/// Declare auto-ptr for @p filter.
#define SCP_MULTI_SEGC_FILTER_OPT(process_info, opnd_index, filter) \
	sc_single_filter *filter = NULL; \
	if (RV_IS_ERR(get_multi_segc_filter(process_info, opnd_index, &filter))) { \
		return RV_ERR_GEN; \
	} \
	std::auto_ptr<sc_single_filter> auto_##filter(filter);



class scp_operator_activity : public sc_activity
{
public:
	sc_retval init(sc_addr _this) { return RV_OK; }

	void done() { delete this; }

	sc_retval activate(sc_session *, sc_addr _this, sc_addr process,sc_addr, sc_addr);

protected:
	/// Represent unpacked scp-operator tuple.
	class scp_operator
	{
		friend class scp_operator_activity;
	public:
		scp_operator(sc_addr _sign)
			: sign(_sign)
		{
			std::fill_n(num_opnd_arcs, pre_unpack_count, SCADDR_NIL);
			std::fill_n(set_opnd_arcs, pre_unpack_count, SCADDR_NIL);
		}

		size_t get_pre_unpack_count() const { return pre_unpack_count; }

		sc_addr get_num_opnd_arc(size_t i) { return num_opnd_arcs[i - 1]; }

		sc_addr get_set_opnd_arc(size_t i) { return set_opnd_arcs[i - 1]; }

	private:
		sc_addr sign;

		static const size_t pre_unpack_count = 5;
		sc_addr num_opnd_arcs[pre_unpack_count];
		sc_addr set_opnd_arcs[pre_unpack_count];
	};

	virtual sc_retval interpret(scp_operator &_this, scp_process_info *info) = 0;

private:
	static sc_retval unpack_op(scp_process_info &info, scp_operator &op);
};

#define SCP_DECLARE_OPERATOR_FULL(name,father) \
	class name : public father \
	{ \
	public: \
			static const char *get_name() { return #name; } \
			sc_retval interpret(scp_operator &, scp_process_info *); \
	};

#define SCP_DECLARE_OPERATOR(name) \
	SCP_DECLARE_OPERATOR_FULL(name, scp_operator_activity);

#define SCP_OPERATOR_IMPL(name) \
	sc_retval name::interpret(scp_operator &_this, scp_process_info *info)

LIBPM_API sc_retval scp_operators_init(sc_session *system);
LIBPM_API void scp_operators_done();


LIBPM_API sc_retval register_operator(sc_activity * activity, const char *idtf);

#define SCP_REGISTER_OPERATOR(name,idtf) \
	do { \
		if (register_operator(new name(),idtf)) \
			return RV_ERR_GEN; \
	} while(0)

extern std::map<sc_addr, sc_string> scp_operators;

extern LIBPM_API sc_segment *operators_segment;
extern LIBPM_API sc_session *operators_session;

extern LIBPM_API bool no_print_el;

// @name Change operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_contAssign)
SCP_DECLARE_OPERATOR(scp_oper_contErase)
SCP_DECLARE_OPERATOR(scp_oper_add)
SCP_DECLARE_OPERATOR(scp_oper_sub)
SCP_DECLARE_OPERATOR(scp_oper_mult)
SCP_DECLARE_OPERATOR(scp_oper_div)
SCP_DECLARE_OPERATOR(scp_oper_sin)
SCP_DECLARE_OPERATOR(scp_oper_asin)
SCP_DECLARE_OPERATOR(scp_oper_cos)
SCP_DECLARE_OPERATOR(scp_oper_acos)
SCP_DECLARE_OPERATOR(scp_oper_pow)
SCP_DECLARE_OPERATOR(scp_oper_floor)
SCP_DECLARE_OPERATOR(scp_oper_ceil)
SCP_DECLARE_OPERATOR(scp_oper_to_str)
SCP_DECLARE_OPERATOR(scp_oper_gsub)
SCP_DECLARE_OPERATOR(scp_oper_sys_merge_element)
SCP_DECLARE_OPERATOR(scp_oper_varAssign)
SCP_DECLARE_OPERATOR(scp_oper_varErase)
// @}

// @name Idtf operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_idtfMove)
SCP_DECLARE_OPERATOR(scp_oper_idtfAssign)
SCP_DECLARE_OPERATOR(scp_oper_idtfErase)
// @}

// @name Gen operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_genEl)
SCP_DECLARE_OPERATOR(scp_oper_genElStr3)
SCP_DECLARE_OPERATOR(scp_oper_genElStr5)
// @}

// @name Search operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_searchElStr3)
SCP_DECLARE_OPERATOR(scp_oper_searchElStr5)
SCP_DECLARE_OPERATOR(scp_oper_searchSetStr3)
SCP_DECLARE_OPERATOR(scp_oper_searchSetStr5)
SCP_DECLARE_OPERATOR(scp_oper_searchElByIdtf)
// @}

// @name Select operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_selectYStr3)
SCP_DECLARE_OPERATOR(scp_oper_selectYStr5)
SCP_DECLARE_OPERATOR(scp_oper_selectNStr3)
SCP_DECLARE_OPERATOR(scp_oper_selectNStr5)
// @}

// @name Erase operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_eraseEl)
SCP_DECLARE_OPERATOR(scp_oper_eraseElStr3)
SCP_DECLARE_OPERATOR(scp_oper_eraseElStr5)
SCP_DECLARE_OPERATOR(scp_oper_eraseSetStr3)
// @}

// @name If operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_ifFormCont)
SCP_DECLARE_OPERATOR(scp_oper_ifEq)
SCP_DECLARE_OPERATOR(scp_oper_ifGr)
SCP_DECLARE_OPERATOR(scp_oper_ifGrEq)
SCP_DECLARE_OPERATOR(scp_oper_ifType)
SCP_DECLARE_OPERATOR(scp_oper_ifCoin)
SCP_DECLARE_OPERATOR(scp_oper_ifFormIdtf)
SCP_DECLARE_OPERATOR(scp_oper_ifNumber)
SCP_DECLARE_OPERATOR(scp_oper_ifString)
SCP_DECLARE_OPERATOR(scp_oper_ifVarAssign)
// @}

// @name Control operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_call)
SCP_DECLARE_OPERATOR(scp_oper_callReturn)
SCP_DECLARE_OPERATOR(scp_oper_callExtReturn)
SCP_DECLARE_OPERATOR(scp_oper_return)
SCP_DECLARE_OPERATOR(scp_oper_return_then)
SCP_DECLARE_OPERATOR(scp_oper_return_else)
SCP_DECLARE_OPERATOR(scp_oper_halt)
SCP_DECLARE_OPERATOR(scp_oper_nop)
SCP_DECLARE_OPERATOR(scp_oper_sys_breakpoint)
SCP_DECLARE_OPERATOR(scp_oper_waitReturn)
// @}


// @name Event operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_setevhandler)
SCP_DECLARE_OPERATOR(scp_oper_delevhandler)
SCP_DECLARE_OPERATOR(scp_oper_sys_wait)
// @}

// @name Segment operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_open_segment)
SCP_DECLARE_OPERATOR(scp_oper_open_segment_uri)
SCP_DECLARE_OPERATOR(scp_oper_close_segment)
SCP_DECLARE_OPERATOR(scp_oper_create_segment)
SCP_DECLARE_OPERATOR(scp_oper_get_default_segment)
SCP_DECLARE_OPERATOR(scp_oper_set_default_segment)
SCP_DECLARE_OPERATOR(scp_oper_unlink)
SCP_DECLARE_OPERATOR(scp_oper_get_autoseg)
SCP_DECLARE_OPERATOR(scp_oper_spin_segment)
SCP_DECLARE_OPERATOR(scp_oper_opendir)
SCP_DECLARE_OPERATOR(scp_oper_opendir_uri)
SCP_DECLARE_OPERATOR(scp_oper_getloc)
SCP_DECLARE_OPERATOR(scp_oper_create_tmp_segment)
SCP_DECLARE_OPERATOR(scp_oper_create_unique_segment)
SCP_DECLARE_OPERATOR(scp_oper_move_elements)
// @}

// @name Print operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_print)
SCP_DECLARE_OPERATOR_FULL(scp_oper_printNl, scp_oper_print)
SCP_DECLARE_OPERATOR(scp_oper_printEl)
SCP_DECLARE_OPERATOR(scp_oper_print_opened_segments)
// @}

// @name Pattern operators
// @{
SCP_DECLARE_OPERATOR(scp_oper_sys_search)
SCP_DECLARE_OPERATOR(scp_oper_sys_gen)
// @}

#endif // __LIBPM_SCP_OPERATOR_H_INCLUDED__

/**@}*/
