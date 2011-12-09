
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

sc_string get_full_uri(sc_session *s, sc_addr el)
{
	if (!el) {
		return "NIL";
	} else {
		std::ostringstream str;
		str << el->seg->get_full_uri() << "/" << s->get_idtf(el);
		if (el->is_dead())
			str << "[dead]";
		return str.str();
	}
}

sc_string get_full_uri(sc_session *s, sc_segment *seg)
{
	return seg->get_full_uri();
}

sc_string get_quoted_full_uri(sc_session *s, sc_addr el)
{
	std::ostringstream str;
	str << "\"" << el->seg->get_full_uri() << "/" << s->get_idtf(el) << "\"";
	return str.str();
}

sc_string get_quoted_full_uri(sc_session *s, sc_segment *seg)
{
	std::ostringstream str;
	str << "\"" << seg->get_full_uri() << "\"";
	return str.str();
}

sc_string get_quoted_idtf(sc_session *s, sc_addr el)
{
	std::ostringstream str;
	str << "\"" << s->get_idtf(el) << "\"";
	return str.str();
}

bool is_system_id(const sc_string& id)
{
//	int i_id;
	sc_string tmp;
	//  Ситстемным является идентификатор если он GUID  или начинается
	//на TM_tmp_ или tmp_.
	//{B82F916D-9F4B-11D1-ABE4-00C04FC30999}
	if (id.size()==36) {		
		if ((id.c_str())[8]=='-'&&
			(id.c_str())[13]=='-'&&
			(id.c_str())[23]=='-')
			return true;		
	}
	if (memcmp(id.c_str(),"tmp_",4)==0) 
		return true;
	if (memcmp(id.c_str(),"@@",2)==0) 
		return true;
	if (memcmp(id.c_str(),"TM_tmp_",7)==0) 
		 return true;

	return false;
}

sc_addr get_el_by_full_uri(sc_session *s, const sc_string &uri)
{
	assert(s);
	assert(!uri.empty());

	sc_segment* segm_id; 
	sc_addr el=0; 
	sc_string segm_uri,name, str = uri;
	for(int i = str.size()-1;i>-1;i--){
		if(str[i]=='/'){
			name = str.substr(i+1);
			segm_uri = str.substr(0,i);
			break;
		}
	}
	if(segm_uri.empty()){ 
		throw std::logic_error("[get_idtf_by_full_uri(...)] segm_uri is empty ");
		return (sc_addr)NULL;
	}
	if (segm_uri.empty())
		return 0;

	if(s->_stat(segm_uri))
		return el; 
	else
		segm_id = s->open_segment(segm_uri.c_str());
	el = s->find_by_idtf(name.c_str(),segm_id);
	return el;
}

sc_addr create_el_by_full_uri(sc_session *s, const sc_string &uri, sc_type type)
{
	sc_string dn = dirname(uri);
	sc_segment *seg = s->open_segment(dn);
	if (!seg) {
		seg = create_segment_full_path(s,dirname(uri));
	}
		
	sc_addr rv;
	if (!(rv = s->find_by_idtf(basename(uri), seg))) {
		rv = s->create_el(seg, type);
		if (basename(uri).size()) {
			if (s->set_idtf(rv,basename(uri))) {
				SC_ABORT();
			}
		}
	}

	return rv;
}

sc_retval search_oneshot(sc_session *s,sc_constraint *c,bool sink,int len,...)
{
	sc_iterator *iter = s->create_iterator(c,sink);
	va_list list;
	assert(len<4096);
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	va_start(list,len);
	for (int i=0;i<len;i++) {
		int ind = va_arg(list,int);
		assert(ind<4096);
		sc_addr *ptr = va_arg(list,sc_addr *);
		*ptr = iter->value(ind);
	}
	va_end(list);
	delete iter;
	return RV_OK;
}

sc_retval search_oneshot_a(sc_session *s,sc_constraint *c,bool sink,int len,sc_param res[])
{
	sc_iterator *iter = s->create_iterator(c,sink);
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	for (int i=0;i<len;i++)
		res[i].addr = iter->value(res[i].i);
	delete iter;
	return RV_OK;
}

sc_retval search_3l2_f_cpa_cna_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr *e3, sc_addr *e4, sc_addr e5)
{
	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(
			CONSTR_3l2_f_a_a_a_f,
			e1,
			SC_A_CONST|SC_POS,
			SC_N_CONST,
			SC_A_CONST|SC_POS,
			e5
		),
		true
	);
	
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}

	if (e2) {
		*e2 = iter->value(1);
	}

	if (e3) {
		*e3 = iter->value(2);
	}

	if (e4) {
		*e4 = iter->value(3);
	}

	delete iter;
	return RV_OK;
}

