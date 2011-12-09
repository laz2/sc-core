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
 * @file scp_debugger.cpp
 * @brief Implementation of SCP-debugger.
 * @author Dmitriy Lazurkin
 * @ingroup libpm
 */

/// @addtogroup libpm
/// @{

#include "precompiled_p.h"

#include <limits.h>

static sc_session   *session  = 0;
static scp_debugger *debugger = 0;

static sc_addr breakpoint_op_type = 0;

/// Breakpoint SCP-operator activity.
class breakpoint_activity : public sc_activity
{
public:
	breakpoint_activity() {}

	~breakpoint_activity() {}

	sc_retval init(sc_addr _this) { return RV_OK; }

	void done() {}

	sc_retval activate(sc_session *s, sc_addr _this, sc_addr process, sc_addr, sc_addr)
	{
		//
		// If we here after callReturn then process in state REPEATE.
		// Breakpoint has goto_ component to next operator. Otherwise set SCP_STATE_THEN.
		//
		scp_set_state(process, SCP_STATE_THEN);

		//
		// If process hasn't tracer then attach debugger to it.
		// TODO: Handle other tracers
		//
		if (!scp_process_info::is_traced(process))
			debugger->attach(process);

		//
		// Breakpoint must sent event only after resume command.
		// After resume process not in step tracing mode.
		//
		if (!scp_process_info::is_step_tracing(process)) {
			sc_addr brk = scp_process_info::get_active_op(process);
			return session->activate(debugger->get_sign(), scp_debugger::BreapointEvent, process, brk);
		} else {
			return RV_OK;
		}
	}

};

sc_retval scp_debugger::init(sc_session *session_)
{
	session = session_;

	scp_debugger::keynodes_segment = create_segment_full_path(session, "/proc/scp_debugger/keynode");
	create_keynodes(session, scp_debugger::keynodes_count,
		scp_debugger::keynodes_uris, scp_debugger::keynodes);

	session->reimplement(BREAKPOINT, new breakpoint_activity());
	gen3_f_cpa_f(session, scp_segment, SCP_OPERATOR_TYPE, 0, BREAKPOINT);

	debugger = scp_debugger::create(scp_debugger::keynodes_segment);
	session->set_idtf(debugger->get_sign(), "Debugger");
	return RV_OK;
}

void scp_debugger::deinit()
{
	if (debugger)
		debugger->raise_event(TerminatedEvent);
}

/// Move input executing flow with attribute @p attr from scp-operator @p from to scp-operator @p to.
static
void move_input_arcs(sc_addr from, sc_addr to, sc_addr attr)
{
	sc_iterator *it = session->create_iterator(sc_constraint_new(
		CONSTR_5_a_a_f_a_f,
		0,
		0,
		from,
		0,
		attr), true);
	sc_for_each (it) session->set_end(it->value(1), to);
}

/// Move input executing flow from scp-operator @p from to scp-operator @p to.
/// @note Use <code>goto_</code>, <code>then_</code>, <code>else_</code>.
static
void move_input_branch_arcs(sc_addr from, sc_addr to)
{
	move_input_arcs(from, to, GOTO_);
	move_input_arcs(from, to, THEN_);
	move_input_arcs(from, to, ELSE_);
}


//
// Methods for control scp-process
// {
//

void scp_debugger::attach(sc_addr process)
{
	if (!scp_process_info::is_traced(process)) {
		scp_process_info::set_tracer(process, sign);
		scp_process_info::set_traced(process, true);
	}
}

void scp_debugger::suspend_one(sc_addr process)
{
	if (sc_set::is_in(session, process, processSuspended)) {
		// Process already suspended
		return;
	}

	attach(process);

	sc_set::include_in(session, process, processSuspended);

	if (pm_sched_is_sleeping(process)) {
		//
		// Process is already sleeping. May be it waits event.
		// Turn on step tracing for catch its wakeup and
		// include it in processWasSleeping.
		//
		scp_process_info::set_step_tracing(process, true);
		sc_set::include_in(session, process, processWasSleeping);
	} else {
		scp_process_info::set_step_tracing(process, false);
		pm_sched_put_to_sleep(process);
	}
}

void scp_debugger::suspend(sc_addr process)
{
	if (process) {
		suspend_one(process);
	} else {
		//
		// Suspend all scp-processes.
		//
		sc_iterator *it = session->create_iterator(sc_constraint_new(
			CONSTR_3_f_a_a, SCP_PROCESS, SC_A_CONST|SC_POS, 0), true);
		sc_for_each (it) suspend_one(it->value(2));
	}

	raise_event(SuspenedEvent, process);
}

