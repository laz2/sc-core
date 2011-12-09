
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


#ifndef LIBSC_ABORT_H
#define LIBSC_ABORT_H
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
#	ifdef _LIB // стандартный макрос для статической библиотеки 
#		define __LIBSC_ABORT_API
#	else // _USRDLL - стандартный макрос для динамической библиотеки
#		ifdef libsc_EXPORTS
#			define __LIBSC_ABORT_API __declspec(dllexport)
#		else
#			define __LIBSC_ABORT_API __declspec(dllimport)
#		endif
#	endif
#else
#	define __LIBSC_ABORT_API
#endif


__LIBSC_ABORT_API
extern int abort_on_critical;
__LIBSC_ABORT_API
void __libsc_abort(const char *msg, const char *file, int line);
__LIBSC_ABORT_API
void __libsc_diag_message(const char *msg, const char *file, int line);
__LIBSC_ABORT_API
void __libsc_critical_message(const char *msg, const char *file, int line);

// noreturn here is to save stack frame
//  and avoid gcc warning in cases where
// abort calling function does not return value
__LIBSC_ABORT_API
#ifdef __GNUC__
__attribute__((noreturn))
#elif defined(_MSC_VER)
__declspec(noreturn)
#endif
void __libsc_noreturn(void);

#define SC_ABORT() do {__libsc_abort(0,__FILE__,__LINE__); __libsc_noreturn(); } while(0)

#define SC_ABORT_MESSAGE(msg) do {__libsc_abort((msg),__FILE__,__LINE__); __libsc_noreturn(); } while(0)

#define SC_CRITICAL_RETURN(msg,rv) \
	do { \
		__libsc_critical_message(msg,__FILE__,__LINE__); \
		return (rv); \
	} while(0)

#define SC_CRITICAL_MESSAGE(msg) __libsc_critical_message(msg,__FILE__,__LINE__)


#endif
