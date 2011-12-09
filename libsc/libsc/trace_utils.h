
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


/**
 * @file trace_utils.h
 * @brief Определение функция трассировки.
 * @ingroup libsc
 */

#ifndef __TRACE_UTILS_H___
#define __TRACE_UTILS_H___

#define TRACE_FULL_NAME


#ifdef _DEBUG

inline
char *type2string (sc_type type) {
	static char s[80];
	s[0]='\0';
	if (type & SC_EMPTY)
		strcat(s, "SC_EMPTY|");
	if (type & SC_CONST)
		strcat(s,"SC_CONST|");
	if (type & SC_VAR)
		strcat(s,"SC_VAR|");
	if (type & SC_METAVAR)
		strcat(s,"SC_METAVAR|");
	if (type & SC_POS)
		strcat(s,"SC_POS|");
	if (type & SC_FUZ)
		strcat(s,"SC_FUZ|");
	if (type & SC_NEG)
		strcat(s,"SC_NEG|");
	if (type & SC_UNDF)
		strcat(s,"SC_UNDF|");
	if (type & SC_ARC_ACCESSORY)
		strcat(s,"SC_ARC|");
	if (type & SC_NODE)
		strcat(s,"SC_NODE|");

	return s;
}


inline char *ADDR_OR_IDTF(sc_addr x) {
	static char s[400];
	if (strcmp(system_session->get_idtf(x).c_str(),"")) {
#ifdef TRACE_FULL_NAME
		strcpy(s, x->seg->get_full_uri().c_str());
		sprintf(s, "%s/%s", x->seg->get_full_uri().c_str(),
			system_session->get_idtf(x).c_str());
#else
		sprintf(s, "%s", system_session->get_idtf(x).c_str());
#endif
	}
	else
		sprintf(s, "%p", (void *)x);

	return s;
}

inline void TRACE_ELEM(sc_addr x)
{
	CString s1, s2, s3;

	s1.Format ("%s\t%s\t",
		type2string(system_session->get_type(x)), 
		ADDR_OR_IDTF(system_session->get_beg(x)));

	s2.Format ("%s\t", ADDR_OR_IDTF(x));
	s3.Format ("%s", ADDR_OR_IDTF(system_session->get_end(x)));

	s1 = s1 + s2 + s3;

	TRACE_DEBUG((LPCTSTR)s1);
}

inline void TRACE_SET_ELEM(const sc_addr _set)
{
	TRACE_DEBUG("-----------TRACE_SET_ELEM----------------");
	sc_iterator* it = system_session->create_iterator(
		sc_constraint_new(CONSTR_3_f_a_a,_set,0,0),true);
	for(;!it->is_over();it->next()){
		TRACE_ELEM(it->value(2));
	}
	delete it;
}
#else
#	define type2string		(void)0
#	define ADDR_OR_IDTF		(void)0
#	define TRACE_ELEM		(void)0
#	define TRACE_SET_ELEM	(void)0
#endif //_DEBUG


#endif //__TRACE_UTILS_H___