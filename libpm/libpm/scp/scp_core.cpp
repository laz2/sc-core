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
 * @file scp_core.cpp
 * @brief SCP-interpretator's core.
 * @ingroup libpm
 */

/// @addtogroup libpm
/// @{

#include "precompiled_p.h"

#define __SCP_KEYNODES_COMPILATION
#include "scp_keynodes.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define ___LINE_AS_STRING(a) #a
#define __LINE_AS_STRING ___LINE_AS_STRING(__LINE__)

#ifdef VERBOSE_ERRORS
	#define BUG_ERROR(rv) \
	do { \
	fprintf(stderr,"BUG! in file %s at line %d\n",__FILE__,__LINE__); \
	SC_ABORT(); \
	} while(0)
#else
	#define R_ERROR(str) return RV_ERR_GEN
	#define BUG_ERROR(rv) return rv
#endif //VERBOSE_ERRORS

sc_segment *scp_segment = 0;

DEFINE_SC_ATOM(step_event);
DEFINE_SC_ATOM(step_after_event);
DEFINE_SC_ATOM(call_event);
DEFINE_SC_ATOM(return_event);
DEFINE_SC_ATOM(error_event);
DEFINE_SC_ATOM(child_event);
DEFINE_SC_ATOM(death_event);

// this version uses system_session
static sc_retval search_5_f_cpa_cna_cpa_f(sc_addr e1, sc_addr *e2, sc_addr *e3, sc_addr *e4, sc_addr e5)
{
	return search_5_f_cpa_cna_cpa_f(system_session, e1, e2, e3, e4, e5);
}

int stop_on_error;

void scp_core_init(sc_session *system)
{
	system->mkdir("/proc/scp");
	scp_segment = system->create_segment("/proc/scp/core");
	scp_segment->ring_0 = true;

	scp::keynodes_segment = create_segment_full_path(system, "/proc/scp/keynode");
	create_keynodes(system, scp::keynodes_count, scp::keynodes_uris, scp::keynodes);
}

/**
 * Ищет класс для scp-оператора.
 */
inline sc_addr get_op_type(sc_addr op)
{
	sc_addr op_class = 0;
	search_3l2_f_cpa_cna_cpa_f(system_session, SCP_OPERATOR_TYPE, 0, &op_class, 0, op);
	return op_class;
}

/**
 * Ищет следующий scp-оператор, переход к которому помечен переданным атрибутом.
 *
 * @param active_op активный оператор.
 * @param branch_attr атрибут, которым помечен переход.
 * @return найденный следующий оператор.
 */
inline sc_addr get_next_op(sc_addr active_op, sc_addr branch_attr)
{
	return sc_tup::at(system_session, active_op, branch_attr);
}

bool has_op_debug_info(sc_addr op)
{
	return sc_tup::at(system_session, op, OPERATOR_LINE_) && sc_tup::at(system_session, op, OPERATOR_FILE_);
}

size_t get_op_line(sc_addr op)
{
	sc_addr op_line = sc_tup::at(system_session, op, OPERATOR_LINE_);
	size_t op_line_value = -1;

	if (op_line) {
		const Content *c = system_session->get_content_const(op_line);

		if (c->type() == TCREAL)
			op_line_value = size_t(((Cont) *c).r);
		else if (c->type() == TCINT)
			op_line_value = size_t(((Cont) *c).i);
		else
			return RV_ERR_GEN;
	}

	return op_line_value;
}

inline const char * get_op_filename(sc_addr op)
{
	const char *op_filename = 0;

	sc_addr op_file = sc_tup::at(system_session, op, OPERATOR_FILE_);
	if (op_file) {
		const Content *c = system_session->get_content_const(op_file);
		op_filename = *c;
	}

	return op_filename;
}

sc_string get_op_file_str(sc_addr op)
{
	const char *op_file_name;
	std::stringstream op_file_stream;

	op_file_name = get_op_filename(op);
	if (op_file_name) {
		op_file_stream << "\"" << op_file_name << "\"";
	} else {
		op_file_stream << "\"UNKNOWN\"";
	}

	return op_file_stream.str();
}

sc_string get_op_line_str(sc_addr op)
{
	std::stringstream op_line_stream;

	if (has_op_debug_info(op)) {
		op_line_stream << get_op_line(op);
	} else {
		op_line_stream << "\"UNKNOWN\"";
	}

	return op_line_stream.str();
}

static sc_retval print_proc_info(sc_addr process, sc_addr op)
{
	sc_addr program = scp_process_info::get_program(process);

	assert(program);

	sc_string progid = get_full_uri(system_session, program);
	sc_string operid = system_session->get_idtf(op);
	sc_addr op_type = get_op_type(op);

	printf("[SCP] ");

	sc_addr cur_ctx_list = sc_tup::at(system_session, process, scp::processStack_);
	if (cur_ctx_list) {
		while (cur_ctx_list) {
			sc_addr cur_scope = sc_list::get_value(system_session, cur_ctx_list);
			printf(".");
			cur_ctx_list = sc_list::get_next(system_session, cur_ctx_list);
		}
	}

	if (!op_type) {
		printf("%s::%s\n", progid.c_str(), operid.c_str());
	} else {
		printf("%s::%s(%s, line %s)\n",
			progid.c_str(), operid.c_str(), system_session->get_idtf(op_type).c_str(), get_op_line_str(op).c_str());
	}

	fflush(stdout);

	return RV_OK;
}

void print_ctx_info(sc_addr scope)
{
	sc_addr active_op = sc_tup::at(system_session, scope, ACTIVE_);
	sc_addr op_type = get_op_type(active_op);

	fprintf(stderr, "\tFile %s, line %s\n",
		get_op_file_str(active_op).c_str(), get_op_line_str(active_op).c_str());

	fflush(stdout);
}

void print_error(sc_addr process)
{
	sc_addr processError;
	if (search_5_f_cpa_cna_cpa_f(system_session, process, 0, &processError, 0, ERROR_) == RV_OK) {
		const Content *errorContent = system_session->get_content_const(processError);

		ASSERT_FATAL_ERROR(errorContent->type() == TCSTRING, "Process error node have no string content");

		fprintf(stderr, "\tError: %s\n", static_cast<const Cont &>(*errorContent).d.ptr);
	}
}

void print_stack_trace(sc_addr process)
{
	fprintf(stderr, "\n\nProcessor Module Stack Trace:\n");

	sc_addr ctxs_list = sc_tup::at(system_session, process, scp::processStack_);
	if (ctxs_list) {
		sc_addr cur_ctx_list = ctxs_list;
		while (cur_ctx_list) {
			sc_addr cur_scope = sc_list::get_value(system_session, cur_ctx_list);
			print_ctx_info(cur_scope);
			cur_ctx_list = sc_list::get_next(system_session, cur_ctx_list);
		}
	}

	print_ctx_info(process);
	print_error(process);
}

bool scp_check_state(sc_session *s, sc_addr process, sc_addr state_node)
{
	return !search_3_f_cpa_f(s, state_node, 0, process);
}

/**
 * Для scp-процесса устанавливает переданный scp-оператор активным.
 * Не удаляет дугу, связывающий текущий активный scp-оператор.
 */
