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
 * @file antlr3_scc_utils.cpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#include "precompiled_p.h"

#include "antlr3_scc_utils.h"

pANTLR3_INPUT_STREAM open_file(const fs::path &p)
{
	return antlr3AsciiFileStreamNew(to_antlr3_uint8(p.string().c_str()));
}

char* antlr3_strdup(pANTLR3_STRING str)
{
	char *dup = new char[str->size];
	strcpy(dup, reinterpret_cast<char *>(str->chars));
	return dup;
}

void antlr3_skip_last_char(pANTLR3_STRING str)
{
	SC_ASSERT(str);
	if (str->chars) {
		str->chars[str->len - 1] = 0;
		--str->len;
	}
}

pANTLR3_STRING decode_base64(const pANTLR3_STRING str)
{
	return const_cast<pANTLR3_STRING>(str);
}
