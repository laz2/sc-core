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
 * @file scp_keynodes.h
 * @brief Keynodes of SCP language and interpreter.
 * @ingroup libpm
 */

#ifndef __SCP_KEYNODES_H_INCLUDED__
#define __SCP_KEYNODES_H_INCLUDED__

#include <libpm/pm.h>

namespace scp
{
extern LIBPM_API const char *keynodes_uris[];
extern LIBPM_API const int keynodes_count;

extern LIBPM_API sc_segment *keynodes_segment;

extern LIBPM_API sc_addr keynodes[];

extern LIBPM_API const sc_addr &processStack_;
}

#endif // __SCP_KEYNODES_H_INCLUDED__

/**@}*/