inline sc_retval set_next_op(sc_addr process, sc_addr nextOp)
{
	gen5_f_cpa_f_cpa_f(system_session, process, 0, nextOp, 0, ACTIVE_);
	return RV_OK;
}

class scp_process_activity: public basic_advanced_activity, public advanced_activity_creator<scp_process_activity, &scp_segment>
{
private:
	bool step_tracing;
	bool traced;
	sc_session *session;

public:
	friend sc_retval scp_create_process(sc_session *processSession, sc_addr program, sc_addr father, sc_addr actualParams, sc_addr *_process, sc_addr descr);

	static sc_session* retrieve_session(sc_addr sign)
	{
		scp_process_activity *a = scp_process_activity::get(sign);
		if (!a) {
			return 0;
		}

		return a->session;
	}

	scp_process_activity(sc_session *s) : session(s), traced(false), step_tracing(false) {}

	void handle_error()
	{
		if (stop_on_error) {
			printf("Stopped PM due to ERROR_STATE\n");
			exit(1981);
		}

		dprintf("Stopping process due to ERROR_STATE\n");

		scp_set_state(sign, SCP_STATE_DEAD);
		print_stack_trace(sign);
	}

	/// Activate all tracers with @p event and supplied parameters.
	/// @note Yet support only one process tracer.
	sc_retval broadcast_event(sc_addr event, sc_addr prm1=0, sc_addr prm2=0)
	{
		sc_addr ptracer = scp_process_info::get_tracer(sign);
		if (ptracer)
			return system_session->activate(ptracer, event, prm1, prm2);
		return RV_OK;
	}

	sc_retval raise_error_event(sc_addr op)
	{
		return is_traced() ? broadcast_event(PROCESS_ERROR_EVENT, sign, op) : RV_OK;
	}

	/// Raise event: Process operator @p op calls other program.
	sc_retval raise_call_event(sc_addr op)
	{
		return is_traced() ? broadcast_event(PROCESS_CALL_EVENT, sign, op) : RV_OK;
	}

	sc_retval raise_return_event(sc_addr op)
	{
		return is_traced() ? broadcast_event(PROCESS_RETURN_EVENT, sign, op) : RV_OK;
	}

	sc_retval raise_step_event(sc_addr op)
	{
		return (is_traced() && is_step_tracing()) ? broadcast_event(PROCESS_STEP_EVENT, sign, op) : RV_OK;
	}

	sc_retval raise_step_after_event(sc_addr op)
	{
		return (is_traced() && is_step_tracing()) ? broadcast_event(PROCESS_STEP_AFTER_EVENT, sign, op) : RV_OK;
	}

	sc_retval raise_child_event(sc_addr child_process)
	{
		return is_traced() ? broadcast_event(PROCESS_CHILD_EVENT, sign, child_process) : RV_OK;
	}

	sc_retval raise_death_event()
	{
		return is_traced() ? broadcast_event(PROCESS_DEATH_EVENT, sign) : RV_OK;
	}

	sc_session *get_session() const { return session; }

	bool is_traced() const { return traced; }

	void set_traced(bool traced) { this->traced = traced; }

	bool is_step_tracing() const { return step_tracing; }

	void set_step_tracing(bool mode)
	{
		if (diag_output) {
			if (mode != this->step_tracing)
				std::cout << "[SCP] Turn " << (mode ? "ON" : "OFF") <<
					" step tracing for process " << SC_URI(sign) << std::endl;
		}
		this->step_tracing = mode;
	}

	sc_retval activate(sc_session *s, sc_addr process, sc_addr param1, sc_addr, sc_addr)
	{
		if (scp_check_state(system_session, process, PROCESS_STATE_DEAD)) {
			raise_death_event();

			scp_process_info info;
			if (scp_fill_process_info(process, &info)) {
				SC_ABORT();
			}

			return finalize_process(&info);
		}

		// Search active operator for current SCP-process context
		sc_addr active_arc = 0;
		sc_addr active_op = scp_process_info::get_active_op(process, &active_arc);
		if (!active_op) {
			scp_set_state(process, SCP_STATE_ERROR);
		} else {
			raise_step_event(active_op);

			if (verb_output)
				print_proc_info(process, active_op);

			// Для активного оператора определим тип, к которому он принадлежит
			sc_addr op_type = get_op_type(active_op);
			if (!op_type) {
				SCP_ERROR_F(process, "Type of operator '%s' is incorrect", FULL_IDTF(active_op).c_str());
				scp_set_state(process, SCP_STATE_ERROR);
			} else {
				// Выполняем оператор, используя обработчик узла типа
				if (system_session->activate(op_type, process))
					scp_set_state(process, SCP_STATE_ERROR);
			}
		}

		// After executing operator context may be changed because search for new active operator
		active_op = scp_process_info::get_active_op(process, &active_arc);

		if (!scp_check_state(system_session, process, PROCESS_STATE_REPEAT))
			raise_step_after_event(active_op);

		sc_addr next_op;

		if (scp_check_state(system_session, process, PROCESS_STATE_ERROR)) {
			raise_error_event(active_op);

			if (next_op = get_next_op(active_op, ERROR_)) {
				scp_set_state(process, SCP_STATE_THEN);
				system_session->erase_el(active_arc);
				set_next_op(process, next_op);
			} else {
				handle_error();
			}
		}

		if (scp_check_state(system_session, process, PROCESS_STATE_DEAD))
			return RV_OK;

		if (!scp_check_state(system_session, process, PROCESS_STATE_REPEAT)) {
			sc_addr next_attr = THEN_;

			if (scp_check_state(system_session, process, PROCESS_STATE_ELSE))
				next_attr = ELSE_;

			if (!(next_op = get_next_op(active_op, next_attr)) && !(next_op = get_next_op(active_op, GOTO_))) {
				SCP_ERROR(process, "Failed to move to next operator");

				scp_set_state(process, SCP_STATE_DEAD);
				print_stack_trace(process);

				return RV_OK;
			}

			system_session->erase_el(active_arc);

			set_next_op(process, next_op);
		}

		if (scp_check_state(system_session, process, PROCESS_STATE_SLEEP))
			pm_sched_put_to_sleep(process);

		return RV_OK;
	}
};

bool scp_process_info::is_traced(sc_addr process)
{
	scp_process_activity *a = scp_process_activity::get(process);
	return a->is_traced();
}

void scp_process_info::set_traced(sc_addr process, bool traced)
{
	scp_process_activity *a = scp_process_activity::get(process);
	a->set_traced(traced);
}

bool scp_process_info::is_step_tracing(sc_addr process)
{
	scp_process_activity *a = scp_process_activity::get(process);
	return a->is_step_tracing();
}

void scp_process_info::set_step_tracing(sc_addr process, bool stepTracing)
{
	scp_process_activity *a = scp_process_activity::get(process);
	a->set_step_tracing(stepTracing);
}

sc_addr get_init_op(sc_addr program)
{
	return sc_tup::at(system_session, program, INIT_);
}