sc_retval search_5_f_cpa_cna_cpa_f(sc_session *s,sc_addr e1,sc_addr *e2,sc_addr *e3,sc_addr *e4,sc_addr e5)
{
	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(
			CONSTR_5_f_a_a_a_f,
			e1,
			SC_A_CONST|SC_POS,
			SC_N_CONST,
			SC_A_CONST|SC_POS,
			e5
		),
		true
	);
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	if (e2)
		*e2 = iter->value(1);
	if (e3)
		*e3 = iter->value(2);
	if (e4)
		*e4 = iter->value(3);
	delete iter;
	return RV_OK;
}

sc_retval search_5_f_cpa_a_cpa_f(sc_session *s,sc_addr e1,sc_addr *e2,sc_addr *e3,sc_addr *e4,sc_addr e5)
{
	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(
			CONSTR_5_f_a_a_a_f,
			e1,
			SC_A_CONST|SC_POS,
			0,
			SC_A_CONST|SC_POS,
			e5
		),
		true
	);
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	if (e2)
		*e2 = iter->value(1);
	if (e3)
		*e3 = iter->value(2);
	if (e4)
		*e4 = iter->value(3);
	delete iter;
	return RV_OK;
}

sc_retval search_3_f_cpa_f(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr e3)
{
	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(
			CONSTR_3_f_a_f,
			e1,
			SC_A_CONST|SC_POS,
			e3
		),
		true
	);

	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}

	if (e2) {
		*e2 = iter->value(1);
	}

	delete iter;
	return RV_OK;
}

sc_retval search_3_f_cpa_cna(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr *e3)
{
	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(
			CONSTR_3_f_a_a,
			e1,
			SC_A_CONST|SC_POS,
			SC_N_CONST
		),
		true
	);

	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}

	if (e2) {
		*e2 = iter->value(1);
	}

	if (e3) {
		*e3 = iter->value(2);
	}

	delete iter;
	return RV_OK;
}

sc_retval search_3_f_cpa_a(sc_session *s, sc_addr e1, sc_addr *e2, sc_addr *e3)
{
	sc_iterator *iter = s->create_iterator(
		sc_constraint_new(
		CONSTR_3_f_a_a,
		e1,
		SC_A_CONST|SC_POS,
		0
		),
		true
	);

	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}

	if (e2) {
		*e2 = iter->value(1);
	}

	if (e3) {
		*e3 = iter->value(2);
	}

	delete iter;
	return RV_OK;
}

sc_retval delete_oneshot(sc_session *s,sc_constraint *c,bool sink,int len,...)
{
	sc_iterator *iter = s->create_iterator(c,sink);
	va_list list;
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	va_start(list,len);
	for (int i=0;i<len;i++) {
		int ind = va_arg(list,int);
		s->erase_el(iter->value(ind));
	}
	va_end(list);
	delete iter;
	return RV_OK;
}

int	diag_output = 0;
int	verb_output = 0;

#ifdef WIN32
#   ifdef _UNICODE
#       define vfprintf vfwprintf 
#   else 
#   endif
#endif

void	dprintf(const char *format,...)
{
	va_list list;
	if (!diag_output)
		return;
	va_start(list,format);
	vfprintf(stdout,format,list);
}

static
int	sc_type_is_compatible(sc_type one,sc_type two)
{
	sc_type diff = one ^ two; // 0 in fields where one and two are equal
				  // !0 otherwise
	if (diff & SC_CONSTANCY_MASK) // constancies are different
		return 0;
	if (diff & SC_SYNTACTIC_MASK)
		// if one of types are undf
		// then ok, else not ok
		return diff & SC_UNDF;
	if (!(one & SC_CONNECTOR_MASK)) // if arcs then fuzzyness must match
		return 1;
	// "IVP: there are modality & actuality must be checked...")  
	return !(diff & (SC_PERMANENCY_MASK | SC_FUZZYNESS_MASK));
}

static
bool	is_identical(sc_session *s,sc_addr from,sc_addr to)
{
	return from == to || s->get_idtf(from) == s->get_idtf(to);
}

glue_identical_checker::glue_identical_checker(sc_session *s, sc_segment *_from, sc_segment *_to)
	: session(s), from(_from), to(_to)
{
	check_in_progress = false;
}

glue_identical_checker::~glue_identical_checker()
{
}


