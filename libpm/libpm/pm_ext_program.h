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

/**
 * @file pm_ext_program.h
 * @ingroup libpm
 * @author Dmitry Lazurkin
 */

#ifndef __LIBPM_EXT_PROGRAM_INCLUDED_H__
#define __LIBPM_EXT_PROGRAM_INCLUDED_H__

/// @addtogroup libpm
/// @{

#include "config.h"

LIBPM_API sc_retval init_ext_program(sc_session *s);

/// Call external program @p prg with parameters @p prms and session @p s.
LIBPM_API sc_retval call_ext_program(sc_session *s, sc_addr prg, addr_vector &prms);

class LIBPM_API ext_program_keynodes
{
public:
	static const char *keynodes_uris[];

	static sc_segment *keynodes_segment;

	static const int keynodes_count;

	static sc_addr keynodes[];

	static const sc_addr &dynamicModule;
	static const sc_addr &externalProgram;
};

/// @}

#endif // __LIBPM_EXT_PROGRAM_INCLUDED_H__
