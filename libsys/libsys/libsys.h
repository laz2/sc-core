
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

#ifndef __LIBSYS_H__INCLUDED__
#define __LIBSYS_H__INCLUDED__

#ifndef LIBSYS_API
#ifdef WIN32
#	ifdef _LIB
#		define LIBSYS_API
#	else 
#		ifdef libsys_EXPORTS
#			define LIBSYS_API __declspec(dllexport)
#		else
#			define LIBSYS_API __declspec(dllimport)
#		endif
#	endif
#else
#	define LIBSYS_API
#endif
#endif

#ifdef WIN32
#include <tchar.h>
#include <string>
#	pragma warning (disable : 4996)
#endif

#include <string.h>

#endif // #ifndef __LIBSYS_H__INCLUDED__