// TODO: more serious lazy content support may be made here
// If we use it only for TM, then present code should work correctly
// code copied from glue_identical
bool glue_identical_checker::check()
{
	sc_session *s = session;
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, from, false), true);
	sc_addr from_addr,identical;
	sc_type from_type,to_type;
	bool ok_yet = true;

/*	for (;!it->is_over();it->next()) {
		sc_addr addr = it->value(0);
		if (addr->
	}
	delete it;
	it = s->create_iterator_on_segment(from,false);*/

	assert(!check_in_progress);
	check_in_progress = true;

	for (;!it->is_over();it->next()) {
		from_addr = it->value(0);
		identical = s->find_by_idtf(s->get_idtf(from_addr),to);
		if (!identical)
			continue;
		from_type = s->get_type(from_addr);
		to_type = s->get_type(identical);
		if (!sc_type_is_compatible(from_type,to_type)) {
			error(INCOMPATIBLE_TYPE, from_addr, identical);
			if (!check_in_progress)
				return false;
			ok_yet = false;
			continue;
		}
		if ((to_type & SC_CONNECTOR_MASK) && (from_type & SC_CONNECTOR_MASK)) {
			sc_addr end,from_end;
			end = s->get_end(identical);
			from_end = s->get_end(from_addr);
			if (end && !is_identical(s,from_end,end)) {
				error(DIFFERENT_ARC_TO, from_addr, identical);
				if (!check_in_progress)
					return false;
				ok_yet = false;
				continue;
			}
			end = s->get_beg(identical);
			from_end = s->get_beg(from_addr);
			if (end && !is_identical(s,from_end,end)) {
				error(DIFFERENT_ARC_FROM, from_addr, identical);
				if (!check_in_progress)
					return false;
				ok_yet = false;
				continue;
			}
		}
		const Content *cont_from = s->get_content_const(from_addr);
		const Content *cont_identical = s->get_content_const(identical);
		if (cont_from->type() != TCEMPTY
				&& cont_identical->type() != TCEMPTY) {
			if (*cont_from != *cont_identical) {
				error(DIFFERENT_CONTENT, from_addr, identical);
				if (!check_in_progress)
					return false;
				ok_yet = false;
				continue;
			}
		}
		to_merge(from_addr, identical);
	}
	delete it;
	return ok_yet;
}

void glue_identical_checker::error(/*error_reason*/int reason, sc_addr from_element, sc_addr to_element)
{
}

void glue_identical_checker::to_merge(sc_addr from_element, sc_addr to_element)
{
}


// WARNING: side effect is possible in case of error
// In practice they are ruled out by checking'
// 'merge-ability' with glue identical checker
sc_retval glue_identical(sc_session *s,sc_segment *from,sc_segment *to,sc_addr *err_el)
{
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, from, false), true);
	sc_iterator *it2;
	sc_addr from_addr,identical;
	sc_type from_type,to_type;
	for (;!it->is_over();it->next()) {
		Content from_content;
		from_addr = it->value(0);
		identical = s->find_by_idtf(s->get_idtf(from_addr),to);
		if (!identical)
			continue;
		from_type = s->get_type(from_addr);
		to_type = s->get_type(identical);
		if (!sc_type_is_compatible(from_type,to_type)) {
		err_:
			if (err_el)
				*err_el = identical;
			delete it;
			return RV_ERR_GEN;
		}
		// we assume content compatibility check is not needed here
		from_content = s->get_content(from_addr);
		// includes special case for lazy content
		if (from_content.type() != TCEMPTY
			&& (from_content.type() != TCLAZY
			|| s->get_content(identical).type() == TCEMPTY)) {
			s->set_content(identical,from_content);
		}
		if ((to_type & SC_SYNTACTIC_MASK) == SC_UNDF)
			// side effect here
			s->change_type(identical,from_type);
		else if ((to_type & SC_CONNECTOR_MASK) && (from_type & SC_CONNECTOR_MASK)) {
			sc_addr end,from_end;
			end = s->get_end(identical);
			from_end = s->get_end(from_addr);
			if (end) {
				if (!is_identical(s,from_end,end))
					goto err_;
			} else
				// dangerous side effect here
				s->set_end(identical,from_end);
			end = s->get_beg(identical);
			from_end = s->get_beg(from_addr);
			if (end) {
				if (!is_identical(s,from_end,end))
					goto err_;
			} else
				// dangerous side effect here
				s->set_beg(identical,from_end);
		}
		// now move arc's ends
		it2 = s->create_iterator(sc_constraint_new(
						 CONSTR_ALL_INPUT,
						 from_addr),
					 true);
		for (;!it2->is_over();it2->next())
			s->set_end(it->value(1),identical);
		delete it2;
		it2 = s->create_iterator(sc_constraint_new(
						 CONSTR_ALL_OUTPUT,
						 from_addr),
					 true);
		for (;!it2->is_over();it2->next())
			s->set_beg(it->value(1),identical);
		delete it2;
		s->erase_el(from_addr);
	}
	delete it;
	return RV_OK;
}

