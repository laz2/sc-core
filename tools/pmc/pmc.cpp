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
 * @file main.cpp
 * @brief Main file of start-pm utility.
 * @author Dmitry Lazurkin
 */

#include <libpm/pm.h>

#include <stdio.h>

#include <libsc/abort.h>
#include <libsc/string_util.h>
#include <libsc/sc_print_utils.h>
#include <libsc/sc_set.h>

#include <libsc/tgf_io.h>
#include <libpm/pm_sched.h>

#include <libpm/scp/scp_core.h>
#include <libpm/scp/scp_package.h>
#include <libpm/scp/scp_operator.h>
#include <libpm/scp/scp_operand.h>

#include <librgp/rgp_server.h>

#include <tclap/CmdLine.h>

#include <algorithm>
#include <functional>

#define __TOSTR2(a) #a
#define __TOSTR(a) __TOSTR2(a)
#define VERSION_STRING __TOSTR(PM_VER)

/// Do schedule SCP-processes until at least one not dead.
static void do_schedule_scp_processes()
{
	while (!sc_set::is_empty(system_session, SCP_PROCESS))
		pm_sched_do_step();
}

/// Run program with uri @p program_uri. Execute scheduler.
static void do_run_program(const sc_string &program_uri)
{
	sc_session *s = libsc_login();

	sc_string program_name = basename(program_uri);
	sc_string program_seg_uri = dirname(program_uri);

	sc_segment *program_seg = s->open_segment(program_seg_uri);
	if (!program_seg) {
		std::cerr << "Segment uri '" << program_seg_uri << "' not found.\n";
		exit(3);
	}

	sc_addr program = s->find_by_idtf(program_name, program_seg);
	if (!program) {
		std::cerr << "Program with name '" << program_name << "' not found in segment '" << program_seg_uri << "'.\n";
		exit(3);
	}

	sc_retval rv = scp_package_run_program(libsc_login(), program, 0, 0,0);
	if (rv) {
		std::cerr << "Unable to run program.\n";
		exit(3);
	}

	do_schedule_scp_processes();
}

/// Ascending comparator for sc_addr's. Uses it's idtfs.
class asc_addr_idtf_comp : public std::binary_function<sc_addr, sc_addr, bool>
{
public:
	bool operator() (const sc_addr &lhs, const sc_addr &rhs) const
	{
		const sc_string &lhs_idtf = system_session->get_idtf(lhs);
		const sc_string &rhs_idtf = system_session->get_idtf(rhs);
		return !lhs_idtf.compare(rhs_idtf);
	}
};

/**
 * @brief Run testcases from segment @p seg. Execute scheduler.
 * @note Testcase is just program thats idtf starts with 'test'.
 */
static void run_testsuite_segment(sc_segment *seg)
{
	sc_session *s = libsc_login();
	s->open_segment(seg->get_full_uri());

	// Collect all scp-programs from this segment that's name starting with "test"
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, SCP_PROGRAM, SC_A_CONST|SC_POS, SC_N_CONST), true);
	addr_list testcases;
	sc_for_each(it) {
		sc_addr program = it->value(2);
		if (program->seg == seg) {
			sc_string idtf = s->get_idtf(program);

			// std::string haven't startswith method. This bad solution for startswith.
			if (idtf.find("test") == 0)
				testcases.push_back(program);
		}
	}

	if (testcases.empty())
		return;

	std::cout << "Testsuite '" << seg->get_full_uri() << "'\n";

	testcases.sort(asc_addr_idtf_comp());

	// Run all testcases
	for (addr_list::iterator testcase_iter = testcases.begin();
		testcase_iter != testcases.end(); testcase_iter++) {
		sc_addr process = 0;
		
		sc_retval rv = scp_package_run_program(libsc_login(), *testcase_iter, 0, 0, &process);
		if (rv)
			std::cerr << "Unable to run test '" << get_full_uri(s, *testcase_iter) << "'.\n";
		
		std::cout << "\tTestcase '" << s->get_idtf(*testcase_iter) << "' : ";
		
		do_schedule_scp_processes();

		std::cout << std::endl;
	}

	s->close();
}

/// Run from tgf-file @p filename. Execute scheduler.
static void do_run_testsuites(const sc_string &testsuite_uri)
{
	sc_retval rv = system_session->_stat(testsuite_uri);
	if (rv == RV_THEN) {
		// testsuite_uri - segment uri
		run_testsuite_segment(system_session->open_segment(testsuite_uri));
	} else if (RV_IS_ELSE(rv)) {
		// testsuite_uri - directory uri
		sc_dir_iterator *it = system_session->search_dir(testsuite_uri);
		auto_sc_dir_iterator_ptr auto_it(it);
		for (; !it->is_over(); it->next()) {
			if (it->value() != "META") {
				if (testsuite_uri[testsuite_uri.size() - 1] != '/') {
					do_run_testsuites(testsuite_uri + "/" + it->value());
				} else {
					do_run_testsuites(testsuite_uri + it->value());
				}
			}
		}
	} else {
		std::cerr << "'" << testsuite_uri << "' isn't directory or segment URI" << std::endl;
		exit(3);
	}
}

