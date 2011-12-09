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
 * @file scp_core.h
 * @brief Interface SCP-interpretator's core.
 * @ingroup libpm
 */

#ifndef __SCP_CORE_H__
#define __SCP_CORE_H__

#include <libsc/libsc.h>

#include <libsc/sc_print_utils.h>
#include <libsc/sc_utils.h>
#include <libsc/sc_tuple.h>

#include <libsc/sc_atoms.h>

#include <map>
#include <sstream>
#include <stdexcept>

/// @addtogroup libpm
/// @{

extern LIBPM_API sc_segment *scp_segment;

extern LIBPM_API int stop_on_error;

LIBPM_API void scp_core_init(sc_session *system_session);

class scp_error : public std::logic_error
{
public:
	explicit scp_error(const std::string &_msg) : std::logic_error(_msg)
	{
	}
};

/// SCP-process info structure.
struct LIBPM_API scp_process_info
{
	sc_session *session;     ///< Session with which SCP-process works.

	sc_addr process;         ///< Sign of SCP-process.

	sc_addr var_value;       ///< Binary relation "var ==> value".

	sc_addr auto_dirent;     ///< Auto-segment sign of SCP-process.

	sc_addr defaultseg_sign; ///< Default segment sign of SCP-process.

	sc_addr program;         ///< Interpreted SCP-program.

	sc_addr vars;            ///< Set of variables interpreted SCP-program.

	sc_addr consts;          ///< Set of constants interpreted SCP-program.

	sc_addr active_op;       ///< Active interpreted SCP-operator.

	/// Returns father SCP-process. 0 if fails.
	static sc_addr get_father(sc_addr process)
	{
		return sc_tup::at(system_session, process, FATHER_);
	}

	/// Returns opened segments set of SCP-process. 0 if fails.
	static sc_addr get_opened_segs(sc_addr process)
	{
		return sc_tup::at(system_session, process, OPENED_SEGMENTS_);
	}

	/// Returns default segment sign of SCP-process. 0 if fails.
	static sc_addr get_default_seg(sc_addr process)
	{
		return sc_tup::at(system_session, process, DEFAULT_SEG_);
	}

	/// Sets default segment sign for SCP-process.
	static void set_default_seg(sc_addr process, sc_addr defsegsign);

	/// Returns active interpreted SCP-operator for SCP-process. 0 if fails.
	static sc_addr get_active_op(sc_addr process, sc_addr *active_op_arc=0)
	{
		return sc_tup::at(system_session, process, ACTIVE_, active_op_arc);
	}

	/// Returns interpreted SCP-program for SCP-process. 0 if fails.
	static sc_addr get_program(sc_addr process)
	{
		return sc_tup::at(system_session, process, PROGRAM_);
	}

	/// Returns tracer for SCP-process. 0 if fails.
	static sc_addr get_tracer(sc_addr process)
	{
		return sc_tup::at(system_session, process, PTRACE_);
	}

	/// Sets tracer for SCP-process.
	static void set_tracer(sc_addr process, sc_addr tracer)
	{
		gen5_f_cpa_f_cpa_f(system_session, process, 0, tracer, 0, PTRACE_);
	}

	/// Checks if SCP-process is traced.
	static bool is_traced(sc_addr process);

	/// Sets SCP-process traced flag.
	static void set_traced(sc_addr process, bool traced);

	/// Checks if SCP-process is step traced.
	static bool is_step_tracing(sc_addr process);

	/// Sets SCP-process step traced flag.
	static void set_step_tracing(sc_addr process, bool step_tracing);

	/// Sets SCP-process executing error string.
	static sc_retval set_error(sc_addr process, const sc_string &error_str);
};

/// Returns <code>init_</code> SCP-operator of SCP-program.
LIBPM_API sc_addr get_init_op(sc_addr program);

/// Returns next SCP-operator for SCP-operator @p op with branch which has attribute @p branch_attr.
LIBPM_API sc_addr get_next_op(sc_addr op, sc_addr branch_attr);


LIBPM_API bool has_op_debug_info(sc_addr op);

LIBPM_API size_t get_op_line(sc_addr op);
LIBPM_API sc_addr get_op_type(sc_addr op);

LIBPM_API sc_string get_addr_full_uri(sc_addr addr);
LIBPM_API sc_string get_addr_idtf(sc_addr);

#define FULL_IDTF(addr) get_addr_full_uri(addr)

#ifdef _DEBUG
	#define SCP_ERROR(process, msg) \
	{ \
		std::stringstream _buf; \
		_buf << msg << "\n"; \
		scp_process_info::set_error(process, _buf.str()); \
	}
