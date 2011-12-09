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

namespace scl
{
const char* scl_keynodes_uris[] = {
	"/proc/scl/keynode/impl*",
	"/proc/scl/keynode/eq*",
	"/proc/scl/keynode/and*",
	"/proc/scl/keynode/or*",
	"/proc/scl/keynode/not*",
	"/proc/scl/keynode/if_",
	"/proc/scl/keynode/then_",
	"/proc/scl/keynode/atom",
	"/proc/scl/keynode/production_engine"
};

sc_segment *scl_keynodes_segment;

const int scl_keynodes_count = sizeof(scl_keynodes_uris) / sizeof(const char*);

sc_addr scl_keynodes[scl_keynodes_count];

const sc_addr &IMPL_REL = scl_keynodes[0];
const sc_addr &EQ_REL = scl_keynodes[1];
const sc_addr &AND_REL = scl_keynodes[2];
const sc_addr &OR_REL = scl_keynodes[3];
const sc_addr &NOT_REL = scl_keynodes[4];
const sc_addr &IMPL_IF_ = scl_keynodes[5];
const sc_addr &IMPL_THEN_ = scl_keynodes[6];
const sc_addr &ATOM = scl_keynodes[7];
const sc_addr &PRODUCTION_ENGINE = scl_keynodes[8];
}

/**@}*/
