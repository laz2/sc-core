
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


#ifndef __FALLBACK_H__
#define __FALLBACK_H__

#include "libsc.h"
#include "sc_constraint.h"
#include "fixalloc.h"

// fallback iterator assumes that last item of deconstruct program is return
class sc_fallback_iterator : public sc_iterator {
public:
	DECLARE_FIXALLOCER(sc_fallback_iterator)
protected:
	bool over;
	char sc_set_stack_len;
	long len;
	union iter_part {
		sc_iterator *iter;
		sc_filter filter;
		sc_func func;
		sc_plain_set set_state;
	} *current;
	sc_session *session;
	const sc_deconstruct *dec;
	sc_param *registers;
	void __next(int);
	int dummy[1]; // to pad size to 32 bytes on x86 (important for fixallocer)
public:
	sc_fallback_iterator(sc_session *s,sc_constraint *_constr,
			const sc_deconstruct *_dec,bool _sink);
	sc_retval next();
	bool is_over() {return over;}
	sc_addr value(sc_uint num = 0);
	~sc_fallback_iterator();
};

#endif // __FALLBACK_H__
