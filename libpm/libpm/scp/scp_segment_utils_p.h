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

/**
 * @brief Utils for working with opened segments set of SCP-process.
 * @file scp_segment_utils.h
 * @author Dmitry Lazurkin
 */

#ifndef __LIBPM_SCP_SEGMENT_UTILS_INCLUDED__
#define __LIBPM_SCP_SEGMENT_UTILS_INCLUDED__

#include <libpm/pm.h>

/// Opens segment by @p sign for SCP-process.
LIBPM_API sc_retval scp_open_segment(scp_process_info *info, sc_addr sign);

/// Opens segment by uri from content of @p ptr for SCP-process and
/// sets opened segment sign as value of SCP-variable @p var.
LIBPM_API sc_retval scp_open_segment_uri(scp_process_info *info, sc_addr ptr, sc_addr var);

/// Creates segment by uri from content of @p ptr for SCP-process and
/// sets created segment sign as value of SCP-variable @p var.
/// @note Creates all non-existence directories in URI.
LIBPM_API sc_retval scp_create_segment(scp_process_info *info, sc_addr ptr, sc_addr var);

/// Creates unique segment for SCP-process and sets created segment sign as value of SCP-variable @p var.
LIBPM_API sc_retval scp_create_unique_segment(scp_process_info *info, sc_addr var);

/// Closes segment by @p sign for SCP-process.
LIBPM_API sc_retval scp_close_segment(scp_process_info *info, sc_addr sign);

/**
 * @brief Opens segment by sign @p s and all referenced segments. Repeats this for all segments which opens.
 * @param info info of SCP-process
 * @param s sign of first segment for opening
 * @param[out] S  set of all opened segments signs
 * @param maxlev max recursion level for opening
 * @return SC-return value
 */
LIBPM_API sc_retval scp_span_segments(scp_process_info *info, sc_addr s, sc_addr S, int maxlev);

/// Opens directory by @p sign (opens directory META-segment) for SCP-process
/// and sets opened META-segment sign as value of SCP-variable @p var.
LIBPM_API sc_retval scp_opendir(scp_process_info *info, sc_addr sign, sc_addr var);

/// Opens directory by uri from content of @p ptr (opens directory META-segment) for SCP-process and
/// sets opened META-segment sign as value of SCP-variable @p var.
LIBPM_API sc_retval scp_opendir_uri(scp_process_info *info, sc_addr ptr, sc_addr var);

#endif // __LIBPM_SCP_SEGMENT_UTILS_INCLUDED__

/**@}*/
