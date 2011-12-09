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
 * @addtogroup libpm
 * @{
 */

#ifndef __SCL_KEYNODES_H__
#define __SCL_KEYNODES_H__

#include <libpm/pm.h>

namespace scl
{
extern LIBPM_API const char *scl_keynodes_uris[];
extern LIBPM_API const int scl_keynodes_count;

extern sc_segment *scl_keynodes_segment;

extern LIBPM_API sc_addr scl_keynodes[];

extern const sc_addr &IMPL_REL;
extern const sc_addr &EQ_REL;
extern const sc_addr &AND_REL;
extern const sc_addr &OR_REL;
extern const sc_addr &NOT_REL;
extern const sc_addr &IMPL_IF_;
extern const sc_addr &IMPL_THEN_;
extern const sc_addr &ATOM;
extern const sc_addr &PRODUCTION_ENGINE;
}

#endif // __SCL_KEYNODES_H__

/**@}*/
