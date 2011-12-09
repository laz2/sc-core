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
 * @file antlr3_scc_utils.hpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_ANTLR3_SCC_UTILS_HPP_INCLUDED__
#define __SCC_ANTLR3_SCC_UTILS_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include <antlr3.h>

#include <boost/filesystem.hpp>

#include "config.h"

namespace fs=boost::filesystem;

inline
const char *to_const_char(pANTLR3_STRING str)
{
	return reinterpret_cast<const char *>(str->chars);
}

inline
pANTLR3_UINT8 to_antlr3_uint8(const char * str)
{
	return reinterpret_cast<pANTLR3_UINT8>(const_cast<char *>(str));
}

LIBSCC_API pANTLR3_INPUT_STREAM open_file(const fs::path &p);

LIBSCC_API char *antlr3_strdup(pANTLR3_STRING str);

LIBSCC_API pANTLR3_STRING decode_base64(const pANTLR3_STRING str);

LIBSCC_API void antlr3_skip_last_char(pANTLR3_STRING str);

#define SCC_TRY try

#define SCC_CATCH_POS(pos) \
	catch (scc::lang_error &ex) { \
		scc::cc->report_error(ex.what(), pos); \
	}

/// @}

#endif // __SCC_ANTLR3_SCC_UTILS_HPP_INCLUDED__