void scp_debugger::resume_one(sc_addr process)
{
	if (!sc_set::exclude_from(session, process, processSuspended))
		return;

	if (sc_set::exclude_from(session, process, processWasSleeping)) {
		//
		// Process was sleeping, just remove from sets
		// and turn off step tracing.
		//
		scp_process_info::set_step_tracing(process, false);
	} else {
		pm_sched_wakeup(process);
		scp_process_info::set_step_tracing(process, false);
	}
}

void scp_debugger::resume(sc_addr process)
{
	if (process) {
		resume_one(process);
	} else {
		//
		// Resume all suspended scp-processes.
		//
		sc_iterator *it = session->create_iterator(sc_constraint_new(
			CONSTR_3_f_a_a, processSuspended, SC_A_CONST|SC_POS, 0), true);
		sc_for_each (it) resume_one(it->value(2));
	}

	raise_event(ResumedEvent, process);
}

void scp_debugger::terminate_one(sc_addr process)
{
	pm_sched_wakeup(process); // If process is sleeping, then wakeup it.
	scp_process_info::set_step_tracing(process, true);
	sc_set::include_in(session, process, processForTerminate);
}

void scp_debugger::terminate(sc_addr process)
{
	if (process) {
		terminate_one(process);
	} else {
		//
		// Terminate all scp-processes.
		//
		sc_iterator *it = session->create_iterator(sc_constraint_new(
			CONSTR_3_f_a_a, SCP_PROCESS, SC_A_CONST|SC_POS, 0), true);
		sc_for_each (it) terminate_one(it->value(2));
	}
}

void scp_debugger::step_over(sc_addr process)
{
	resume_one(process);
	scp_process_info::set_step_tracing(process, true);
	raise_event(StepOverEvent, process);
}

void scp_debugger::step_into(sc_addr process)
{
	resume_one(process);
	scp_process_info::set_step_tracing(process, true);
	sc_set::include_in(session, process, processStepInto);
	raise_event(StepIntoEvent, process);
}

void scp_debugger::step_return(sc_addr process)
{
	resume_one(process);
	sc_set::include_in(session, process, processStepReturn);
	raise_event(StepReturnEvent, process);
}

//
// }
//


scp_debugger::scp_debugger() :
	reciever(0),
	debug_copies(0),
	breakpoints(0)
{}

scp_debugger::~scp_debugger()
{
}

scp_debugger *scp_debugger::get_instance()
{
	return debugger;
}

sc_retval scp_debugger::init(sc_addr _this)
{
	sc_retval rv = advanced_activity<scp_debugger>::init(_this);
	if (rv)
		return rv;

	sc_generator g(session, scp_segment);
	g.element(DEBUGGER);
	g.arc();
	g.enter_set(sign); {
		g.attr(BREAKPOINTS_);
		breakpoints = g.element("");
		g.attr(DEBUG_COPIES_);
		debug_copies = g.element("");
	}; g.leave_set();
	g.finish();

	return RV_OK;
}

void scp_debugger::done()
{
	sc_iterator *it = session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, debug_copies, 0, 0), true);
	std::auto_ptr<sc_iterator> autoIt(it);
	for (; !it->is_over(); it->next()) {
		sc_addr tuple = it->value(2);
		sc_addr copy = sc_tup::at(session, tuple, N2_);
		session->unlink(copy->seg->get_full_uri());
		session->erase_el(tuple);
	}
	autoIt.reset();

	session->erase_el(debug_copies);
	// TODO: remove all breakpoints
	session->erase_el(breakpoints);

	advanced_activity<scp_debugger>::done();
}


//
// Methods for manage breakpoints
// {
//

/// Return operator with breakpoint @p brk.
static
inline sc_addr get_op_with_brk(sc_addr brk)
{
	assert(scp_debugger::is_breakpoint(brk));
	return sc_tup::at(session, brk, GOTO_);
}

bool scp_debugger::is_breakpoint(sc_addr op)
{
	return sc_set::is_in(session, op, BREAKPOINT) != 0;
}

