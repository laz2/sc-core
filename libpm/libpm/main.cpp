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

#include "libpm_precompiled_p.h"

#include "scp/scp_core.h"
#include "scp/scp_operator.h"
#include "scp/scp_package.h"
#include "scp/scp_var.h"
#include "scp/scp_debugger.h"

#define USE_NSM

#ifdef USE_NSM
void  nsm_pre_init();
void  nsm_init();
#endif

#include <iostream>

sc_segment *do_read_tgf(const sc_string &filename)
{
	FILE *input = fopen(filename.c_str(), "rb");
	if (!input) {
		std::cerr << "Cannot open input file!\n";
		exit(2);
	}

	int fd = fileno(input);
	tgf_stream_in *tgf_in = tgf_stream_in_new();
	if (!tgf_in) {
		goto __tgf_init_error;
	}

	tgf_stream_in_fd(tgf_in, fd);
	if (tgf_stream_in_start(tgf_in, 0)) {
__tgf_init_error:
		std::cerr << "tgf init error!\n";
		exit(2);
	}

	sc_segment *seg = create_unique_segment(system_session,"/tmp/program");
	if (!seg) {
		SC_ABORT();
	}

	if (read_tgf_to(tgf_in, system_session, seg)) {
		std::cerr << "Cannot read tgf(" << filename << ")!\n";
		SC_ABORT();
	}

	tgf_stream_in_destroy(tgf_in);
	fclose(input);

	return seg;
}

sc_session *get_session()
{
	return system_session;
}

static
void init_dirhier(sc_session *system)
{
	system->mkdir("/tmp");
	system->mkdir("/proc");
	system->mkdir("/proc/scp");
	system->mkdir("/proc/agents");
}

void do_init(bool service_mode, bool use_fs_repo, const sc_string &fs_repo_location, bool with_debugger)
{
	sc_session *system;

	if (!fs_repo_location.empty()) {
		fs_repo_loc = fs_repo_location;
	}
	system = libsc_init(use_fs_repo);

	init_dirhier(system);
	pm_keynodes_init(system);
	pm_sched_init(system, true);
	scp_vars_init(system);
	scp_core_init(system);
	scp_operators_init(system);

	scp_package_init(system);

	if (with_debugger)
		scp_debugger::init(system);

	if (!service_mode) {
#ifdef USE_NSM
		nsm_pre_init();
#endif

#ifdef USE_NSM
		nsm_init();
#endif
	}

	init_sc_operations(system);

	scl::init_productions_engine(system);

	init_ext_program(system);
}

void do_deinit()
{
	scp_debugger::deinit();
}

void pm_startup()
{
	system_session->open_segment("/proc/keynode");
	system_session->open_segment("/proc/numattrs");
	system_session->open_segment("/startup");

	sc_addr startup = sc_tup::at(system_session, CURRENT_MODULE, STARTUP_);
	if (!startup)
		return;

	sc_session *s = libsc_login();

	sc_iterator *it = system_session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, startup, 0, 0),true);
	sc_for_each (it) {
		sc_addr prg = it->value(2);
		if (scp_package_run_program(s, prg, 0, 0,0))
			dprintf("SCP program %s startup failed\n",system_session->get_idtf(prg).c_str());
	}

	s->close();
}

void do_dedicated(bool shed, bool detach)
{
	pm_startup();
	if (shed) {
		std::cout << "Switching to dedicated mode\n";
		pm_sched_main();
	}
}

void do_step()
{
	pm_sched_do_step();
}