/// Flush stdout and stderr at program exit.
static void sync_streams()
{
	// FIXME: Usage of std::cout/std::cerr.
	fflush(stdout);
	fflush(stderr);
}

#include <time.h>

TCLAP::ValueArg<std::string> arg_run_program("", "run-program", "Run program with specified URI", false, "", "URI of program id");
TCLAP::ValueArg<std::string> arg_run_testsuite("", "run-testsuite", "Run testsuite in specified segment", false, "", "URI of testsuite segment");
TCLAP::ValueArg<std::string> arg_fsrepo("", "fsrepo", "Place of binary repository", false, "", "Dir path");

TCLAP::SwitchArg arg_nofsrepo("", "no-fsrepo", "Not use fs_repo, only in memory", false);
TCLAP::SwitchArg arg_diag("d", "diagnostic", "Include more diagnostic in output");
TCLAP::SwitchArg arg_verbose("v", "verbose", "Switch on verbose output");
TCLAP::SwitchArg arg_stay("s", "stay", "Enter dedicated mode after programs termination");
TCLAP::SwitchArg arg_stop_on_error("", "stop-on-error", "Stop executing when interpreter encounters error");
TCLAP::SwitchArg arg_no_print_el("", "no-print-el", "Turn print_el off");
TCLAP::SwitchArg arg_with_debugger("", "with-debugger", "Enable SCP-debugger");
TCLAP::SwitchArg arg_with_rgp_server("", "with-rgp-server", "Enable RGP-server");
TCLAP::ValueArg<int> arg_rgp_control("", "rgp-control", "Set RGP server control port", false, 0, "port number");
TCLAP::ValueArg<int> arg_rgp_event("", "rgp-event", "Set RGP server event port", false, 0, "port number");
TCLAP::SwitchArg arg_save_at_exit("", "save-at-exit", "Save repository on FS at exit", false);
TCLAP::SwitchArg arg_allow_unresolved_links("", "allow-unresolved-links", "Allow unresolved links while segment loading", false);

int main(int argc, char **argv)
{
	setlocale(LC_ALL, ".1251");

	atexit(sync_streams);

	try {
		TCLAP::CmdLine cmd("Processing module with fs_repository", ' ', VERSION_STRING);

		cmd.add(arg_run_program);
		cmd.add(arg_run_testsuite);
		cmd.add(arg_fsrepo);

		cmd.add(arg_nofsrepo);
		cmd.add(arg_diag);
		cmd.add(arg_verbose);
		cmd.add(arg_stay);
		cmd.add(arg_stop_on_error);
		cmd.add(arg_no_print_el);
		cmd.add(arg_with_debugger);
		cmd.add(arg_with_rgp_server);
		cmd.add(arg_rgp_control);
		cmd.add(arg_rgp_event);
		cmd.add(arg_save_at_exit);
		cmd.add(arg_allow_unresolved_links);

		cmd.parse(argc, argv);

		bool no_fsrepo     = arg_nofsrepo.getValue();
		diag_output        = arg_diag.getValue();
		verb_output        = arg_verbose.getValue();
		bool stay          = arg_stay.getValue();
		stop_on_error      = arg_stop_on_error.getValue();
		no_print_el        = arg_no_print_el.getValue();
		bool with_debugger = arg_with_debugger.getValue();
		bool with_rgp      = arg_with_rgp_server.getValue();
		//allow_unresoled_links = arg_allow_unresolved_links.getValue();

		print_el_max_power = 0;
		print_el_variant = 0;

		if (arg_fsrepo.isSet())
			fs_repo_loc = arg_fsrepo.getValue().c_str();

		verb_output &= !no_print_el;

		do_init(false, !no_fsrepo, "", with_debugger);
		if (with_rgp) {
			if (arg_rgp_control.isSet()) {
				init_rgp_server(system_session, arg_rgp_control.getValue());
			} else {
				init_rgp_server(system_session);
			}
		}

		if (arg_run_program.isSet()) {
			do_run_program(arg_run_program.getValue().c_str());
			if (stay)
				pm_sched_main();
		} else if (arg_run_testsuite.isSet()) {
			do_run_testsuites(arg_run_testsuite.getValue().c_str());
		} else {
			pm_startup();
			do_schedule_scp_processes();
			if (stay)
				pm_sched_main();
		}

		do_deinit();

		if (arg_save_at_exit.isSet())
			libsc_deinit();
	} catch (TCLAP::ArgException &e) {
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	return 0;
}