sc_addr scp_debugger::set_breakpoint(sc_addr program, sc_addr op)
{
	assert(program);
	assert(op);

	sc_generator g(session, op->seg);
	g.element(BREAKPOINT);
	g.arc();
	sc_addr breakpoint = g.element("");

	if (program) {
		g.element(program);
		g.arc();
		g.element(breakpoint);
	}

	move_input_branch_arcs(op, breakpoint);

	sc_addr init_arc;
	if (!search_oneshot(session, sc_constraint_new(CONSTR_5_f_a_f_a_f,
			program,
			0,
			op,
			0,
			INIT_
		), true, 1, 1, &init_arc)) {
		session->set_end(init_arc, breakpoint);
	}

	g.arc();
	g.attr(GOTO_);
	g.element(op);

	//
	// May be scp-processes already have op as active operator.
	// Set breakpoint as active operator for that processes.
	//
	{
		sc_iterator *it = session->create_iterator(
			sc_constraint_new(CONSTR_3l2_f_a_a_a_f,
				SCP_PROCESS, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, op), true);
		sc_for_each (it) {
			sc_addr active_op_arc = it->value(3);
			session->set_end(active_op_arc, breakpoint);
		}
	}

	g.finish();

	return breakpoint;
}

/// Pair maps line number to scp-operator.
/// @see scp_debugger::find_op_for_line
typedef std::pair<size_t, sc_addr> line2addr_pair;

/// Map line numbers to each scp-operator in program.
/// @see scp_debugger::find_op_for_line
typedef std::list<line2addr_pair>  line2addr_list;

/// Comparator for #line2addr_pair. Compare line numbers.
/// @see scp_debugger::find_op_for_line
struct line2addr_pair_comparator : public std::binary_function<line2addr_pair, line2addr_pair, bool>
{
	bool operator()(const line2addr_pair &a, const line2addr_pair &b) const
	{
		return a.first < b.first;
	}
};

sc_addr scp_debugger::find_op_for_line(sc_addr program, size_t line)
{
	assert(program);
	assert(line > 0 && "Expect positive line number");

	//
	// Search nearest operator with greater or equal line number, because
	// at this moment m4scp puts in debug information line number of last operator line.
	//

	//
	// Simplest and furious way for search operator with specified line number:
	// 1. Make list of pairs "line - operator".
	// 2. Add to this list line number @p line for operator #SCADDR_NIL.
	// 3. Add to this list pairs "line - operator" for all operator of @p program.
	//    If we found operator with line number @p line, then search success - return that operator.
	// 4. Sort ascending this list by first pair element (line number).
	// 5. Search pair with line number @p line.
	// 6. The next of this pair is our operator.
	//    If pair at end of list then line number @p line is out of range program source code.
	//

	//
	// Pass 1
	//
	line2addr_list line2op;

	//
	// Pass 2
	//
	line2op.push_back(std::make_pair(line, SCADDR_NIL));

	//
	// Pass 3
	//
	{
		sc_iterator *it = session->create_iterator(
			sc_constraint_new(CONSTR_3_f_a_a, program, SC_A_CONST|SC_POS, SC_N_CONST), true);
		sc_for_each (it) {
			sc_addr op_arc = it->value(1);
			sc_addr op     = it->value(2);

			if (!sc_set::is_in_any(session, op_arc, VAR_, CONST_, PRM_, CONST_SEGMENTS_)) {
				size_t op_line = get_op_line(op);
				if (op_line == line) {
					return op;
				} else {
					// TODO: may be add pair in ascending order?
					line2op.push_back(std::make_pair(op_line, op));
				}
			}
		}
	}

	//
	// Pass 4
	//
	line2op.sort(line2addr_pair_comparator());

	//
	// Pass 5
	//
	line2addr_list::iterator it = std::find(line2op.begin(),
		line2op.end(), std::make_pair(line, SCADDR_NIL));

	//
	// Pass 6
	//
	if (it == line2op.end()) {
		return 0;
	} else {
		return (++it)->second;
	}
}

sc_addr scp_debugger::set_breakpoint(sc_addr program, size_t line)
{
	if (diag_output) {
		std::cout << print_f("[SCPDBG] Start setting breakpoint on program '%s', line %d",
			FULL_IDTF(program).c_str(), line) << std::endl;
	}

	sc_addr op_for_brk = 0;

	//
	// If @p line is -1 then set breakpoint on init operator.
	//
	if (line == -1) {
		op_for_brk = get_init_op(program);
	} else {
		op_for_brk = find_op_for_line(program, line);
	}

	//
	// If we found operator @p op_with_brk then set breakpoint on it.
	//
	if (op_for_brk) {
		if (diag_output) {
			std::cout << print_f("[SCPDBG] Setting breakpoint on operator '%s' with type '%s' and line %d",
				SC_IDc(op_for_brk),
				SC_IDc(get_op_type(op_for_brk)),
				get_op_line(op_for_brk)) << std::endl;
		}

		return set_breakpoint(program, op_for_brk);
	} else {
		if (diag_output) {
			std::cout << print_f("[SCPDBG] Operator on '%s', line %d not found",
				SC_URIc(program), line) << std::endl;
		}

		return 0;
	}
}

