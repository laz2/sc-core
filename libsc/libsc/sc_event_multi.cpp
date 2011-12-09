
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

#include "sc_event_multi.h"

void sc_event_multi::detach_all()
{
	while (!list.empty()) {
		sc_event_wait *w = *list.begin();
		w->remove();
		list.remove(w);
		delete w;
	}
}

sc_retval sc_event_multi::attach_to(sc_session *s,sc_wait_type type,sc_param prm[],int len)
{
	sc_event_wait *w = new sc_event_wait(this);
	list.push_back(w);
	return s->attach_wait(type,prm,len,w);
}
