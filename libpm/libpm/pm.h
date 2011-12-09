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
 * @defgroup libpm libpm - processing module modeling library
 * @{
 */

/**
 * @file pm.h
 * @brief Main interface of Processor Module (PM).
 */

#ifndef __LIBPM_PM_H_INCLUDED__
#define __LIBPM_PM_H_INCLUDED__

#include <libsc/libsc.h>
#include <libsc/pm_keynodes.h>
#include <libsc/sc_change_utils.h>

#include "config.h"

/// Segment URI which contains standard keynodes
const sc_string KEYNODE_SEGURI = "/proc/keynode";

/// Read sc-constructions from tgf-file @p filename to tmp-segment.
LIBPM_API sc_segment *do_read_tgf(const sc_string &filename);

/// Initialize libpm enviroment.
LIBPM_API void do_init(bool service_mode, bool use_fs_repo, const sc_string &fs_repo_location = "", bool with_debugger = false);

/// Deinitialize libpm enviroment.
LIBPM_API void do_deinit();

/// Make programs auto-run. If @p sched true launch scheduler loop.
LIBPM_API void do_dedicated(bool shed = true, bool detach = false);

/// Return system session.
LIBPM_API sc_session *get_session();

/// Make one step of scheduler.
LIBPM_API void do_step();

/// Create process for entries in CURRENT_MODULE with attr STARTUP_.
LIBPM_API void pm_startup();

#endif // __LIBPM_PM_H_INCLUDED__

/**@}*/