void scp_process_info::set_default_seg(sc_addr process, sc_addr defsegsign)
{
	sc_addr arc;

	if (search_5_f_cpa_cna_cpa_f(system_session, process, &arc, 0, 0, DEFAULT_SEG_)) {
		FATAL_ERROR("Cannot find default seg");
	}

	system_session->erase_el(arc);
	gen5_f_cpa_f_cpa_f(system_session, process, 0, defsegsign, 0, DEFAULT_SEG_);
}

sc_retval scp_process_info::set_error(sc_addr process, const sc_string& error_str)
{
	sc_addr process_error = sc_tup::at(system_session, process, ERROR_);

	if (process_error == 0) {
		process_error = system_session->create_el(scp_segment, SC_N_CONST);
		gen5_f_cpa_f_cpa_f(system_session, process, 0, process_error, 0, ERROR_);
	}

	system_session->set_content(process_error, error_str);

	return RV_OK;
}

sc_retval scp_fill_process_info(sc_addr process, scp_process_info *pi)
{
	pi->process = process;
	pi->session = scp_process_activity::retrieve_session(process);

	if (!pi->session)
		return RV_ERR_GEN;

	static sc_addr scp_process_info_attrs[] =  { PROGRAM_, VAR_VALUE_, ACTIVE_, AUTOSEGMENT_, DEFAULT_SEG_ };
	sc_addr *rv[] = { &pi->program, &pi->var_value, &pi->active_op, &pi->auto_dirent, &pi->defaultseg_sign };
	static const int scp_process_info_attrs_count = sizeof(scp_process_info_attrs) / sizeof(sc_addr);
	sc_tup::unpack(system_session, process, scp_process_info_attrs, rv, scp_process_info_attrs_count);

	if (!pi->program || !pi->var_value || !pi->active_op || !pi->auto_dirent && !pi->defaultseg_sign) {
		return RV_ERR_GEN;
	}

	if (search_5_f_cpa_cna_cpa_f(pi->program, 0, &pi->vars, 0, VAR_)) {
		LOG_ERROR("Cannot find variable set of %s\n." << SC_URIc(pi->program));
		return RV_ERR_GEN;
	}

	if (search_5_f_cpa_cna_cpa_f(pi->program, 0, &pi->consts, 0, CONST_)) {
		dprintf("Cannot find constant set of %s\n", SC_URIc(pi->program));
		return RV_ERR_GEN;
	}

	return RV_OK;
}

class child_death_hook : public basic_advanced_activity
{
public:
	sc_retval activate(sc_session *s, sc_addr _this, sc_addr father, sc_addr son, sc_addr)
	{
		if (!scp_check_state(system_session, father, PROCESS_STATE_SLEEP))
			return RV_OK;

		sc_addr waiting_on = sc_tup::at(system_session, father, WAITING_FOR_);
		if (!waiting_on)
			return RV_OK;

		if (waiting_on == ALL_CHILDS && count_childs(father) < 2)
			goto _wakeup;

		if (waiting_on != son)
			return RV_OK;

	_wakeup:
		scp_set_state(father, SCP_STATE_RUN);
		if (pm_sched_wakeup(father))
			SC_ABORT();

		return RV_OK;
	}

private:
	static int count_childs(sc_addr process)
	{
		int cnt = 0;
		sc_iterator *iter = system_session->create_iterator(
			sc_constraint_new(CONSTR_5_a_a_f_a_f, SC_N_CONST, SC_A_CONST, process, SC_A_CONST, FATHER_), true);
		sc_for_each (iter) cnt++;
		return cnt;
	}
};

/// Unpack components of parameters @p tuple to #addr_vector @p result.
/// @note If tuple is #SCADDR_NIL then
/// @note If error then set error string for scp-process @p process
///       and return #RV_ERR_GEN from current scope.
#define SCP_UNPACK_PARAMS(s, process, tuple, result) \
	addr_vector result; \
	if (tuple) { \
		try { \
			sc_tup::unpack_ord(s, tuple, result); \
		} catch (std::logic_error) { \
			SCP_ERROR(process, "Components of parameters tuple must be unique."); \
			return RV_ERR_GEN; \
		} \
	}

/// This base class just for concept checking and documentation.
/// Don't use it with polymorphism.
class params_processor
{
public:
	inline void init(addr_vector &called_prms)
	{
	}

	inline void process_output(addr_vector &called_prms, size_t idx, sc_addr passed_prm)
	{
	}

	inline void process_input(addr_vector &called_prms, size_t idx, sc_addr value)
	{
	}
};

/// Iterate over declared parameters of program @p called_prg
/// and passed parameters for call or process creating.
/// Apply @p processor for input and output parameters.
///
/// @see scp_params_processor
/// @see ext_params_processor
template<typename ParamsProcessorType>
sc_retval process_program_params(sc_session *s, sc_addr process, sc_addr caller_prg,
								 sc_addr caller_var_value, sc_addr called_prg,
								 addr_vector &passed_prms, ParamsProcessorType processor)
{
	//
	// Unpack parameters of called program declaration
	//
	addr_vector called_prms;
	sc_addr tuple = sc_tup::at(s, called_prg, PRM_);
	if (tuple) {
		try {
			sc_tup::unpack_ord(s, tuple, called_prms);
		} catch (std::logic_error) {
			SCP_ERROR(process, "Components of parameters tuple must be unique.");
			return RV_ERR_GEN;
		}
	}

	if (called_prms.empty() && !passed_prms.empty()) {
		SCP_ERROR_F(process,
			"External program \"%s\" doesn't have parameters.", SC_URIc(called_prg));
		return RV_ERR_GEN;
	}

	//
	// Check maximum parameters count for called program
	//
	if (passed_prms.size() != called_prms.size()) {
		SCP_ERROR_F(process,
			"External program \"%s\" expected %d parameters, but called with %d parameters.",
			SC_URIc(called_prg), called_prms.size(), passed_prms.size());
		return RV_ERR_GEN;
	}

	sc_addr caller_vars = sc_tup::at(s, caller_prg, VAR_);

	//
	// Initialization phase for program parameters processor.
	//
	processor.init(called_prms);

	for (size_t prm_index = 0; prm_index < called_prms.size(); ++prm_index) {
		sc_addr called_prm = s->get_end(called_prms[prm_index]);

		bool input = sc_set::is_in(s, called_prms[prm_index], IN_) != SCADDR_NIL;
		bool output = sc_set::is_in(s, called_prms[prm_index], OUT_) != SCADDR_NIL;

		if (!passed_prms[prm_index]) {
			LOG_WARN("Program parameter " << prm_index << "_ doesn't have matching actual parameter");
			continue;
		}
		sc_addr passed_prm = s->get_end(passed_prms[prm_index]);

		bool is_passed_prm_var = sc_set::is_in(s, passed_prm, caller_vars) != SCADDR_NIL;

		if (output) {
			if (!is_passed_prm_var) {
				SCP_ERROR_F(process,
					"Parameter %d_ \"%s\" of program \"%s\" is output, but passed parameter \"%s\" isn't variable.",
					prm_index, SC_URIc(called_prm), SC_URIc(called_prg), SC_URIc(passed_prm));
				return RV_ERR_GEN;
			}

			//
			// Process output parameter.
			//
			processor.process_output(called_prms, prm_index, passed_prm);
		}

		if (input) {
			sc_addr value = passed_prm;

			if (is_passed_prm_var) {
				if (__get_var_value(caller_var_value, passed_prm, 0, &value)) {
					SCP_ERROR_F(process, "Passed as input parameter variable \"%s\" doesn't have value.", SC_URIc(passed_prm));
					return RV_ERR_GEN;
				}
			}

			if (s->is_segment_opened(value->seg) == false) {
				SCP_ERROR_F(process, "Segment \"%s\" of parameter %d_ value from program \"%s\" isn't opened",
					SC_URIc(value->seg), prm_index, SC_URIc(called_prg));
				return RV_ERR_GEN;
			}

			//
			// Process input parameter.
			//
			processor.process_input(called_prms, prm_index, value);
		}
	}

	return RV_OK;
}

