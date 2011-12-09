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
 * @file scps_lexer.cpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#include "precompiled_p.h"

#include "antlr3_scc_utils.h"
#include "scps_lexer.h"

#include <b64/decode.h>

pANTLR3_STRING scc::process_string_token(pANTLR3_STRING tok)
{
	int l = tok->len - 4;

	pANTLR3_UINT8 out = tok->chars;
	pANTLR3_UINT8 in = tok->chars + 2;

	while (l-- > 0) {
		if (*in == '\n') {
			++in;
			if (*in == '\r')
				*out++ = *in++;
			continue;
		}

		if (*in == '\r') {
			*out++ = *in++;
			if (*in == '\n')
				*out++ = *in++;
			continue;
		}

		if (*in == '\t') {
			*out++ = *in++;
			continue;
		}

		if (*in != '\\') {
			*out++ = *in++;
			continue;
		}

		++in;
		--l;

		if (l < 0) {
			*in++ = '\\';
			break;
		}

		switch (*in) {
		case 'n':
			*out++ = '\n';
			break;
		case 'r':
			*out++ = '\r';
			break;
		case 't':
			*out++ = '\t';
			break;
		default:
			*out++ = *in;
		}

		++in;
	}

	*out = 0;
	tok->len = out - tok->chars;

	return tok;
}

pANTLR3_STRING scc::process_string_base64_token(pANTLR3_STRING tok)
{
	int l = tok->len - 7;

	pANTLR3_UINT8 out = tok->chars;
	pANTLR3_UINT8 in = tok->chars + 5;

	// Skip \n, \r, \t characters
	while (l-- > 0) {
		if (isspace(*in))
			++in;
		else
			*out++ = *in++;
	}

	*out = 0;
	tok->len = out - tok->chars;

	// Decode base64
	base64::decoder b64_decoder;
	pANTLR3_STRING decoded = tok->factory->newSize(tok->factory, 3 * tok->len / 4);
	decoded->len = b64_decoder.decode(to_const_char(tok), tok->len, reinterpret_cast<char *>(decoded->chars));
	decoded->chars[decoded->len] = 0;

	tok->factory->destroy(tok->factory, tok);

	return decoded;
}