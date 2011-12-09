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
 * @addtogroup librgp
 * @{
 */

/**
 * @file io_p.h
 * @brief Some functions for IO, which reads and writes 
 * @author Dmitry Lazurkin
 */

#ifndef __LIBRGP_IO_H_INCLUDED__
#define __LIBRGP_IO_H_INCLUDED__

#include "rgp.h"

namespace RGP {

template <class T, class InIt>
inline T read_int(InIt& start) {
	T ret = 0;
	for (int i = 0; i < (int)sizeof(T); ++i) {
		ret <<= 8;
		ret |= static_cast<unsigned char>(*start);
		++start;
	}
	return ret;
}

template <class T, class OutIt>
inline void write_int(T val, OutIt& start) {
	for (int i = (int)sizeof(T)-1; i >= 0; --i) {
		*start = static_cast<unsigned char>((val >> (i * 8)) & 0xff);
		++start;
	}
}

template <class InIt>
uint32_t read_uint32(InIt& start) {
	return read_int<uint32_t>(start);
}

template <class InIt>
int32_t read_int32(InIt& start) {
	return read_int<int32_t>(start);
}

template <class InIt>
uint16_t read_uint16(InIt& start) {
	return read_int<uint16_t>(start);
}

template <class InIt>
uint16_t read_int16(InIt& start) {
	return read_int<int16_t>(start);
}

template <class InIt>
uint8_t read_uint8(InIt& start) {
	return read_int<uint8_t>(start);
}

template <class InIt>
int8_t read_int8(InIt& start) {
	return read_int<uint8_t>(start);
}

template <class OutIt>
void write_uint32(uint32_t val, OutIt& start) {
	write_int(val, start);
}

template <class OutIt>
void write_int32(int32_t val, OutIt& start) {
	write_int(val, start);
}

template <class OutIt>
void write_uint16(uint16_t val, OutIt& start) {
	write_int(val, start);
}

template <class OutIt>
void write_int16(int16_t val, OutIt& start) {
	write_int(val, start);
}

template <class OutIt>
void write_uint8(uint8_t val, OutIt& start) {
	write_int(val, start);
}

template <class OutIt>
void write_int8(boost::int8_t val, OutIt& start) {
	write_int(val, start);
}

} // namespace RGP

#endif // __LIBRGP_IO_H_INCLUDED__

/**@}*/