/// Create auto-segment for SCP-process.
sc_addr create_autoseg(sc_session *s, sc_session *process_session, sc_addr process)
{
	sc_segment *autoseg = create_unique_segment(s, "/tmp/autoseg");
	sc_addr auto_dirent = seg2sign(s, autoseg);

	gen5_f_cpa_f_cpa_f(s, process, 0, auto_dirent, 0, AUTOSEGMENT_);
	process_session->open_segment(autoseg->get_full_uri());

	return auto_dirent;
}

/// Prepare SCP-program by creating set of constant's segments.
sc_addr prepare_program_const_segs(sc_session *s, sc_addr process, sc_addr prg) {
	sc_addr const_segs;

	if (!search_5_f_cpa_cna_cpa_f(s, prg, 0, &const_segs, 0, CONST_SEGMENTS_))
		return const_segs;

	sc_addr consts;
	if (search_5_f_cpa_cna_cpa_f(s, prg, 0, &consts, 0, CONST_)) {
		SCP_ERROR(process, "Program \"" << FULL_IDTF(prg) << "\" hasn't constants\n");
		return 0;
	}

	const_segs = s->create_el(scp_segment, SC_N_CONST);
	gen5_f_cpa_f_cpa_f(s, prg, 0, const_segs, 0, CONST_SEGMENTS_);

	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, consts, SC_A_CONST|SC_POS, 0), true);
	sc_for_each (iter) {
		sc_addr constant = iter->value(2);

		if (!constant) {
			continue;
		}

		sc_addr seg = seg2sign(s, constant->seg);
		ASSERT_FATAL_ERROR(seg, "Cannot retrieve sign for segment");

		if (search_3_f_cpa_f(s, const_segs, 0, seg) != RV_OK) {
			gen3_f_cpa_f(s, const_segs, 0, seg);
		}
	}

	return const_segs;
}

/// Parameters processor for scp-programs.
/// @see process_program_params
class scp_params_processor : public params_processor
{
public:
	explicit scp_params_processor(sc_session *_s, sc_addr _called_var_value, sc_addr _out_binding)
		: s(_s), called_var_value(_called_var_value), out_binding(_out_binding)
	{
	}

	inline void init(addr_vector &called_prms)
	{
	}

	inline void process_output(addr_vector &called_prms, size_t idx, sc_addr passed_prm)
	{
		sc_rel::add_ord_tuple(s, out_binding, passed_prm,s->get_end(called_prms[idx]));
	}

	inline void process_input(addr_vector &called_prms, size_t idx, sc_addr value)
	{
		__set_variable(called_var_value, s->get_end(called_prms[idx]), value);
	}

private:
	sc_session *s;
	sc_addr called_var_value;
	sc_addr out_binding;
};

sc_retval scp_create_process(sc_session *process_session, sc_addr prg, sc_addr father, sc_addr prms,
							 sc_addr *_process, sc_addr descr)
{
	SCP_UNPACK_PARAMS(system_session, father, prms, passed_prms);
	return scp_create_process(process_session, prg, father, passed_prms, _process, descr);
}

sc_retval scp_create_process(sc_session *process_session, sc_addr program, sc_addr father, addr_vector &actual_prms,
							 sc_addr *_process, sc_addr descr)
{
	START_TRANSACTION(s, system_session);

	if (!sc_set::is_in(s, program, SCP_PROGRAM)) {
		LOG_ERROR(FULL_IDTF(program) << " isn't scp-program");
		return RV_ERR_GEN;
	}

	try {
		sc_activity *activity = new scp_process_activity(process_session);

		// Opens for process segment of interpreted program
		process_session->open_segment(program->seg->get_full_uri());

		sc_addr const_dirents = prepare_program_const_segs(s, father, program);
		if (!const_dirents) {
			LOG_ERROR("Could not create const segments set for scp-process");
			return RV_ERR_GEN;
		}

		sc_tup process_tup(s, scp_segment);
		sc_addr process = process_tup.sign();
		if (_process)
			*_process = process;

		if (descr)
			gen3_f_cpa_f(s, descr, 0, process);

		//
		// Create OPENED_SEGMENTS_ set
		//
		sc_addr opened_segs = create_el(s, scp_segment, SC_N_CONST);
		process_tup.add(OPENED_SEGMENTS_, opened_segs);
		open_segset(process_session, const_dirents, s);

		sc_addr auto_dirent = create_autoseg(s, process_session, process);
		if (!auto_dirent)
			return RV_ERR_GEN;

		sc_addr defsegsign = auto_dirent;
		if (father) {
			if (search_5_f_cpa_cna_cpa_f(s, father, 0, &defsegsign, 0, DEFAULT_SEG_)) {
				SC_ABORT();
			}

			sc_set::include_in(s, defsegsign, opened_segs);
		}

		process_tup.add(DEFAULT_SEG_, defsegsign);

		s->reimplement(process, activity);
		sc_set::include_in(s, process, SCP_PROCESS);

		sc_addr init_op = get_init_op(program);
		if (!init_op) {
			LOG_ERROR("Program '" << SC_URI(program) << "' hasn't init operator");
			return RV_ERR_GEN;
		}

		sc_addr var_value = create_el(s, scp_segment, SC_N_CONST);
		process_tup.add(VAR_VALUE_, var_value);

		init_cache(var_value); // LEAK

		process_tup.add(PROGRAM_, program);
		process_tup.add(ACTIVE_, init_op);

		sc_addr hook = create_el(s, scp_segment, SC_N_CONST);
		s->reimplement(hook, new child_death_hook());
		process_tup.add(CHILD_DEATH_HOOK_, hook);

		///
		/// Construct parameters for new SCP-process
		///

		sc_addr prg_prms = sc_tup::at(s, program, PRM_);
		bool prg_has_prms = prg_prms != 0 && !sc_set::is_empty(system_session, prg_prms);

		if (prg_has_prms) {
			if (!father) {
				LOG_ERROR("Cannot born process for SCP-program \"" << get_full_uri(system_session, program)
							<< "\" with params and without father");
				return RV_ERR_GEN;
			}

			if (actual_prms.empty()) {
				SCP_ERROR(father, "Cannot born process for SCP-program \"" << get_full_uri(system_session, program) <<
					"\" with params not giving it actual params");
				return RV_ERR_GEN;
			}

			sc_addr out_binding = s->create_el(scp_segment, SC_N_CONST);
			process_tup.add(OUTPUT_PRM_BINDING_, out_binding);

			scp_params_processor prms_processor(s, var_value, out_binding);
			sc_addr father_prg       = sc_tup::at(s, father, PROGRAM_);
			sc_addr father_var_value = sc_tup::at(s, father, VAR_VALUE_);
			if (process_program_params(s, process, father_prg, father_var_value, program,
					actual_prms, prms_processor) != RV_OK) {
				LOG_ERROR("Failed to construct input params for SCP-program \"" << get_full_uri(system_session, program) << "\"");
				return RV_ERR_GEN;
			}
		} else {
			if (!actual_prms.empty()) {
				LOG_ERROR("Cannot born process for SCP-program \"" << get_full_uri(system_session, program) <<
					"\" without params, but giving it actual params");
				return RV_ERR_GEN;
			}
		}

		if (father)
			process_tup.add(FATHER_, father);

		// Если для родительского процесса установлен трассировщик,
		// то установим этот же трассировщик и для дочернего процесса.
		if (father) {
			sc_addr tracer = scp_process_info::get_tracer(father);

			if (tracer) {
				scp_process_info::set_tracer(process, tracer);
				scp_process_activity *father_activity = scp_process_activity::get(father);
				father_activity->raise_child_event(process);
			}
		}

		s->commit();

		return RV_OK;
	} catch (memory_error) {
		return RV_ERR_GEN;
	}
}

