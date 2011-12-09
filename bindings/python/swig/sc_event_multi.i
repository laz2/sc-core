
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

%module(directors="1") sc_event_multi

/*class sc_wait;
class sc_event_multi;
class sc_event_wait : public sc_wait {
		sc_event_multi *parent;
	public:
		sc_event_wait(sc_event_multi *_parent) {parent = _parent;}
		bool activate(sc_wait_type type,sc_param *prm,int len);
		~sc_event_wait();
	};
*/

%{
	#include <libsc/sc_event_multi.h>
%}

%feature("director") sc_event_multi;
%include libsc/sc_event_multi.h
/*
class LIBSC_API sc_event_multi {
	
	typedef std::list<sc_event_wait *> my_list_type;
	my_list_type list;
	// will work only once
public:
	void detach_all();
	virtual bool activate(sc_wait_type type,sc_param *prm,int len) = 0;
	sc_retval attach_to(sc_session *s,sc_wait_type type,sc_param prm[],int len);
	virtual ~sc_event_multi();
};*/
/*
%{
	typedef sc_event_multi::sc_event_wait sc_event_wait;
%}
*/