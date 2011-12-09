
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

static int atoms_inited;

sc_segment *sc_atoms_segment;

static sc_atom *special_init;

void sc_atoms_init()
{
	sc_atom *atom = special_init;
	sc_atoms_segment = system_session->create_segment("/proc/keynode");
	atoms_inited = 1;
	while (atom) {
		atom->value = sc_atom_intern(atom->name);
		atom = atom->next;
	}
}

sc_addr sc_atom_intern(const char *name)
{
	sc_addr rv;
	rv = system_session->find_by_idtf(name, sc_atoms_segment);
	if (!rv) {
		rv = system_session->create_el(sc_atoms_segment, SC_N_CONST);
		assert(rv);
		if (system_session->set_idtf(rv, name))
			SC_ABORT();
	}
	return rv;
}

void sc_atom_intern_special(sc_atom *atom)
{
	if (atoms_inited) {
		atom->value = sc_atom_intern(atom->name);
		return;
	}
	atom->next = special_init;
	special_init = atom;
}