//
// Functional for calling scp-programs from scp-programs.
//
// ======================================================
//

/// @brief Class represents execution contexts stack of scp-process.
/// @details
/// Yet sign of top stack element is scp-process sign.
/// Other context stack represented as #sc_list.
/// Top of list is component with attribute scp::processStack_.
///
/// @todo Uniform context stack representation.
/// @todo Rethink unpacking of context structure.
class scp_context_stack
{
public:
	/// Create object for scp-process @p _process.
	/// Object works with memory through @p s.
	scp_context_stack(sc_session *_s, sc_addr _process) : s(_s), process(_process)
	{
		fetch_context();
	}

	/// Return true if process hasn't previous contexts.
	bool is_empty() const { return prev_ctx_list == SCADDR_NIL; }

	/// Push current context into stack.
	/// Create new context for scp-program @p new_prg and set it as current.
	sc_retval push_and_switch(sc_addr new_prg)
	{
		// Save current process context
		sc_addr ctx = create_context();
		sc_addr ctx_list = sc_list::create(s, scp_segment, ctx, 0);

		if (is_empty()) {
			gen5_f_cpa_f_cpa_f(s, scp_segment, process, 0, ctx_list, 0, scp::processStack_);
		} else {
			sc_list::set_next(s, scp_segment, ctx_list, prev_ctx_list);
			s->set_end(prev_ctx_arc, ctx_list);
		}

		// Set new process context
		cur_prg = new_prg;
		s->set_end(cur_prg_arc, new_prg);

		sc_addr init_op = get_init_op(new_prg);
		if (!init_op) {
			SCP_ERROR(process, "Program '" << FULL_IDTF(new_prg) << "' hasn't init operator");
			return RV_ERR_GEN;
		}
		cur_active_op = init_op;
		s->set_end(cur_active_op_arc, init_op);

		sc_addr var_value = s->create_el(scp_segment, SC_N_CONST);
		cur_var_value = var_value;
		init_cache(var_value);
		s->set_end(cur_var_value_arc, var_value);

		cur_out_binding = s->create_el(scp_segment, SC_N_CONST);
		if (cur_out_binding_arc) {
			s->set_end(cur_out_binding_arc, cur_out_binding);
		} else {
			cur_out_binding_arc = sc_tup::add_c(s, process, OUTPUT_PRM_BINDING_, cur_out_binding);
		}

		return RV_OK;
	}

	/// Erase current context, pop from stack previous context and set it as current.
	sc_retval pop_and_switch()
	{
		SC_ASSERTS(!is_empty(), "Pop from empty scp-process context stack");

		//
		// 1. Unpack previous context.
		// 2. Return values to previous context.
		// 3. Restore program from previous context.
		// 4. Restore active operator.
		// 5. Restore previous var value and erase current var value.
		// 6. Restore output binding and erase current output binding.
		// 7. Pop previous context from context stack.
		//

		// Pass 1
		sc_addr prev_prg, prev_var_value, prev_active_op, prev_out_binding;
		sc_addr prev_ctx = sc_list::get_value(s, prev_ctx_list);
		unpack_context(prev_ctx, &prev_prg, &prev_var_value, &prev_active_op, &prev_out_binding);
		SC_ASSERT(prev_prg && prev_var_value && prev_active_op && prev_out_binding &&
			"Bad unpack previous SCP-context");

		// Pass 2
		return_values(prev_var_value);

		// Pass 3: restore program
		s->set_end(cur_prg_arc, prev_prg);
		cur_prg = prev_prg;

		// Pass 4: restore active operator
		s->set_end(cur_active_op_arc, prev_active_op);
		cur_active_op = prev_active_op;

		// Pass 5: restore var value
		s->set_end(cur_var_value_arc, prev_var_value);
		dispose_cache(cur_var_value);
		sc_rel::erase(s, cur_var_value);
		cur_var_value = prev_var_value;

		// Pass 6: restore output binding
		s->set_end(cur_out_binding_arc, prev_out_binding);
		sc_set::erase_from(s, cur_out_binding);
		s->erase_el(cur_out_binding);
		cur_out_binding = prev_out_binding;

		// Pass 7: pop previous context from stack
		sc_addr cur_ctx_list = prev_ctx_list;
		prev_ctx_list = sc_list::get_next(s, cur_ctx_list);
		if (prev_ctx_list)
			s->set_end(prev_ctx_arc, prev_ctx_list);
		s->erase_el(cur_ctx_list);

		return RV_OK;
	}

	/// Return var value in current context.
	sc_addr get_var_value() const { return cur_var_value; }

	/// Return scp-program in current context.
	sc_addr get_prg() const { return cur_prg; }

	/// Return active scp-operator in current context.
	sc_addr get_active_op() const { return cur_active_op; }

	/// Return output binding in current context.
	sc_addr get_out_binding() const { return cur_out_binding; }

private:
	inline void unpack_context(sc_addr ctx, sc_addr *prg, sc_addr *var_value, sc_addr *active_op, sc_addr *out_binding)
	{
		static sc_addr CTX_ATTRS[] = { PROGRAM_, VAR_VALUE_, ACTIVE_, OUTPUT_PRM_BINDING_ };
		static const int CTX_ATTRS_COUNT = sizeof(CTX_ATTRS) / sizeof(sc_addr);

		sc_addr *rv[] = { prg, var_value, active_op, out_binding };

		sc_tup::unpack(s, ctx, CTX_ATTRS, rv, CTX_ATTRS_COUNT);
	}

