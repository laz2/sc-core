
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



#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <sstream>

#define FATAL_ERROR(msg) \
	{ \
		std::stringstream _buf; \
		_buf << msg; \
		fprintf(stderr, "Fatal error in file '%s' at line %d: %s\n", __FILE__, __LINE__, _buf.str().c_str()); \
		abort(); \
	}

#define ASSERT_FATAL_ERROR(exp, msg) \
	{ \
		if (!(exp)) { \
			FATAL_ERROR(msg); \
		} \
	}

#define LOG_ERROR(msg) \
	{ \
		std::stringstream _buf; \
		_buf << msg; \
		fprintf(stderr, "Error in file '%s' at line %d: %s\n", __FILE__, __LINE__, _buf.str().c_str()); \
	}

#define TRACE_ERROR(msg) LOG_ERROR(msg)

#define LOG_WARN(msg) \
	{ \
		std::stringstream _buf; \
		_buf << msg; \
		fprintf(stderr, "Warning in file '%s' at line %d: %s\n", __FILE__, __LINE__, _buf.str().c_str()); \
	}

#define LOG_DEBUG(msg) \
	{ \
		std::stringstream _buf; \
		_buf << msg; \
		fprintf(stderr, "Debug message in file '%s' at line %d: %s\n", __FILE__, __LINE__, _buf.str().c_str()); \
	}

#endif // __LOGGING_H__
