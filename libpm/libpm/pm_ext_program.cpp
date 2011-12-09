/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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

#include <boost/extension/shared_library.hpp>

#include "pm_ext_program.h"

namespace be=boost::extensions;

/// External program signature.
typedef sc_retval (*pm_ext_program)(sc_session *, addr_vector&);

sc_retval init_ext_program(sc_session *s)
{
	ext_program_keynodes::keynodes_segment = create_segment_full_path(s, "/proc/ext_program/keynode");
	create_keynodes(s, ext_program_keynodes::keynodes_count,
		ext_program_keynodes::keynodes_uris, ext_program_keynodes::keynodes);
	return RV_OK;
}

/// Load external program implementation from dynamic module @p dymod_sign.
static
pm_ext_program load_ext_program(sc_addr dynmod_sign, sc_addr prg)
{
	if (diag_output) {
		std::cout << "[PM::EXT_PRG] [INFO] Loading external program " << SC_QURI(prg) << 
			" for dynamic module " << SC_QURI(dynmod_sign) << std::endl;
	}

	be::shared_library dynmod(system_session->get_idtf(dynmod_sign) + ".dll");
	if (dynmod.open()) {
		pm_ext_program prg_ptr = static_cast<pm_ext_program>(
			dynmod.get<sc_retval, sc_session*, addr_vector&>(system_session->get_idtf(prg)));

		if (prg_ptr) {
			system_session->set_content(prg, Content::integer(reinterpret_cast<cint>(prg_ptr)));
		} else {
			std::cerr << "[PM::EXT_PRG] [ERR] Not found external program " << SC_QURI(prg) << 
				" for dynamic module " << SC_QURI(dynmod_sign) << std::endl;
		}

		return prg_ptr;
	} else {
		std::cerr << "[PM::EXT_PRG] [ERR] Not found dynamic module " << SC_QURI(dynmod_sign) << std::endl;
		return NULL;
	}
}

/// Load external program implementation from corresponding dynamic module.
static
pm_ext_program load_ext_program(sc_addr prg)
{
	if (sc_set::is_in(system_session, prg, ext_program_keynodes::externalProgram)) {

		sc_addr dynmod_sign = 0;
		if (search_3l2_f_cpa_cna_cpa_f(system_session, ext_program_keynodes::dynamicModule,
				0, &dynmod_sign, 0, prg) == RV_OK) {
			return load_ext_program(dynmod_sign, prg);
		} else {
			std::cerr << "[PM::EXT_PRG] [ERR] Not found dynamic module which contains external program "
				<< SC_QURI(prg) << std::endl;
		}
	} else {
		std::cerr << "[PM::EXT_PRG] [ERR] " << SC_QURI(prg) << 
			" isn't external program." << std::endl;
	}

	return NULL;
}

/// Return external program pointer for program sign.
/// @note Expect @p prg is in @c externalProgram.
static inline
pm_ext_program get_ext_program(sc_addr prg)
{
	const Content *c = system_session->get_content_const(prg);
	if (c->type() == TCINT) {
		return reinterpret_cast<pm_ext_program>(Cont(*c).i);
	} else {
		return load_ext_program(prg);
	}
}

sc_retval call_ext_program(sc_session *s, sc_addr prg, addr_vector &prms)
{
	pm_ext_program ext_prg = get_ext_program(prg);

	if (prg) {
		return ext_prg(s, prms);
	} else {
		return RV_ERR_GEN;
	}
}

//
// Keynodes of external programs language.
//

const char* ext_program_keynodes::keynodes_uris[] = {
	"/proc/ext_program/keynode/dynamicModule",
	"/proc/ext_program/keynode/externalProgram"
};

sc_segment *ext_program_keynodes::keynodes_segment;

const int ext_program_keynodes::keynodes_count = sizeof(ext_program_keynodes::keynodes_uris) / sizeof(const char*);

sc_addr ext_program_keynodes::keynodes[ext_program_keynodes::keynodes_count];

const sc_addr &ext_program_keynodes::dynamicModule   = ext_program_keynodes::keynodes[0];
const sc_addr &ext_program_keynodes::externalProgram = ext_program_keynodes::keynodes[1];
