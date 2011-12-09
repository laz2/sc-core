
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
 * @file sc_config.h
 * @brief Configuration of libsc.
 * @ingroup libsc
 */

#ifndef __LIBSC_SC_CONFIG_H_INCLUDED__
#define __LIBSC_SC_CONFIG_H_INCLUDED__

/// addtogroup libsc
/// @{

#ifndef LIBSC_API
	#ifdef WIN32
		#ifdef _LIB
			#define LIBSC_API
		#else
			#ifdef libsc_EXPORTS
				#define LIBSC_API __declspec(dllexport)
			#else
				#define LIBSC_API __declspec(dllimport)
			#endif
		#endif
	#else
		#define LIBSC_API
	#endif
#endif

#undef SC_ARCH
#define SC_ARCH i386

/**
 * This is not required to be actual page size.
 * Used in constraint system params checker
 * Must be a power of 2
 */
#define SC_ARCH_PAGESIZE 4096

/// @}

#endif // __LIBSC_SC_CONFIG_H_INCLUDED__
