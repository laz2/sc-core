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
 * @file scps_lexer.hpp
 * @ingroup SCC
 * @brief Common functional of SCPs language lexer.
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCPS_LEXER_HPP_INCLUDED__
#define __SCC_SCPS_LEXER_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include <antlr3.h>

#include "config.h"

namespace scc
{
	LIBSCC_API pANTLR3_STRING process_string_token(pANTLR3_STRING tok);

	LIBSCC_API pANTLR3_STRING process_string_base64_token(pANTLR3_STRING tok);
} // namespace scps

/// @}

#endif // __SCC_SCPS_LEXER_HPP_INCLUDED__
