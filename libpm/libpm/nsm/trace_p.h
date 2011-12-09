
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


#ifndef __TRACE_H__
#define __TRACE_H__

#include <libsc/sc_types.h>
#include "nsm.h"
#include <libsc/libsc.h>

#define TRACE_FULL_NAME

//TODO <--
#undef TRACE_ISOM
#undef TRACE_ISOM_RESULT
//#define TRACE_ISOM
//#define TRACE_ISOM_RESULT
// -->

inline char *ADDR_OR_IDTF(sc_addr x) {
	static char s[4296];
	if (strcmp(nsm_session->get_idtf(x).c_str(),"")) {
#ifdef TRACE_FULL_NAME
		strcpy(s, x->seg->get_full_uri().c_str());
		sprintf(s, "%s/%s", x->seg->get_full_uri().c_str(),
							nsm_session->get_idtf(x).c_str());
#else
		sprintf(s, "%s", nsm_session->get_idtf(x).c_str());
#endif
	}
	else
		sprintf(s, "%p", (void *)x);

	return s;
}

char *type2string (sc_type type);



#ifdef TRACE_ISOM_RESULT
	#define R_TRACE_ELEM(x)\
		R_SYSTRACE2(type2string(nsm_session->get_type(x)), "\t")\
		R_SYSTRACE2(ADDR_OR_IDTF(nsm_session->get_beg(x)), "\t")\
		R_SYSTRACE2(ADDR_OR_IDTF(x),"\t")\
		R_SYSTRACE (ADDR_OR_IDTF(nsm_session->get_end(x)));
	#define R_SYSTRACE(x) TRACE_DEBUG(x);
	#define R_SYSTRACE2(x,y) TRACE_DEBUG(x << y);
#else  // TRACE_ISOM_RESULT
	#define R_TRACE_ELEM(x) (void)0;
	#define R_SYSTRACE(x) (void)0;
	#define R_SYSTRACE2(x,y) (void)0;
#endif // TRACE_ISOM_RESULT


#ifdef TRACE_ISOM
	#define SYSTRACE(x) TRACE_DEBUG(x);
	#define SYSTRACE2(x,y) TRACE_DEBUG(x << y);
	#define SYSTRACE3(x,y,z) TRACE_DEBUG(x << y << z);

	#define TRACE_ELEM(x)\
		SYSTRACE2(type2string(nsm_session->get_type(x)), "\t")\
		SYSTRACE2(ADDR_OR_IDTF(nsm_session->get_beg(x)), "\t")\
		SYSTRACE2(ADDR_OR_IDTF(x),"\t")\
		SYSTRACE (ADDR_OR_IDTF(nsm_session->get_end(x)));

	#define TRACE_ELEM_1(txt,x) SYSTRACE2(txt,"\t");TRACE_ELEM(x);SYSTRACE("\n");

	#define TRACE_LIST_SC_ADDR(s, lst) {\
		SYSTRACE2(s, ":\n")\
		LISTSC_ADDR::const_iterator i = lst.begin();\
		for (; i!=lst.end(); i++) {\
			TRACE_ELEM(*i);\
			SYSTRACE("\n")\
		}\
		SYSTRACE("\n")\
	}

	typedef std::map<sc_addr, sc_addr>	MAPSC_ADDR;
	inline void TRACE_MAP_SC_ADDR_SC_ADDR(const char *s, std::list<MAPSC_ADDR> lst) {
		SYSTRACE2(s, ":\n")
		std::list<MAPSC_ADDR>::const_iterator i = lst.begin();
		for (; i!=lst.end(); i++) {
			MAPSC_ADDR::const_iterator j = i->begin();
			for (; j!=i->end(); j++) {
				TRACE_ELEM(j->first)
				SYSTRACE("<=>")
				TRACE_ELEM(j->second);
				SYSTRACE("\n");
			}
		}
		SYSTRACE("\n");
	}

	#define TRACE_MAP_INT_SC_ADDR(s, lst) {\
		SYSTRACE2(s, ":\n")\
		std::map<int, sc_addr>::const_iterator i = lst.begin();\
		for (; i!=lst.end(); i++) {\
			SYSTRACE2(i->first, "<=>")\
			TRACE_ELEM(i->second);\
			SYSTRACE("\n")\
		}\
		SYSTRACE("\n")\
	}

	#define TRACE_MAP_SC_ADDR(s, lst) {\
		SYSTRACE2(s, ":\n");\
		MAPSC_ADDR::const_iterator i = lst.begin();\
		for (; i!=lst.end(); i++) {\
			TRACE_ELEM(i->first);\
			SYSTRACE("<=>")\
			TRACE_ELEM(i->second);\
			SYSTRACE("\n")\
		}\
		SYSTRACE("\n")\
	}

	inline void TRACE_SET_ELEM(const sc_addr _set)
	{
		SYSTRACE("\n--------TRACE_SET_ELEM------------\n");
		sc_iterator* it = system_session->create_iterator(
			sc_constraint_new(CONSTR_3_f_a_a,_set,0,0),true);
		for(;!it->is_over();it->next()){
			TRACE_ELEM(it->value(2));
			SYSTRACE("\n");
		}
		delete it;
	}

#else
	#define TRACE_ELEM(x) (void)0;
	#define TRACE_SET_ELEM(x) (void)0;
	#define TRACE_ELEM_1(txt,x) (void)0;
	#define TRACE_LIST_SC_ADDR(s, lst) (void)0;
	#define TRACE_MAP_SC_ADDR(s, lst) (void)0;
	#define TRACE_MAP_INT_SC_ADDR(s,lst) (void)0;
	#define TRACE_MAP_SC_ADDR_SC_ADDR(x,y) (void)0; 
	#define SYSTRACE(x) (void)0;
	#define SYSTRACE2(x,y) (void)0;
	#define SYSTRACE3(x,y,z) (void)0;
#endif //#ifdef TRACE_ISOM

#endif //__TRACE_H__
