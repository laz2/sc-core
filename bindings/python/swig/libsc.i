
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

%module pm

// Suppress SWIG warning
#pragma SWIG nowarn=SWIGWARN_PARSE_NESTED_CLASS

class sc_segment_iterator
{
public:
	virtual sc_addr_ll next() = 0;
	virtual bool is_over() = 0;
	virtual ~iterator(){}
};

%{
	#include <libsc/libsc.h>
	#include <libsc/segment_utils.h>
	#include <libsc/sc_event.h>

	#include <libsc/sc_utils.h>
	#include <libsc/segtable.h>
	#include <libsc/idtf_gen.h>
%}

%import libsc/config.h

%include stl.i
%include cdata.i
%include cmalloc.i
%include carrays.i
%include typemaps.i
%include windows.i

%import tgf_module.i

%ignore libsc_tmp_prefix;
%ignore search_res_holder; 
%ignore operator<<;

%immutable sc_atoms_segment;
%immutable system_session;

%include libsc/libsc.h
%include libsc/segtable.h
%include sc_addr.i
%include libsc/sc_event.h
%include libsc/sc_segment.h
%{
	typedef sc_segment::iterator sc_segment_iterator;
%}

/*%ignore search_res_holder {
    sc_addr item(int index)
	{
		return holder[index];
	}
 }*/


 %inline {
	class _iterator_wrapper_sc_iterator {
	public:
		_iterator_wrapper_sc_iterator(sc_iterator *_impl)
			: impl(_impl), first(true)
		{}

		PyObject* next() {
			if (impl->is_over()) {
				PyErr_SetNone(PyExc_StopIteration);
				return NULL;
			} else {
				if (first) {
					first = false;
					return SWIG_From_int(static_cast<int>(RV_OK));
				} else {
					sc_retval rv = impl->next();
					if (impl->is_over()) {
						PyErr_SetNone(PyExc_StopIteration);
						return NULL;
					} else {
						return SWIG_From_int(static_cast<int>(rv));
					}
				}
			}
		}

	private:
		sc_iterator *impl; // FIXME: I think we don't need to free memory
		// By iteration protocol we need to call next() at begin of iteration.
		// By sc_iterator interface iterator already at the first result.
		// Flag first indicates that iterator at the first result.
		bool first; 
	};
};

%include libsc/sc_iterator.h

%extend sc_iterator {
	_iterator_wrapper_sc_iterator* __iter__() {
		return new _iterator_wrapper_sc_iterator($self);
	}
};


%ignore sc_type_valid_table;
%include sctypes.i

%include content.i
%include libsc/segment_utils.h

%rename(sc_constraint_new_by_info) __sc_constraint_new(sc_constraint_info * info);
%include scconstraint.i


%include stdconstraints.i

%include sc_event_multi.i

%include libsc/sc_utils.h

%inline %{
	int get_verb_output() {
		return verb_output;
	}

	void set_verb_output(int val) {
		verb_output = val;
	}

	int get_diag_output() {
		return diag_output;
	}

	void set_diag_output(int val) {
		diag_output = val;
	}

	int get_allow_unresolved_links() {
		return allow_unresolved_links;
	}
    
	void set_allow_unresolved_links(bool v) {
		allow_unresolved_links = v;
	}
 %}

%include pm_keynodes.i

%{
	#include "libsc/tgf_io.h"
%}
%immutable tgf2sc_type_table;
%immutable sc2tgf_type_table;
%include libsc/tgf_io.h
%free(tgf_dump)

%include sc_generator.i


%inline %{
	sc_param* ADDR_AS_PAR(sc_addr addr) {
		sc_param* res = new sc_param();
		res->addr=addr;
		return res;
	}
 %}
 
%include libsc/idtf_gen.h

//
// Structures in sc-memory
//
%{
	#include <libsc/sc_list.h>
	#include <libsc/sc_queue.h>
%}
%rename(sc_list) sc_list_iterator; // FIXME: Why swig names sc_list as sc_list_iterator?
%ignore *::operator++;
%include libsc/sc_list.h
%include libsc/sc_queue.h


%include pm.i
