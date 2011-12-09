
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

#include "precompiled_p.h"

static sc_string default_generator();

static uid_generator_func uid_generator = default_generator;

void LIBSC_API set_uid_generator(uid_generator_func f)
{
	uid_generator = f;
}

sc_string generate_uid()
{
	return uid_generator();
}

#ifdef WIN32
//#include <rpcdce.h>
#include <rpc.h>

static
sc_string default_generator()
{
	// base on code from CU
	std::string _buffer;
	unsigned char *temp;
	UUID uid;
	UuidCreate(&uid);
	UuidToString(&uid, &temp);
	_buffer = (char *) temp;
	RpcStringFree(&temp);
	return _buffer;
}
#elif defined(USE_LIBUUID)
#include <uuid/uuid.h>
static
sc_string default_generator()
{
	uuid_t uuid;
	char buf[37];
	uuid_generate(uuid);
	uuid_unparse(uuid,buf);
	return sc_string(buf);
}
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <stdio.h>

static
sc_string default_generator()
{
	struct timeb tstruct;
	ftime(&tstruct);
	static char buf[256];
	sprintf(buf,"%lu_%u_%u_%u",tstruct.time,tstruct.millitm,tstruct.timezone,
				tstruct.dstflag);
	return std::string(buf);
}
#endif
