
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
 * @file scp_debugger.h
 * @brief Interface of SCP-debugger.
 * @author Dmitriy Lazurkin
 * @ingroup libpm
 */

#ifndef __LIBPM_SCP_DEBUGGER_H_INCLUDED__
#define __LIBPM_SCP_DEBUGGER_H_INCLUDED__

/// @addtogroup libpm
/// @{

#include <libsc/advanced_activity.h>


LIBPM_API sc_retval init_scp_debugger(sc_session *session);


LIBPM_API void deinit_scp_debugger();

/// Class of SCP-debugger.
class LIBPM_API scp_debugger : public advanced_activity<scp_debugger>
{
public:
	static const char *keynodes_uris[];

	static sc_segment *keynodes_segment;

	static const int keynodes_count;

	static sc_addr keynodes[];

	// Commands from reciever to debugger
	static const sc_addr &RegisterCommand;
	static const sc_addr &SetBreakpointCommand;
	static const sc_addr &UnsetBreakpointCommand;
	static const sc_addr &ResumeCommand;
	static const sc_addr &SuspendCommand;
	static const sc_addr &StepOverCommand;
	static const sc_addr &StepIntoCommand;
	static const sc_addr &StepReturnCommand;
	static const sc_addr &TerminateCommand;
	static const sc_addr &UnregisterCommand;

	// Debug states of scp-process
	static const sc_addr &processSuspended;
	static const sc_addr &processWasSleeping;
	static const sc_addr &processStepAfterBreak;
	static const sc_addr &processStepInto;
	static const sc_addr &processStepReturn;
	static const sc_addr &processStackState;
	static const sc_addr &processForTerminate;

	// Events from debugger to reciever
	static const sc_addr &BreakpointChangedEvent;
	static const sc_addr &SuspenedEvent;
	static const sc_addr &ResumedEvent;
	static const sc_addr &StepOverEvent;
	static const sc_addr &StepIntoEvent;
	static const sc_addr &StepReturnEvent;
	static const sc_addr &StepEndEvent;
	static const sc_addr &BreapointEvent;
	static const sc_addr &TerminatedEvent;
	static const sc_addr &ChildEvent;
	static const sc_addr &CallEvent;
	static const sc_addr &ReturnEvent;

public:
	scp_debugger();

	~scp_debugger();

	sc_retval init(sc_addr _this);

	void done();
	
	RAA_DEFINE_DISPATCH() {
		RAA_DISPATCH(RegisterCommand,        handle_register);
		RAA_DISPATCH(SetBreakpointCommand,   handle_set_breakpoint);
		RAA_DISPATCH(UnsetBreakpointCommand, handle_unset_breakpoint);
		RAA_DISPATCH(ResumeCommand,          handle_resume);
		RAA_DISPATCH(SuspendCommand,         handle_suspend);
		RAA_DISPATCH(StepOverCommand,        handle_step_over);
		RAA_DISPATCH(StepIntoCommand,        handle_step_into);
		RAA_DISPATCH(StepReturnCommand,      handle_step_return);
		RAA_DISPATCH(TerminateCommand,       handle_terminate);
		RAA_DISPATCH(UnregisterCommand,      handle_unregister);

		RAA_DISPATCH(PROCESS_ERROR_EVENT,      catch_error_event);
		RAA_DISPATCH(PROCESS_STEP_EVENT,       catch_step_before_event);
		RAA_DISPATCH(PROCESS_STEP_AFTER_EVENT, catch_step_after_event);
		RAA_DISPATCH(PROCESS_CALL_EVENT,       catch_call_event);
		RAA_DISPATCH(PROCESS_RETURN_EVENT,     catch_return_event);
		RAA_DISPATCH(PROCESS_CHILD_EVENT,      catch_child_event);
		RAA_DISPATCH(BreapointEvent,           catch_breakpoint_event);
		RAA_DISPATCH(PROCESS_DEATH_EVENT,      catch_death_event);
	}

	/// Return singleton of scp-debugger.
	static scp_debugger* get_instance();

	/// Init scp-debugger subsystem.
	static sc_retval init(sc_session *session);

	static void deinit();

/// name Methods for control scp-process
/// @{
public:
	/// Attach debugger to process.
	void attach(sc_addr process);

	/// Suspend process.
	/// If @p process is #SCADDR_NIL then suspend all scp-processes.
	void suspend(sc_addr process=SCADDR_NIL);

	/// Resume process.
	/// If @p process is #SCADDR_NIL then resume all suspended scp-processes.
	void resume(sc_addr process=SCADDR_NIL);

	/// Terminate process.
	/// If @p process is #SCADDR_NIL then terminate all scp-processes.
	void terminate(sc_addr process=SCADDR_NIL);

private:
	/// Suspend only one process.
	/// @see #suspend
	void suspend_one(sc_addr process);

