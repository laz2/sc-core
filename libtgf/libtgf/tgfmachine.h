
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


/*
 *  SCPx processing module project.
 *
 *  tgf_machine_32bit.h - machine dependent definitions
 *
 *  This file assumes generic 32bit machine
 *
 *  Created at Wed 09 May 2001 01:03:03 AM EEST by ALK
 *
 */
#ifndef __TGF_MACHINE_H__
#define __TGF_MACHINE_H__

typedef unsigned char tgf_uint8;
typedef unsigned short tgf_uint16;
typedef unsigned int tgf_uint32;
typedef signed char tgf_sint8;
typedef signed short tgf_sint16;
typedef signed int tgf_sint32;
typedef unsigned char tgf_uchar;

#if TGF_BYTE_ORDER_BE
#define TGF_HOST_ENDIANNESS 0
#else
#define TGF_HOST_ENDIANNESS 1
#endif

#define	TGF_P2I(ptr) ((int)(long)(ptr))
#define TGF_I2P(i) ((void *)(long)(i))

#endif /* __TGF_MACHINE_H__ */
