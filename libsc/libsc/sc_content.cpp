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
 * @file sc_content.cpp
 * @brief Implementation of content in sc-memory.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

/// @addtogroup libsc
/// @{

#include "precompiled_p.h"

bool Content::operator==(const Content &c) const
{
	if ((c.c_type != TCINT && c.c_type != TCREAL) || (c_type != TCINT && c_type != TCREAL)) {
		if (c.c_type != c_type) {
			return false;
		} 
	}

	switch (c_type) {
	case TCSTRING:
	case TCDATA:
	case TCLAZY:
		if (cont.d.size != c.cont.d.size) {
			return false;
		} 
		return !memcmp(c.cont.d.ptr, cont.d.ptr, cont.d.size);
	case TCEMPTY:
		return true;
	default:
		break;
	}
	double v1, v2;
	v1 = (c_type == TCREAL) ? cont.r : cont.i;
	v2 = (c.c_type == TCREAL) ? c.cont.r : c.cont.i;
	return v1 == v2;
}

bool Content::operator>(const Content &c) const
{
	if ((c.c_type != TCINT && c.c_type != TCREAL) || (c_type != TCINT && c_type != TCREAL)) {
		if (c.c_type != c_type) {
			return c_type > c.c_type;
		} 
	}

	switch (c_type) {
	case TCSTRING:
	case TCDATA:
		if (cont.d.size < c.cont.d.size) {
			return memcmp(cont.d.ptr, c.cont.d.ptr, cont.d.size) > 0;
		} else {
			return memcmp(cont.d.ptr, c.cont.d.ptr, c.cont.d.size) >= 0;
		} 
	case TCEMPTY:
		return false;
	default:
		break;
	}
	double v1, v2;
	v1 = (c_type == TCREAL) ? cont.r : cont.i;
	v2 = (c.c_type == TCREAL) ? c.cont.r : c.cont.i;
	return v1 > v2;
}

bool Content::operator<(const Content &c) const
{
	if ((c.c_type != TCINT && c.c_type != TCREAL) || (c_type != TCINT && c_type != TCREAL)) {
		if (c.c_type != c_type) {
			return c_type < c.c_type;
		}
	} 

	switch (c_type) {
	case TCSTRING:
	case TCDATA:
		if (cont.d.size < c.cont.d.size) {
			return memcmp(cont.d.ptr, c.cont.d.ptr, cont.d.size) < 0;
		} else {
			return memcmp(cont.d.ptr, c.cont.d.ptr, c.cont.d.size) <= 0;
		} 
	case TCEMPTY:
		return false;
	default:
		break;
	}

	double v1, v2;
	v1 = (c_type == TCREAL) ? cont.r : cont.i;
	v2 = (c.c_type == TCREAL) ? c.cont.r : c.cont.i;
	return v1 < v2;
}

int Content::copy_cont(const Cont &c, TCont t)
{
	char *ptr;
	if (t == TCDATA || t == TCSTRING || t == TCLAZY) {
		ptr = new char[c.d.size];
		if (ptr == 0) {
			return  - 1;
		} 
		eraseCont();
		memcpy(ptr, c.d.ptr, c.d.size);
		cont.d.ptr = ptr;
		cont.d.size = c.d.size;
	} else {
		cont = c;
	}
	set_type(t);
	return 0;
}

std::ostream& operator<<(std::ostream &out, const Content &content) {
	out << "<";
	const Cont &cont = static_cast<Cont>(content);
	switch (content.type()) {
		case TCSTRING:
			out << "STRING, " << cont.d.ptr;
			break;
		case TCINT:
			out << "INT, " << cont.i;
			break;
		case TCREAL:
			out << "REAL, " << cont.r;
			break;
		case TCEMPTY:
			out << "EMPTY";
			break;
		case TCDATA:
			out << "DATA, size=" << cont.d.size;
			break;
	}
	out << ">";
	return out;
}

/// @}
