
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

#ifdef WIN32
#	include <windows.h>
#endif

int abort_on_critical=1;

static
void message(const char *msg)
{
#ifdef WIN32
	MessageBox(0,msg,"libsc_abort", MB_ICONSTOP|MB_OK);
#else
	fputs(msg,stderr);
	fflush(stderr);
#endif
}

void __libsc_noreturn(void)
{
	abort();
}

static
char message_prolog[] = "Internal error!\n\n";
static
char message_epilog[] = "\nPlease, report to developers\n";

static
void do_message(const char *msg, const char *msg2, const char *file, int line)
{
	std::ostringstream s;
	s << message_prolog;
	if (msg)
		s << msg << std::endl;
	s << msg2 << " at file " << file << " line " << line << std::endl;
	s << message_epilog;
	message(s.str().c_str());
}

static inline
void __actual_abort()
{
#ifdef WIN32
	abort();
#else
	// yet will work only for gcc & x86
	__asm__ ("int $3");
#endif
}

void __libsc_abort(const char *msg, const char *file, int line)
{
	do_message(msg,"aborting",file,line);
	__actual_abort();
}

void __libsc_diag_message(const char *msg, const char *file, int line)
{
	do_message(msg,"diag message",file,line);
}

void __libsc_critical_message(const char *msg, const char *file, int line)
{
	do_message(msg,"critical condition",file,line);
	if (abort_on_critical)
		__actual_abort();
}