void scp_debugger::remove_breakpoint(sc_addr brk)
{
	assert(is_breakpoint(brk));
	// TODO: implement
}

void scp_debugger::unset_breakpoint(sc_addr program, sc_addr op)
{
	sc_addr breakpoint, init_arc;
	
	if (search_oneshot(session,sc_constraint_new(CONSTR_5_a_a_f_a_f,
			0, // breakpoint
			0,
			op,
			0,
			GOTO_
		), true, 1, 0, &breakpoint)) {
		return;
	}
	
	move_input_branch_arcs(breakpoint, op);

	if (!search_oneshot(session,sc_constraint_new(CONSTR_5_a_a_f_a_f,
			0, // SCP-program
			0,
			breakpoint,
			0,
			INIT_
		), true, 1, 1, &init_arc)) {
		session->set_end(init_arc, op);
	}

	session->erase_el(breakpoint);

	return;
}

//
// }
//


static
void log_event_or_command_param(sc_addr prm)
{
	if (prm) {
		std::cout << "<" << get_addr_full_uri(prm);
		const Content *c = system_session->get_content_const(prm);
		std::cout << ", " << *c << ">";
	} else {
		std::cout << "NIL";
	}
}

void scp_debugger::log_event_or_command(sc_addr prm1, sc_addr prm2, sc_addr prm3)
{
	if (diag_output) {
		std::cout << "[SCPDBG] --> " << system_session->get_idtf(prm1) << "( ";
		log_event_or_command_param(prm2);
		std::cout << ", ";
		log_event_or_command_param(prm3);
		std::cout << " )" << std::endl;
	}
}

sc_string log_as_string(sc_addr process, sc_addr op)
{
	std::ostringstream out;
	out << print_f("%s::%s ( %s, line %d )", 
		SC_URIc(scp_process_info::get_program(process)),
		SC_IDc(op),
		SC_IDc(get_op_type(op)),
		get_op_line(op));
	return out.str();
}

//
// Methods for raising events to reciever.
// {
//

sc_retval scp_debugger::raise_event(sc_addr event, sc_addr prm1, sc_addr prm2)
{
	if (reciever) {
		return session->activate(reciever, event, prm1, prm2);
	} else {
		return RV_OK;
	}
}

sc_retval scp_debugger::raise_breakpoint_event(sc_addr process, sc_addr op)
{
	if (reciever) {
		sc_addr line_node = session->create_el(keynodes_segment, SC_U_CONST);
		session->set_content(line_node, Content::integer(get_op_line(op)));

		sc_retval rv = session->activate(reciever, BreapointEvent, process, line_node);

		session->erase_el(line_node);

		return rv;
	} else {
		return RV_OK;
	}
}

sc_retval scp_debugger::raise_step_event(sc_addr process, sc_addr op)
{
	if (reciever) {
		sc_addr line_node = session->create_el(keynodes_segment, SC_U_CONST);
		session->set_content(line_node, Content::integer(get_op_line(op)));

		sc_retval rv = session->activate(reciever, StepEndEvent, process, line_node);

		session->erase_el(line_node);

		return rv;
	} else {
		return RV_OK;
	}
}

sc_retval scp_debugger::raise_breakpoint_changed(sc_addr program, sc_addr brk, size_t line)
{
	//
	// Notification reciever of requested breakpoint line and real line.
	// Use for it #scp_debugger_keynodes::breakpoint_changed_event. Parameters:
	// @arg @c program
	// @arg @c tuple of parameters: { 1_: @c requested_line_number, 2_: @c real_line_number }
	//

	//
	// Create element with requested line number as content.
	//
	sc_addr line_node = session->create_el(keynodes_segment, SC_U_CONST);
	session->set_content(line_node, Content::integer(line));

	//
	// Retrieve real line number of breakpoint and create element with this number as content.
	//
	sc_addr op_with_brk = get_op_with_brk(brk);
	size_t new_line = get_op_line(op_with_brk);
	sc_addr new_line_node = session->create_el(keynodes_segment, SC_U_CONST);
	session->set_content(new_line_node, Content::integer(new_line));

	//
	// Create tuple of parameters.
	//
	sc_addr prms_tuple = sc_tup::create_ord(session, keynodes_segment, line_node, new_line_node);

	//
	// Activate reciever.
	//
	sc_retval rv = session->activate(reciever, BreakpointChangedEvent, program, prms_tuple);

	//
	// Clear memory.
	//
	session->erase_el(prms_tuple);
	session->erase_el(new_line_node);
	session->erase_el(line_node);

	return rv;
}