#else
	#define SCP_ERROR(process, msg) \
	{ \
		std::stringstream _buf; \
		_buf << msg << "\n"; \
		scp_process_info::set_error(process, _buf.str()); \
	}
#endif

#define SCP_ERROR_F(process, ...) \
	SCP_ERROR(process, print_f(__VA_ARGS__));

typedef sc_uint scp_state;
#define SCP_STATE_THEN      1
#define SCP_STATE_ELSE      2
#define SCP_STATE_REPEAT    4
#define SCP_STATE_CONTROL_MASK (SCP_STATE_THEN|SCP_STATE_ELSE|SCP_STATE_REPEAT)
#define SCP_STATE_SLEEP     8
#define SCP_STATE_RUN       16
#define SCP_STATE_SCHED_MASK (SCP_STATE_SLEEP|SCP_STATE_RUN)
#define SCP_STATE_DEAD      32
#define SCP_STATE_ERROR     64

// States dead & error will kill process on second step
// Error state implies dead state
// Repeat state means that current operator should be repeated once more
//  Repeat state is automagically cleared by interpreter
LIBPM_API void scp_set_state(sc_addr process,scp_state);
LIBPM_API bool scp_check_state(sc_session *s,sc_addr process,sc_addr state_node);

/// Unpack tuple of scp-process @p process into @p pi.
LIBPM_API sc_retval scp_fill_process_info(sc_addr process,scp_process_info *);

LIBPM_API sc_retval scp_create_process(sc_session *s, sc_addr prg, sc_addr father, sc_addr prms, sc_addr *process, sc_addr descr=0);

/// @see scp_create_process
LIBPM_API sc_retval scp_create_process(sc_session *s, sc_addr prg, sc_addr father, addr_vector &prms, sc_addr *process, sc_addr descr=0);

/// Call in process @p process_info scp-program @p program with parameters tuple @p prms.
/// @note Push current process context into stack and create new context with parameters binding.
LIBPM_API sc_retval scp_call_program(scp_process_info *info, sc_addr prg, sc_addr prms);

/// @see scp_call_program
LIBPM_API sc_retval scp_call_program(scp_process_info *info, sc_addr prg, addr_vector &prms);

/// Call PM external program with scp-parameters.
LIBPM_API sc_retval scp_call_ext_program(scp_process_info *info, sc_addr prg, sc_addr prms);

/// @see scp_call_ext_program
LIBPM_API sc_retval scp_call_ext_program(scp_process_info *info, sc_addr prg, addr_vector &prms);

LIBPM_API sc_retval finalize_process(scp_process_info *);
LIBPM_API sc_retval scp_process_return(scp_process_info *info, scp_state return_state=SCP_STATE_THEN);
LIBPM_API sc_retval scp_sleep_on_child(sc_addr process,sc_addr child);

// dont use this hack!!!
LIBPM_API sc_addr create_orphaned_son(sc_session *session, sc_addr program, sc_addr on_death,
				sc_addr *pfather, sc_addr prms[], bool _consts[], size_t count);
LIBPM_API sc_addr construct_fictive_process(sc_session *session,
				  size_t count_const, sc_addr consts[],
				  size_t count_var, sc_addr vars[],
				  sc_addr *pprg);
LIBPM_API void	erase_fictive_process(sc_addr process);


LIBPM_API sc_retval scp_spawn_process(sc_session *s,sc_addr prg, sc_addr prm, sc_addr attrs[], int attrs_max, sc_addr *res);

LIBPM_API
DECLARE_SC_ATOM(step_event);

LIBPM_API
DECLARE_SC_ATOM(step_after_event);

LIBPM_API
DECLARE_SC_ATOM(call_event);

LIBPM_API
DECLARE_SC_ATOM(return_event);

LIBPM_API
DECLARE_SC_ATOM(error_event);

LIBPM_API
DECLARE_SC_ATOM(child_event);

LIBPM_API
DECLARE_SC_ATOM(death_event);

#define PROCESS_STEP_EVENT       SC_ATOM(step_event)
#define PROCESS_STEP_AFTER_EVENT SC_ATOM(step_after_event)
#define PROCESS_CALL_EVENT       SC_ATOM(call_event)
#define PROCESS_RETURN_EVENT     SC_ATOM(return_event)
#define PROCESS_ERROR_EVENT      SC_ATOM(error_event)
#define PROCESS_CHILD_EVENT      SC_ATOM(child_event)
#define PROCESS_DEATH_EVENT      SC_ATOM(death_event)

/// @}

#endif // __SCP_CORE_H__