	void fetch_context()
	{
		prev_ctx_list = sc_tup::at(s, process, scp::processStack_, &prev_ctx_arc);
		unpack_context(process, &cur_prg, &cur_var_value, &cur_active_op, &cur_out_binding);
		cur_prg = sc_tup::at(s, process, PROGRAM_, &cur_prg_arc);
		cur_var_value = sc_tup::at(s, process, VAR_VALUE_, &cur_var_value_arc);
		cur_active_op = sc_tup::at(s, process, ACTIVE_, &cur_active_op_arc);
		cur_out_binding = sc_tup::at(s, process, OUTPUT_PRM_BINDING_, &cur_out_binding_arc);
	}

	sc_addr create_context()
	{
		sc_addr ctx = s->create_el(scp_segment, SC_N_CONST);

		sc_tup::add_c(s, ctx, PROGRAM_, cur_prg);
		sc_tup::add_c(s, ctx, VAR_VALUE_, cur_var_value);
		sc_tup::add_c(s, ctx, ACTIVE_, cur_active_op);

		if (!cur_out_binding)
			cur_out_binding = s->create_el(scp_segment, SC_N_CONST);

		sc_tup::add_c(s, ctx, OUTPUT_PRM_BINDING_, cur_out_binding);

		return ctx;
	}

	/// Return values of output variables (use cur_out_binding) from current context to var value @p prev_var_value.
	void return_values(sc_addr prev_var_value)
	{
		sc_addr caller_var, var, value;

		sc_iterator *it = s->create_iterator(
			sc_constraint_new(CONSTR_3_f_a_a, cur_out_binding, SC_A_CONST|SC_POS, SC_N_CONST), true);
		sc_for_each(it) {
			sc_addr conn = it->value(2);
			if (!conn)
				continue;

			if (sc_tup::unpack_ord(s, conn, &caller_var, &var))
				BUG_ERROR(RV_ERR_GEN);

			if (__get_var_value(cur_var_value, var, 0, &value)) {
				__clear_variable(prev_var_value, caller_var);
				continue;
			}

			__set_variable(prev_var_value, caller_var, value);

			if (__get_var_value(prev_var_value, caller_var, 0, 0))
				BUG_ERROR(RV_ERR_GEN);
		}
	}

private:
	sc_session *s;
	sc_addr process; ///< SCP-process and current context sign.

	/// @name Current context entries
	/// @{
	sc_addr cur_prg, cur_prg_arc;
	sc_addr cur_var_value, cur_var_value_arc;
	sc_addr cur_active_op, cur_active_op_arc;
	sc_addr cur_out_binding, cur_out_binding_arc;
	/// @}

	/// @name Info for accessing previous context
	/// @{
	sc_addr prev_ctx_list, prev_ctx_arc;
	/// @}
};

sc_retval scp_call_program(scp_process_info *info, sc_addr called_prg, sc_addr prms)
{
	SCP_UNPACK_PARAMS(system_session, info->process, prms, passed_prms);
	return scp_call_program(info, called_prg, passed_prms);
}

sc_retval scp_call_program(scp_process_info *info, sc_addr prg, addr_vector &prms)
{
	START_TRANSACTION(s, system_session);

	sc_addr process = info->process;

	if (!sc_set::is_in(s, prg, SCP_PROGRAM)) {
		SCP_ERROR(process, SC_URI(prg) << " isn't scp-program");
		return RV_ERR_GEN;
	}

	try {
		//
		// Open segment of interpreted program in process session
		//
		info->session->open_segment(prg->seg->get_full_uri());

		//
		// Prepare program, build set of constant segments.
		//
		sc_addr const_segs = prepare_program_const_segs(s, process, prg);
		if (!const_segs)
			return RV_ERR_GEN;

		//
		// Open constant segments in process session.
		//
		sc_addr opened_segs = scp_process_info::get_opened_segs(process);
		open_segset(info->session, const_segs, s);

		//
		// Yet we will push current process context and build new context.
		//

		// Retrieve contexts stack.
		scp_context_stack ctx_stack(s, process);

		//
		// Save in prev_var_value, prev_prg, active_op current process context.
		// That saving for usage in future in #scp_call_program.
		//
		sc_addr prev_var_value = info->var_value;
		sc_addr prev_prg       = info->program;
		sc_addr active_op      = info->active_op;

		// Push current context and create initial structure of
		// new context for interpretation of called program.
		ctx_stack.push_and_switch(prg);

		// Construct parameters for called program and its interpretation context.
		scp_params_processor processor(s, ctx_stack.get_var_value(), ctx_stack.get_out_binding());
		if (process_program_params(s, process, prev_prg, prev_var_value, prg,
				prms, processor) != RV_OK)
			return RV_ERR_GEN;

		// If no errors occured then commit transaction.
		s->commit();

		//
		// If process has tracer then raise call event.
		//
		sc_addr tracer = scp_process_info::get_tracer(process);
		if (tracer) {
			scp_process_activity *activity = scp_process_activity::get(process);
			activity->raise_call_event(active_op);
		}

		return RV_OK;
	} catch (memory_error) {
		return RV_ERR_GEN;
	}

}

//
// ======================================================
//

//
// Functional for calling external PM programs
// from scp-programs.
//
// ===========================================
//

/// Parameters processor for external programs.
/// @see process_program_params
class ext_params_processor : public params_processor
{
public:
	explicit ext_params_processor(addr_vector &_act_prms, addr_vector &_out_binding)
		: act_prms(_act_prms), out_binding(_out_binding)
	{
	}

	void init(addr_vector &called_prms)
	{
		act_prms.resize(called_prms.size(), SCADDR_NIL);
		out_binding.resize(called_prms.size(), SCADDR_NIL);
	}

	void process_output(addr_vector &called_prms, size_t idx, sc_addr passed_prm)
	{
		out_binding[idx] = passed_prm;
	}

	void process_input(addr_vector &called_prms, size_t idx, sc_addr value)
	{
		act_prms[idx] = value;
	}

private:
	addr_vector &act_prms;
	addr_vector &out_binding;
};

sc_retval scp_call_ext_program(scp_process_info *info, sc_addr called_prg, sc_addr prms)
{
	SCP_UNPACK_PARAMS(system_session, info->process, prms, passed_prms);
	return scp_call_ext_program(info, called_prg, passed_prms);
}

sc_retval scp_call_ext_program(scp_process_info *info, sc_addr called_prg, addr_vector &prms)
{
	addr_vector act_prms;
	addr_vector out_binding;

	if (process_program_params(system_session, info->process, info->program, info->var_value,
			called_prg, prms, ext_params_processor(act_prms, out_binding)) != RV_OK)
		return RV_ERR_GEN;

	//
	// Retrieve function pointer for called external program and call it
	//
	sc_retval rv = call_ext_program(info->session, called_prg, act_prms);
	if (rv == RV_ERR_GEN)
		return RV_ERR_GEN;

	//
	// Return output parameters from external program
	//
	for (size_t i = 0; i < out_binding.size(); ++i) {
		sc_addr out_var = out_binding[i];
		if (out_var) {
			sc_addr out_value = act_prms[i];
			if (!out_value || out_value->is_dead()) {
				__clear_variable(info->var_value, out_var);
			} else {
				if (!info->session->is_segment_opened(out_value->seg))
					info->session->open_segment(out_value->seg->get_full_uri());

				__set_variable(info->var_value, out_var, out_value);
			}
		}
	}

	return rv;
}