//
// }
//


//
// Methods for handling commands from reciever.
// {
//

sc_retval scp_debugger::handle_register(sc_addr sign, sc_addr)
{
	reciever = sign;
	return RV_OK;
}

sc_retval scp_debugger::handle_set_breakpoint(sc_addr program, sc_addr node_with_line)
{
	//
	// At first check correctness of command parameters:
	// @arg @c program in which set breakpoint
	// @arg @c node_with_line sc-element with integer content which holds line number for breakpoint
	//

	if (!program)
		return RV_ERR_GEN;

	if (!sc_set::is_in(session, program, SCP_PROGRAM)) {
		if (diag_output) {
			std::cout << "[SCPDBG] " << FULL_IDTF(program) << " must be " <<
				FULL_IDTF(SCP_PROGRAM) << std::endl;
		}
		return RV_ERR_GEN;
	}

	//
	// Retrieve line number from @a node_with_line content if it passed.
	// If @a node_with_line isn't passed then set breakpoint on first line of program.
	//
	size_t line = -1;
	if (node_with_line) {
		const Content *c = session->get_content_const(node_with_line);
		if (c->type() == TCINT) {
			line = static_cast<Cont>(*c).i;
		} else {
			if (diag_output)
				std::cout << "[SCPDBG] Second parameter must have INT content" << std::endl;
			return RV_ERR_GEN;
		}
	}

	//
	// Set breakpoint on @a line in @a program.
	//
	sc_addr brk = set_breakpoint(program, line);

	if (brk) {
		return raise_breakpoint_changed(program, brk, line);
	} else {
		return RV_ERR_GEN;
	}
}

sc_retval scp_debugger::handle_unset_breakpoint(sc_addr, sc_addr)
{
	assert(false && "Not implemented");
	return RV_OK;
}

sc_retval scp_debugger::handle_terminate(sc_addr process, sc_addr)
{
	terminate(process);
	// TODO: May be terminate PM?
	return RV_OK;
}

sc_retval scp_debugger::handle_unregister(sc_addr, sc_addr)
{
	reciever = SCADDR_NIL;
	return RV_OK;
}

//
// }
//


//
// Methods for handling events from scp-process.
// {
//

sc_retval scp_debugger::catch_breakpoint_event(sc_addr process, sc_addr brk)
{
	//
	// Breakpoint was activated only if debugger not in step tracing mode.
	//

	if (diag_output) {
		sc_addr op = get_op_with_brk(brk);
		std::cout << "[SCPDBG] Activate breakpoint on " << log_as_string(process, op) << std::endl;
	}

	sc_addr tuple = 0;
	sc_rel::bin_ord_at_2(session, processStackState, process, &tuple);
	if (tuple) {
		//
		// Process is stepping over program call and waits for stack state.
		// But activated breakpoint then remove remembered stack state.
		//
		session->erase_el(tuple);
	}

	//
	// Skip breakpoint and make step to next operator.
	//
	scp_process_info::set_step_tracing(process, true);
	sc_set::include_in(session, process, processStepAfterBreak);

	return RV_OK;
}

sc_retval scp_debugger::catch_step_before_event(sc_addr process, sc_addr op)
{
	if (diag_output) {
		std::cout << "[SCPDBG] Step before " << log_as_string(process, op) << std::endl;
	}

	//
	// May be process is yet suspended, but isn't sleeping.
	// Somebody wakeup process without debugger.
	// Then loop until process is suspended.
	//
	while (sc_set::is_in(session, process, processSuspended))
		pm_sched_put_to_sleep_and_switch(process);

	//
	// Skip breakpoints.
	//
	if (is_breakpoint(op))
		return RV_OK;

	//
	// If process was terminated then just skip this event.
	// #catch_step_after_event will handle that behavior.
	//
	if (sc_set::is_in(session, process, processForTerminate))
		return RV_OK;

	//
	// Step into without call. Exclude from processStepInto.
	//
	sc_set::exclude_from(session, process, processStepInto);

	if (sc_set::exclude_from(session, process, processStepAfterBreak)) {
		raise_breakpoint_event(process, op);
	} else {
		raise_step_event(process, op);
	}

	suspend_one(process);
	context_switchback();

	return RV_OK;
}