sc_retval glue_segment(sc_session *s,sc_segment *from,sc_segment *to)
{
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, from, false), true);
	for (;!it->is_over();it->next()) {
		sc_addr addr = it->value(0);
		if (!s->__move_element(addr,to))
			SC_ABORT();
	}
	delete it;
	return RV_OK;
}

sc_addr move_element_neighborhood(sc_session *s,sc_addr el,sc_segment *seg)
{
	sc_iterator *it;
	sc_segment *from = el->seg;
	el = s->__move_element(el,seg);
	if (!el)
		return 0;
	it = s->create_iterator(sc_constraint_new(CONSTR_ALL_INPUT,el),true);
	for (;!it->is_over();it->next()) {
		sc_addr a = it->value(1);
		if (a->seg != from)
			continue;
		a = move_element_neighborhood(s,a,seg);
		if (!a)
			return 0;
	}
	delete it;
	it = s->create_iterator(sc_constraint_new(CONSTR_ALL_OUTPUT,el),true);
	for (;!it->is_over();it->next()) {
		sc_addr a = it->value(1);
		if (a->seg != from)
			continue;
		a = move_element_neighborhood(s,a,seg);
		if (!a)
			return 0;
	}
	delete it;
	return el;
}

sc_retval sc_is_mergeable(sc_session *s, sc_addr main, sc_addr other,
			  sc_type *new_type, Content *new_content)
{
	sc_type main_type, other_type;
	Content main_cont, other_cont;
	main_type = s->get_type(main);
	other_type = s->get_type(other);
	main_cont = s->get_content(main);
	other_cont = s->get_content(other);
	if (s->error)
		return RV_ERR_GEN;

	if (main_type & SC_CONNECTOR_MASK) {
		sc_addr el,el2;
		el = s->get_beg(main);
		if (el && (el2 = s->get_beg(other)) && el != el2)
			return RV_ELSE_GEN;
		el = s->get_end(main);
		if (el && (el2 = s->get_end(other)) && el != el2)
			return RV_ELSE_GEN;
	}

	if ((main_type & SC_POOR_MASK) != (other_type & SC_POOR_MASK)) {
		if ((main_type & SC_CONSTANCY_MASK) != (other_type & SC_CONSTANCY_MASK))
			return RV_ELSE_GEN;

		if ((main_type & SC_SYNTACTIC_MASK) == SC_UNDF) {
			main_type = other_type;
		} else if ((other_type & SC_SYNTACTIC_MASK) != SC_UNDF)
			return RV_ELSE_GEN;
	}

	if (main_cont.type() == TCEMPTY)
		main_cont = other_cont;
	else if (other_cont.type() != TCEMPTY && other_cont != main_cont)
		return RV_ELSE_GEN;

	if (new_type)
		*new_type = main_type;

	if (new_content)
		*new_content = main_cont;
	
	return RV_OK;
}

sc_retval sc_merge_elements(sc_session *s,sc_addr main, sc_addr other)
{
	sc_type new_type;
	Content new_content;
	if (sc_is_mergeable(s,main,other,&new_type,&new_content))
		return RV_ELSE_GEN;
	sc_iterator *it = system_session->create_iterator(
		sc_constraint_new(CONSTR_ALL_INPUT,other),true);
	for (;!it->is_over();it->next())
		system_session->set_end(it->value(1),main);
	delete it;
	it = system_session->create_iterator(
		sc_constraint_new(CONSTR_ALL_OUTPUT,other),true);
	for (;!it->is_over();it->next())
		system_session->set_beg(it->value(1),main);
	delete it;
	if (s->get_content(main) != new_content)
		s->set_content(main,new_content);
	if (s->get_type(main) != new_type) {
		s->change_type(main,new_type);
		assert(!s->error);
	}

	if (new_type & SC_CONNECTOR_MASK) {
		if (!s->get_beg(main))
			s->set_beg(main,s->get_beg(other));
		if (!s->get_end(main))
			s->set_end(main,s->get_end(other));
	}

	sc_param prms[2];
	prms[0].addr = other;
	prms[1].addr = main;
	other->activate(SC_WAIT_MERGE, prms, 2);
	s->erase_el(other);
	return RV_OK;
}