//
// ===========================================
//

/// Erase process structure, close opened segments, remove process from scheduler.
sc_retval finalize_process(scp_process_info *info)
{
	sc_session *ps = info->session;
	if (!ps)
		return RV_ERR_GEN;

	dispose_cache(info->var_value);
	sc_rel::erase(system_session, info->var_value);

	sc_addr father = scp_process_info::get_father(info->process);
	if (father) {
		sc_addr hook = sc_tup::at(system_session, father, CHILD_DEATH_HOOK_);
		if (hook)
			system_session->activate(hook, father, info->process);
	}

	ps->unlink(ps->sign2uri(info->auto_dirent));
	ps->close();

	pm_sched_remove_agent(info->process);
	system_session->erase_el(info->process);

	return system_session->get_error();
}

void scp_set_state(sc_addr process, scp_state state)
{
	int t;
	sc_addr kn;
	if ((t = (state & SCP_STATE_CONTROL_MASK))) {
		sc_set::exclude_from(system_session, process, PROCESS_STATE_THEN, PROCESS_STATE_ELSE, PROCESS_STATE_REPEAT, PROCESS_STATE_ERROR);
		if (t == SCP_STATE_THEN) {
			kn = PROCESS_STATE_THEN;
		} else if (t == SCP_STATE_ELSE) {
			kn = PROCESS_STATE_ELSE;
		} else if (t == SCP_STATE_REPEAT) {
			kn = PROCESS_STATE_REPEAT;
		}

		sc_set::include_in(system_session, process, kn);
	}

	if ((t = (state & SCP_STATE_SCHED_MASK))) {
		sc_set::exclude_from(system_session, process, PROCESS_STATE_SLEEP, PROCESS_STATE_RUN);
		if (t == SCP_STATE_SLEEP) {
			kn = PROCESS_STATE_SLEEP;
		} else {
			kn = PROCESS_STATE_RUN;
		}

		sc_set::include_in(system_session, process, kn);
	}

	if (state & SCP_STATE_DEAD) {
		sc_set::exclude_from(system_session, process, PROCESS_STATE_DEAD);
		sc_set::include_in(system_session, process, PROCESS_STATE_DEAD);
	}

	if (state & SCP_STATE_ERROR) {
		sc_set::exclude_from(system_session, process, PROCESS_STATE_ERROR);
		sc_set::include_in(system_session, process, PROCESS_STATE_ERROR);
	}
}

sc_string get_addr_idtf(sc_addr addr)
{
	return system_session->get_idtf(addr);
}

sc_string get_addr_full_uri(sc_addr addr)
{
	return addr->seg->get_full_uri() + "/" + get_addr_idtf(addr);
}

sc_retval return_values(scp_process_info *soninfo, scp_process_info *fatherinfo, sc_addr binding)
{
	sc_addr father_var, son_var, value;

	sc_iterator *iter = system_session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, binding, SC_A_CONST|SC_POS, SC_N_CONST), true);
	for (; !iter->is_over(); iter->next()) {
		sc_addr conn = iter->value(2);
		if (!conn) {
			continue;
		}

		if (sc_tup::unpack_ord(system_session, conn, &father_var, &son_var)) {
			BUG_ERROR(RV_ERR_GEN);
		}

		if (get_var_value(soninfo, son_var, 0, &value)) {
			scp_clear_variable(fatherinfo, father_var);
			continue;
		}

		if (scp_set_variable(fatherinfo, father_var, value)) {
			BUG_ERROR(RV_ERR_GEN);
		}

		if (get_var_value(fatherinfo, father_var, 0, 0)) {
			BUG_ERROR(RV_ERR_GEN);
		}
	}
	delete iter;

	sc_set::erase_from(system_session, binding);
	system_session->erase_el(binding);

	return RV_OK;
}

static sc_retval pass_up_garbage(scp_process_info &father, scp_process_info &son)
{
	sc_addr garbage_set = scp_process_info::get_opened_segs(father.process);
	SC_ASSERT(garbage_set && "Cannot find garbage_set of father process");

	sc_addr sons_garbage = scp_process_info::get_opened_segs(son.process);
	SC_ASSERT(sons_garbage && "Cannot find garbage_set of son process");

	sc_iterator *iter = system_session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, sons_garbage, SC_A_CONST|SC_POS, SC_N_CONST), true);
	sc_for_each (iter) {
		sc_addr seg = iter->value(2);

		if (!seg) {
			continue;
		}

		gen3_f_cpa_f(system_session, garbage_set, 0, seg);

		if (!father.session->open_segment(system_session->sign2uri(seg))) {
			sc_string uri = system_session->sign2uri(seg);
			system_session->open_segment(uri);
			SC_ABORT();
		}
	}

	system_session->erase_el(sons_garbage);
	return RV_OK;
}

sc_retval scp_process_return(scp_process_info *info, scp_state return_state)
{
	scp_context_stack ctx_stack(system_session, info->process);
	if (ctx_stack.is_empty()) {
		scp_set_state(info->process, SCP_STATE_DEAD);

		sc_addr father = scp_process_info::get_father(info->process);
		if (!father) {
			// father died
			return RV_OK;
		}

		scp_process_info fatherInfo;
		if (scp_fill_process_info(father, &fatherInfo)) {
			BUG_ERROR(RV_ERR_GEN);
		}

		if (pass_up_garbage(fatherInfo,  *info)) {
			SC_ABORT();
		}

		sc_addr binding = 0;
		search_5_f_cpa_cna_cpa_f(system_session, info->process, 0, &binding, 0, OUTPUT_PRM_BINDING_);
		if (binding && return_values(info, &fatherInfo, binding))
			return RV_ERR_GEN;

		scp_clear_this_value_variables(fatherInfo.var_value, info->process);
		return RV_OK;
	} else {
		scp_process_activity::get(info->process)->raise_return_event(info->active_op);

		if (ctx_stack.pop_and_switch() != RV_OK)
			return RV_ELSE_GEN;

		scp_set_state(info->process, return_state);

		return RV_OK;
	}
}

sc_retval scp_sleep_on_child(sc_addr process, sc_addr _child)
{
	sc_addr child;
	if (_child == ALL_CHILDS) {
		child = _child;
	} else if (search_oneshot(system_session, sc_constraint_new(CONSTR_3_f_a_a, _child, 0, SC_N_CONST), true, 1, 2, &child)) {
		// if there is not child assume it's dead
		return RV_OK;
	}

	if (system_session->gen5_f_a_f_a_f(process, 0, scp_segment, SC_A_CONST|SC_POS, child, 0, scp_segment, SC_A_CONST|SC_POS, WAITING_FOR_)) {
		return RV_ERR_GEN;
	}

	scp_set_state(process, SCP_STATE_SLEEP);
	return RV_OK;
}