sc_retval scp_debugger::catch_step_after_event(sc_addr process, sc_addr op)
{
	if (diag_output) {
		std::cout << "[SCPDBG] Step after " << log_as_string(process, op) << std::endl;
	}

	//
	// This method used only for terminate processes.
	// Set dead state for process which was terminated
	// (it in set #processForTerminate).
	//
	if (sc_set::exclude_from(session, process, processForTerminate)) {
		scp_set_state(process, SCP_STATE_DEAD);
	} else {
		//
		// Check if process step over from call in top context.
		//
		if (sc_set::exclude_from(session, process, processStepReturn)) {
			raise_step_event(process, op);
			suspend_one(process);
			context_switchback();
		}
	}

	return RV_OK;
}

sc_retval scp_debugger::catch_call_event(sc_addr process, sc_addr op)
{
	if (diag_output)
		std::cout << "[SCPDBG] Call event from " << log_as_string(process, op) << std::endl;

	if (!sc_set::exclude_from(session, process, processStepInto)) {
		//
		// Process isn't stepping into,
		// then remember stack state for return detecting.
		//

		if (scp_process_info::is_step_tracing(process)) {
			sc_addr stack = sc_tup::at(session, process, scp::processStack_);
			sc_rel::add_ord_tuple(session, processStackState, process, stack);
			scp_process_info::set_step_tracing(process, false);
		}
	}

	if (scp_process_info::is_step_tracing(process))
		raise_event(CallEvent, process);

	return RV_OK;
}

sc_retval scp_debugger::catch_return_event(sc_addr process, sc_addr op)
{
	if (diag_output)
		std::cout << "[SCPDBG] Return event from " << log_as_string(process, op) << std::endl;

	bool stepping_return = sc_set::is_in(session, process, processStepReturn) != SCADDR_NIL;

	if (scp_process_info::is_step_tracing(process) || stepping_return)
		raise_event(ReturnEvent, process);

	if (!stepping_return) {
		sc_addr tuple = 0;
		sc_addr expected_stack = sc_rel::bin_ord_at_2(session, processStackState, process, &tuple);
		if (expected_stack) {
			//
			// Process is stepping over program call and waits return.
			// Compare saved stack state with current stack state.
			//

			sc_addr cur_stack = sc_tup::at(session, process, scp::processStack_);
			if (expected_stack == cur_stack) {
				scp_process_info::set_step_tracing(process, true);
				session->erase_el(tuple);
			}
		} else {
			//
			// May be we step over to top context?
			//
			if (scp_process_info::is_step_tracing(process))
				sc_set::include_in(session, process, processStepReturn);
		}
	} else {
		scp_process_info::set_step_tracing(process, true);
	}

	return RV_OK;
}

sc_retval scp_debugger::catch_death_event(sc_addr process, sc_addr)
{
	sc_addr program = scp_process_info::get_program(process);

	if (diag_output) {
		std::cout << "[SCPDBG] Process death " << SC_URI(program) << std::endl;
	}

	// We don't need to do anything because process sign will erased.
	//
	// sc_set::exclude_from(session, process,
	//                      processSuspended,
	//                      processStepAfterBreak,
	//                      processStepInto,
	//                      processForTerminate);

	sc_addr tuple = 0;
	sc_rel::bin_ord_at_2(session, processStackState, process, &tuple);
	if (tuple)
		session->erase_el(tuple);

	raise_event(TerminatedEvent, process);

	return RV_OK;
}

sc_retval scp_debugger::catch_child_event(sc_addr father, sc_addr child)
{
	if (diag_output) {
		sc_addr father_op = scp_process_info::get_active_op(father);
		sc_addr child_program = scp_process_info::get_program(child);

		std::cout << "[SCPDBG] Child fork " << log_as_string(father, father_op) <<
			", child " << get_addr_full_uri(child_program) << std::endl;
	}

	raise_event(ChildEvent, father, child);

	return RV_OK;
}