	/// Resume only one process.
	/// @see #resume
	void resume_one(sc_addr process);

	/// Terminate only one process.
	/// @see #terminate
	void terminate_one(sc_addr process);

	/// Make step over for process.
	void step_over(sc_addr process);

	/// Make step into for process.
	void step_into(sc_addr process);

	/// Make step return for process.
	void step_return(sc_addr process);
/// @}


/// @name Methods for manage breakpoints
/// @{
public:
	/// Check if scp-operator @p op is breakpoint.
	static bool is_breakpoint(sc_addr op);

	/**
	 * @brief Устанавливает ТО на scp-оператор @p op scp-программы @p program.
	 * @return знак установленной ТО.
	 */
	sc_addr set_breakpoint(sc_addr program, sc_addr op);
	
	/// Set breakpoint in @p program on line with number.
	sc_addr set_breakpoint(sc_addr program, size_t line);

	/// Remove breakpoint from operator @p op of scp-program @p program.
	void unset_breakpoint(sc_addr program, sc_addr op);

	/// Set all input branches to next operator and erase breakpoint.
	void remove_breakpoint(sc_addr brk);

private:
	/// Find for @p line nearest operator in @p program with line number greater or equal.
	static sc_addr find_op_for_line(sc_addr program, size_t line);
/// @}


/// @name Methods for manage program copies and originals.
/// @{
public:
	/**
	 * @brief Создание копии scp-программы @p program.
	 * @return копию scp-программы.
	 */
	sc_addr copy_program(sc_addr program);

	/**
	 * @brief Получение копии scp-программы @p program.
	 */
	sc_addr get_program_copy(sc_addr program);

	/**
	 * @brief Получение оригинала копии scp-программы @p programCopy.
	 */
	sc_addr get_program_original(sc_addr programCopy);

	/**
	 * @brief Получение оригинала копии scp-оператора @p opCopy, который принадлежит скопированной scp-программе @p programCopy.
	 */
	sc_addr get_op_original(sc_addr programCopy, sc_addr opCopy);

	/**
	 * @brief Получение копии scp-оператора @p op, который принадлежит скопированной scp-программе @p programCopy.
	 */
	sc_addr get_op_copy(sc_addr programCopy, sc_addr op);
/// @}


private:
	static void log_event_or_command(sc_addr prm1, sc_addr prm2, sc_addr prm3);


/// @name Methods for raising events to reciever.
/// @{
private:
	sc_retval raise_event(sc_addr event, sc_addr prm1=0, sc_addr prm2=0);

	sc_retval raise_breakpoint_event(sc_addr process, sc_addr op);

	sc_retval raise_step_event(sc_addr process, sc_addr op);

	sc_retval raise_breakpoint_changed(sc_addr program, sc_addr brk, size_t line);
/// @}


/// @name Methods for handling commands from reciever.
/// @{
private:
	/// Register activity with @p sign as reciever for all debugging events.
	sc_retval handle_register(sc_addr sign, sc_addr);

	sc_retval handle_set_breakpoint(sc_addr, sc_addr);

	sc_retval handle_unset_breakpoint(sc_addr, sc_addr);

	sc_retval handle_suspend(sc_addr process, sc_addr)
	{
		suspend(process);
		return RV_OK;
	}

	sc_retval handle_resume(sc_addr process, sc_addr)
	{
		resume(process);
		return RV_OK;
	}

	sc_retval handle_step_over(sc_addr process, sc_addr)
	{
		step_over(process);
		return RV_OK;
	}

	sc_retval handle_step_into(sc_addr process, sc_addr)
	{
		step_into(process);
		return RV_OK;
	}

	sc_retval handle_step_return(sc_addr process, sc_addr)
	{
		step_return(process);
		return RV_OK;
	}

	sc_retval handle_terminate(sc_addr, sc_addr);

	sc_retval handle_unregister(sc_addr, sc_addr);
/// @}


/// @name Methods for handling events from scp-process.
/// @{
private:
	sc_retval catch_breakpoint_event(sc_addr process, sc_addr breakpoint);

	sc_retval catch_step_before_event(sc_addr process, sc_addr op);

	sc_retval catch_step_after_event(sc_addr process, sc_addr op);

	sc_retval catch_call_event(sc_addr process, sc_addr op);

	sc_retval catch_return_event(sc_addr process, sc_addr op);

	sc_retval catch_error_event(sc_addr process, sc_addr op);

	sc_retval catch_death_event(sc_addr process, sc_addr unused);

	sc_retval catch_child_event(sc_addr process, sc_addr childProcess);
/// @}

private:
	sc_addr reciever;     ///< Debugging events reciever

	sc_addr debug_copies; ///< Set of SCP-program debug copies
	sc_addr breakpoints;  ///< Set of setted breakpoints
};

/// @}

#endif // __LIBPM_SCP_DEBUGGER_H_INCLUDED__