/**
 * @brief Create fictive father structure to born son.
 * @details
 * Create consts from array @p consts marking arcs with 1_, ...
 * Create @p count_var variables marking arcs with n_, ...
 * Don't create child_death_hook component.
 *
 * @code
 * scp_process -> fictive_father = {
 *     program_: fictive_program = {
 *         var_: {..},     //n variables needed
 *         const_: {..},   // n constants needed
 *     },
 *     var_value_: vv,
 *     active_: {},
 *     autosegment_: autoseg = {}, // create new
 *     opened_segments_: {..}      // have to present
 *     default_seg_: autoseg,
 * };
 * @endcode
 *
 * @note Don't forget about session and cache.
 */
sc_addr construct_fictive_process(sc_session *session, size_t count_const, sc_addr consts[], size_t count_var, sc_addr vars[], sc_addr *pprg)
{
	pm_transaction *s = new pm_transaction(system_session);
	std::auto_ptr<pm_transaction> autoS(s);

	sc_addr process = s->create_el(scp_segment, SC_N_CONST);
	*pprg = s->create_el(scp_segment, SC_N_CONST);
	sc_addr node = s->create_el(scp_segment, SC_N_CONST);

	sc_addr openedSegs = s->create_el(scp_segment, SC_N_CONST);
	gen5_f_cpa_f_cpa_f(s, process, 0, openedSegs, 0, OPENED_SEGMENTS_);
	sc_addr auto_dirent = create_autoseg(s, session, process);
	if (!auto_dirent) {
		SC_ABORT();
	}
	gen5_f_cpa_f_cpa_f(s, process, 0, auto_dirent, 0, DEFAULT_SEG_);

	gen3_f_cpa_f(s, SCP_PROCESS, 0, process);
	reimplement(s, process, new scp_process_activity(session));
	sc_addr arc;
	sc_addr varValue = create_el(s, scp_segment, SC_N_CONST);
	gen5_f_cpa_f_cpa_f(s, process, &arc, varValue, 0, VAR_VALUE_);

	init_cache(node); // LEAK

	gen3_f_cpa_f(s, ACTIVE_, 0, arc);
	gen5_f_cpa_f_cpa_f(s, process, 0,  *pprg, 0, PROGRAM_);

	node = create_el(s, scp_segment, SC_N_CONST);
	gen5_f_cpa_f_cpa_f(s,  *pprg, 0, node, 0, CONST_);

	for (size_t i = 0; i < count_const; i++) {
		gen3_f_cpa_f(s, node, 0, consts[i]);
		session->open_segment(consts[i]->seg->get_full_uri());
	}

	node = create_el(s, scp_segment, SC_N_CONST);
	gen5_f_cpa_f_cpa_f(s,  *pprg, 0, node, 0, VAR_);

	for (size_t i = 0; i < count_var; i++) {
		gen3_f_cpa_f(s, node, 0, vars[i]);
	}

	s->commit();

	return process;
}

/// Erase fictive process tuple.
/// @see construct_fictive_process
/// @see create_orphaned_son
void erase_fictive_process(sc_addr process)
{
	sc_addr prg = sc_tup::at(system_session, process, PROGRAM_);
	SC_ASSERTS(prg, "Cannot find program of process");
	sc_set::erase_from(system_session, prg);
	system_session->erase_el(prg);

	sc_addr var_value = sc_tup::at(system_session, process, VAR_VALUE_);
	SC_ASSERTS(var_value, "Cannot find values set of process");
	dispose_cache(var_value);
	system_session->erase_el(var_value);

	sc_addr autoseg = sc_tup::at(system_session, process, AUTOSEGMENT_);
	SC_ASSERTS(autoseg, "Cannot find autosegment of process");
	system_session->unlink(system_session->sign2uri(autoseg));

	scp_process_activity::retrieve_session(process)->close();

	sc_set::erase_from(system_session, process);
	system_session->erase_el(process);
}

class orphaned_death_hook : public sc_activity
{
public:
	orphaned_death_hook(sc_addr _real_hook): real_hook(_real_hook) {}

	sc_retval init(sc_addr _this) { return RV_OK; }

	void done() {}

	sc_retval activate(sc_session *s, sc_addr _this, sc_addr father, sc_addr son, sc_addr)
	{
		if (real_hook)
			s->activate(real_hook, son);

		erase_fictive_process(father);

		return RV_OK;
	}

private:
	sc_addr real_hook;
};

sc_addr create_orphaned_son(sc_session *session, sc_addr program, sc_addr on_death, sc_addr *pfather,
							sc_addr prms[], bool _consts[], size_t count)
{
	sc_session *s = system_session;

	int varcount = 0;
	int constcount = 0;
	boost::scoped_array<sc_addr> consts(new sc_addr[count]);
	boost::scoped_array<sc_addr> vars(new sc_addr[count]);

	for (size_t i = 0; i < count; ++i) {
		if (_consts[i]) {
			consts[constcount++] = prms[i];
		} else {
			vars[varcount++] = prms[i];
		}
	}

	sc_addr prg = 0;
	sc_addr father = construct_fictive_process(session, constcount, consts.get(), varcount, vars.get(), &prg);
	if (pfather)
		*pfather = father;

	sc_addr params = s->create_el(scp_segment, SC_N_CONST);
	for (size_t i = 0; i < count; ++i)
		sc_tup::add_c(s, params, pm_numattr[i + 1], prms[i]);

	sc_addr son;
	if (scp_create_process(session->__fork(), program, father, params, &son))
		return 0;

	sc_addr node = s->create_el(scp_segment, SC_N_CONST);
	s->reimplement(node, new orphaned_death_hook(on_death));
	sc_tup::add_c(s, father, CHILD_DEATH_HOOK_, node);

	return son;
}

sc_retval scp_spawn_process(sc_session *s, sc_addr prg, sc_addr prm, sc_addr attrs[], int attrs_max, sc_addr *res)
{
	if (!s) {
		s = system_session;
	}
	sc_session *session;
	sc_addr son;
	sc_addr prms[32];
	bool consts[32];
	int prm_cnt;
	sc_retval rv;
	if (!prg) {
		dprintf("No program specified\n");
		return RV_ELSE_GEN;
	}
	for (int i = 0; i < 32; i++) {
		consts[i] = true;
	}

	session = s;
	if (session == system_session) {
		session = libsc_login();
	}

	for (prm_cnt = 0; prm_cnt < attrs_max; prm_cnt++) {
		prms[prm_cnt] = sc_tup::at(s, prm, attrs[prm_cnt]);
		if (!prms[prm_cnt]) {
			break;
		}

		session->open_segment(prms[prm_cnt]->seg->get_full_uri());
		print_el(prms[prm_cnt]);
	}
	if (!prm_cnt) {
		rv = scp_package_run_program(session, prg, 0, 0, &son);

		if (rv) {
			goto out;
		}
	} else {
		son = create_orphaned_son(session, prg, 0, 0, prms, consts, prm_cnt);
		if (!son) {
			dprintf("Failed to start program\n");
			rv = RV_ERR_GEN;
			goto out;
		}

		pm_sched_add_agent(son, SCHED_CLASS_NORMAL);
	}
	if (res) {
		*res = son;
	}

	rv = RV_OK;
out:
	if (session != s) {
		session->close();
	}

	return rv;
}

/// @}