sc_retval scp_debugger::catch_error_event(sc_addr process, sc_addr op)
{
	sc_addr program = scp_process_info::get_program(process);
	int line_number = get_op_line(op);

	if (diag_output) {
		std::cout << "[SCPDBG] Error on " << log_as_string(process, op) << std::endl;
	}

	// TODO: event to reciever

	return RV_OK;
}

//
// }
//


/// SCP-program copier.
class copier 
{
public:
	sc_addr program_copy;

	copier(addr_set &_aset, addr_set &_operators, sc_addr _program)
		: aset(_aset), operators(_operators), program(_program), g(session, 0)
	{
		program_copy = 0;
		copies = 0;
	}

	void add_operators()
	{
	}

	sc_retval copy()
	{
		sc_segment *seg = create_unique_segment(session, "/tmp/dbg_copy");
		
		if (!seg) {
			return RV_ERR_GEN;
		}

		g.set_active_segment(seg);
		copies = g.enter_set("");
		sc_set::copy(session,seg,aset,(sc_set::copy_callback)___callback,this);
		g.leave_set();
		g.finish();
		assert(program_copy != 0);
		g.element(program_copy);
		g.attr(ORIGINALS_);
		g.arc();
		g.element(copies);
		g.finish();
		return RV_OK;
	}

	void callback(sc_addr original, sc_addr copy)
	{
		if (operators.find(original) != operators.end()) {
			g.enter_set(""); {
				g.attr(N1_);
				g.element(original);
				g.attr(N2_);
				g.element(copy);
			}; g.leave_set();
		} else if (original == program) {
			program_copy = copy;
		}
	}

	static void ___callback(sc_addr original, sc_addr copy, class copier *m_this)
	{
		m_this->callback(original,copy);
	}

private:
	addr_set &aset;
	addr_set &operators;
	sc_addr program;
	sc_generator g;
	sc_addr copies;
};

sc_addr scp_debugger::get_program_copy(sc_addr program)
{
	return sc_rel::bin_ord_at_2(session, debug_copies, program);
}

sc_addr scp_debugger::get_program_original(sc_addr program_copy)
{
	return sc_rel::bin_ord_at_1(session, debug_copies, program_copy);
}

sc_addr scp_debugger::get_op_copy(sc_addr program_copy, sc_addr op)
{
	sc_addr copies = sc_tup::at(session, program_copy, ORIGINALS_);
	
	if (!copies) {
		return 0;
	}

	return sc_rel::bin_ord_at_2(session, copies, op);
}

sc_addr scp_debugger::get_op_original(sc_addr program_copy, sc_addr op_copy)
{
	sc_addr copies = sc_tup::at(session, program_copy, ORIGINALS_);
	
	if (!copies) {
		return 0;
	}

	return sc_rel::bin_ord_at_1(session, copies, op_copy);
}

sc_addr scp_debugger::copy_program(sc_addr program)
{
	sc_addr copy = get_program_copy(program);
	if (copy) {
		return copy;
	}

	addr_set aset;
	addr_set operators;
	sc_retval rv = scp_package_locate_program_text(session, program, aset, &operators);
	if (rv) {
		return 0;
	}

	copier cp(aset, operators, program);
	cp.copy();
	copy = cp.program_copy;

	addr_set::iterator it = operators.begin();
	for (; it != operators.end(); it++) {
		if (!sc_set::is_in(session, *it, copy)) {
			gen3_f_cpa_f(session, scp_segment, copy, 0, *it);
		}
	}
	
	sc_generator g(session, scp_segment);
	g.element(debug_copies);
	g.arc();
	g.enter_set(""); {
		g.attr(N1_);
		g.element(program);
		g.attr(N2_);
		g.element(copy);
	}; g.leave_set();
	g.finish();

	return copy;
}

//
// Keynodes of scp-debugger.
//

