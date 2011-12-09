
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

%module sc_generator
%include stl.i 
%include windows.i
%{
#include <libsc/sc_generator.h>
%}
class LIBSC_API sc_generator_proxy {
protected:
	sc_session *session;
public:
	sc_generator_proxy(sc_session *_session) : session(_session) {}
	virtual ~sc_generator_proxy();

	sc_session *get_session() {return session;}

	virtual sc_addr create_el(sc_segment *seg,sc_type)=0;
	virtual sc_retval erase_el(sc_addr el)=0;
	virtual sc_retval set_beg(sc_addr arc,sc_addr beg)=0;
	virtual sc_retval set_end(sc_addr arc,sc_addr end)=0;
	virtual sc_retval set_idtf(sc_addr arc,sc_string idtf) = 0;
	virtual sc_retval set_content(sc_addr arc,const Content &c) = 0;
	virtual sc_retval change_type(sc_addr arc,sc_type type) = 0;
};

// TODO: rework sc_generator to work through generator proxy
struct sc_generator_state;
class LIBSC_API sc_generator {
	sc_segment *active_segment;
	sc_session *session;
	// lets hide generator state
	// minus is additional malloc
	struct sc_generator_state *state;
	sc_addr generate_internal(const char *idtf,sc_type type,sc_segment *seg);
	sc_addr generate_find_internal(const char *idtf,sc_type type,sc_segment *seg);
	void push_state(bool);
	sc_addr internal_gen_arc(sc_addr from,sc_addr to,sc_type arc_type,sc_segment *seg);
	sc_addr __element(sc_addr);
	sc_generator_proxy *proxy;
	bool own_proxy;
	sc_retval gen3_f_a_f(sc_addr beg,sc_addr *arc,sc_segment *seg,sc_type type,sc_addr end);
	
public:
	sc_type arc_type;
	sc_type element_type;
	sc_type set_arc_type;
	sc_type system_arc_type;
	sc_type attr_arc_type;

	sc_generator(sc_session *_session,sc_segment *segment);
	sc_generator(sc_generator_proxy *proxy,sc_segment *segment);
	~sc_generator();
	sc_session *get_session() {return session;}
	sc_segment *set_active_segment(sc_segment *seg)
	{
		sc_segment *prev = active_segment;
		active_segment = seg;
		return prev;
	}

	// just to wrap proxy methods
	sc_retval set_content(sc_addr el,const Content &cont);
	sc_retval set_idtf(sc_addr el,sc_string str);
	sc_retval change_type(sc_addr el,sc_type type);

	sc_segment *get_active_segment() {return active_segment;};
	sc_addr enter_set(sc_addr set);
	sc_addr enter_set(const char *idtf,sc_type type=0,sc_segment *segment = 0);
	void leave_set();
	sc_addr enter_system(sc_addr);
	sc_addr enter_system(const char *idtf,sc_type type=0,sc_segment *segment = 0);
	void leave_system();

	sc_addr attr(sc_addr attr);
	sc_addr attr(const char *idtf,sc_type type=0,sc_segment *segment = 0);
	sc_addr element(sc_addr el);
	//alias to element(sc_addr)
	sc_generator &operator <<(sc_addr el)
	{
		element(el);
		return *this;
	}
	sc_addr element(const char *idtf,sc_type type=0,sc_segment *segment = 0);
	void finish();
	void arc();
	sc_addr gen_arc(sc_addr from,sc_addr to,const char *idtf,sc_type type=0,sc_segment *segment = 0);
	void leave_all();
};