const char* scp_debugger::keynodes_uris[] = {
	// Commands from reciever to debugger
	"/proc/scp_debugger/keynode/RegisterCommand",
	"/proc/scp_debugger/keynode/SetBreakpointCommand",
	"/proc/scp_debugger/keynode/UnsetBreapointCommand",
	"/proc/scp_debugger/keynode/SuspendCommand",
	"/proc/scp_debugger/keynode/ResumeCommand",
	"/proc/scp_debugger/keynode/StepOverCommand",
	"/proc/scp_debugger/keynode/StepIntoCommand",
	"/proc/scp_debugger/keynode/StepReturnCommand",
	"/proc/scp_debugger/keynode/TerminateCommand",
	"/proc/scp_debugger/keynode/UnregisterCommand",

	// Debug states of scp-process
	"/proc/scp_debugger/keynode/processSuspended",
	"/proc/scp_debugger/keynode/processWasSleeping",
	"/proc/scp_debugger/keynode/processStepAfterBreak",
	"/proc/scp_debugger/keynode/processStepInto",
	"/proc/scp_debugger/keynode/processStepReturn",
	"/proc/scp_debugger/keynode/processStackState",
	"/proc/scp_debugger/keynode/processForTerminate",

	// Events from debugger to reciever
	"/proc/scp_debugger/keynode/BreapointChangedEvent",
	"/proc/scp_debugger/keynode/SuspenedEvent",
	"/proc/scp_debugger/keynode/ResumedEvent",
	"/proc/scp_debugger/keynode/StepOverEvent",
	"/proc/scp_debugger/keynode/StepIntoEvent",
	"/proc/scp_debugger/keynode/StepReturnEvent",
	"/proc/scp_debugger/keynode/StepEndEvent",
	"/proc/scp_debugger/keynode/BreapointEvent",
	"/proc/scp_debugger/keynode/TerminatedEvent",
	"/proc/scp_debugger/keynode/ChildEvent",
	"/proc/scp_debugger/keynode/CallEvent",
	"/proc/scp_debugger/keynode/ReturnEvent"
};

sc_segment *scp_debugger::keynodes_segment;

const int scp_debugger::keynodes_count = sizeof(scp_debugger::keynodes_uris) / sizeof(const char*);

sc_addr scp_debugger::keynodes[scp_debugger::keynodes_count];

// Commands from reciever to debugger
const sc_addr &scp_debugger::RegisterCommand        = scp_debugger::keynodes[0];
const sc_addr &scp_debugger::SetBreakpointCommand   = scp_debugger::keynodes[1];
const sc_addr &scp_debugger::UnsetBreakpointCommand = scp_debugger::keynodes[2];
const sc_addr &scp_debugger::SuspendCommand         = scp_debugger::keynodes[3];
const sc_addr &scp_debugger::ResumeCommand          = scp_debugger::keynodes[4];
const sc_addr &scp_debugger::StepOverCommand        = scp_debugger::keynodes[5];
const sc_addr &scp_debugger::StepIntoCommand        = scp_debugger::keynodes[6];
const sc_addr &scp_debugger::StepReturnCommand      = scp_debugger::keynodes[7];
const sc_addr &scp_debugger::TerminateCommand       = scp_debugger::keynodes[8];
const sc_addr &scp_debugger::UnregisterCommand      = scp_debugger::keynodes[9];

// Debug states of scp-process
const sc_addr &scp_debugger::processSuspended       = scp_debugger::keynodes[10];
const sc_addr &scp_debugger::processWasSleeping     = scp_debugger::keynodes[11];
const sc_addr &scp_debugger::processStepAfterBreak  = scp_debugger::keynodes[12];
const sc_addr &scp_debugger::processStepInto        = scp_debugger::keynodes[13];
const sc_addr &scp_debugger::processStepReturn      = scp_debugger::keynodes[14];
const sc_addr &scp_debugger::processStackState      = scp_debugger::keynodes[15];
const sc_addr &scp_debugger::processForTerminate    = scp_debugger::keynodes[16];

// Events from debugger to reciever
const sc_addr &scp_debugger::BreakpointChangedEvent = scp_debugger::keynodes[17];
const sc_addr &scp_debugger::SuspenedEvent          = scp_debugger::keynodes[18];
const sc_addr &scp_debugger::ResumedEvent           = scp_debugger::keynodes[19];
const sc_addr &scp_debugger::StepOverEvent          = scp_debugger::keynodes[20];
const sc_addr &scp_debugger::StepIntoEvent          = scp_debugger::keynodes[21];
const sc_addr &scp_debugger::StepReturnEvent        = scp_debugger::keynodes[22];
const sc_addr &scp_debugger::StepEndEvent           = scp_debugger::keynodes[23];
const sc_addr &scp_debugger::BreapointEvent         = scp_debugger::keynodes[24];
const sc_addr &scp_debugger::TerminatedEvent        = scp_debugger::keynodes[25];
const sc_addr &scp_debugger::ChildEvent             = scp_debugger::keynodes[26];
const sc_addr &scp_debugger::CallEvent              = scp_debugger::keynodes[27];
const sc_addr &scp_debugger::ReturnEvent            = scp_debugger::keynodes[28];

/// @}
