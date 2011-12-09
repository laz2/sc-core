
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

static
sc_retval create_dir(sc_session *s,const sc_string &uri);

sc_segment *create_unique_segment(sc_session *s,const sc_string &base)
{
	static char buf[32];
	static unsigned int counter = 0;
	sc_segment *seg;
	int pos = base.rfind('/');
	if (pos !=0 && s->_stat(base.substr(0,pos)) != RV_ELSE_GEN)
		return 0;
	if (s->_stat(base) == RV_ERR_GEN)
		if ((seg = s->create_segment(base)))
			return seg;
		else
			abort();
	do {
		sprintf(buf,"%u",counter++);
	} while (s->_stat(base+buf) != RV_ERR_GEN);
	return s->create_segment(base+buf);
}

sc_string create_unique_dir(sc_session *s,const sc_string &base)
{
	static char buf[32];
	static unsigned int counter = 0;
	int pos = base.rfind('/');
	sc_retval rv = s->_stat(base.substr(0,pos));
	if (!rv)
		return "";
	if (rv == RV_ERR_GEN)
		if (create_dir(s,base.substr(0,pos)))
			return "";
	if (s->_stat(base) == RV_ERR_GEN)
		if (!s->mkdir(base))
			return base;
		else
			abort();
	do {
		sprintf(buf,"%u",counter++);
	} while (s->_stat(base+buf) != RV_ERR_GEN);
	rv = s->mkdir(base+buf);
	assert(!rv);
	return base+buf;
}

sc_segment *is_segment(sc_session *s,sc_addr sign)
{
	sc_string str = s->sign2uri(sign);
	if (s->get_error())
		return 0;
	return s->find_segment(str);
}
sc_addr seg2sign(sc_session *s,sc_segment *seg)
{
	return s->uri2sign(seg->get_full_uri());
}
sc_segment *sign2seg(sc_session *s,sc_addr addr)
{
	return is_segment(s,addr);
}

sc_segment *create_segment_full_path(sc_session *s,const sc_string &uri)
{
	assert(uri[0] == '/');
	int i = 1;
	int len = uri.size();
	sc_retval rv;
	for (;;) {
		i = find_first_slash(uri,i);
		if (i == len)
			break;
		rv = s->_stat(uri.substr(0,i));
		if (rv == RV_OK)
			return 0;
		if (rv != RV_ELSE_GEN) {
			if (s->mkdir(uri.substr(0,i)))
				return 0;
		}
		i++;
	}
	return s->create_segment(uri);
}

static
sc_retval create_dir(sc_session *s,const sc_string &uri)
{
	assert(uri[0] == '/');
	int i = 1;
	int len = uri.size();
	sc_retval rv;
	for (;;) {
		i = find_first_slash(uri,i);
		if (i == len)
			break;
		rv = s->_stat(uri.substr(0,i));
		if (rv == RV_OK)
			return RV_ERR_GEN;
		if (rv != RV_ELSE_GEN) {
			if (s->mkdir(uri.substr(0,i)))
				return 0;
		}
		i++;
	}
	return s->mkdir(uri);
}

static inline
sc_addr	__cs_map(sc_session *s,sc_segment *orig,sc_segment *target,sc_addr addr)
{
	if (addr->seg != orig)
		return addr;
	sc_addr ret = s->find_by_idtf(s->get_idtf(addr),target);
	assert(ret);
	return ret;
}

// carefully revise this
// far from perfect 2 pass algorithm
sc_segment *copy_segment(sc_session *s,sc_segment *orig,const sc_string &uri)
{
	sc_retval rv = s->_stat(uri);
	if (!rv)
		return 0;
	if (RV_IS_ERR(rv)) {
		if (create_dir(s,uri))
			return 0;
	}
	sc_string name = basename(orig->get_full_uri());
	if (!RV_IS_ERR(s->_stat(uri+"/"+name)))
		return 0;
	sc_segment *t = s->create_segment(uri+"/"+name);
	if (!t)
		return t;
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, orig, false), true);
	if (!iter)
		return 0;
	for (;!iter->is_over();iter->next()) {
		//printf ("copy element: %s\n", s->get_idtf(iter->value(0)).c_str());
		sc_addr addr = iter->value(0);
		sc_type type = s->get_type(addr);
		Content cont = s->get_content(addr);
		sc_string idtf = s->get_idtf(addr);
		sc_addr targ = s->create_el(t,type);
		assert(targ);
		rv = s->set_content(targ,cont);
		assert(!rv);
		rv = s->set_idtf(targ,idtf);
		assert(!rv);
	}
	delete iter;
	// pass 1.5 open all segments we point to
	{
		sc_segment::iterator *iter = orig->create_link_iterator();
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();
			s->open_segment(dirname(orig->get_link_target(ll)));
		}
		delete iter;
	}
	// pass 2. set arc ends
	iter = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, orig, false), true);
	for (;!iter->is_over();iter->next()) {
		sc_addr arc = iter->value(0);
		if (!(s->get_type(arc) & SC_CONNECTOR_MASK))
			continue;
		sc_addr beg = s->get_beg(arc);
		sc_addr end = s->get_end(arc);
		if (beg) {
			rv = s->set_beg(__cs_map(s,orig,t,arc),__cs_map(s,orig,t,beg));
			assert(!rv);
		}
		if (end) {
			rv = s->set_end(__cs_map(s,orig,t,arc),__cs_map(s,orig,t,end));
			assert(!rv);
		}
	}
	delete iter;
	return t;
}

// carefully revise this
// far from perfect 2 pass algorithm
sc_segment *copy_segment_content(sc_session *s,sc_segment *orig,const sc_string &pat)
{
	sc_retval rv;
	sc_segment *t = create_unique_segment(s,pat);
	if (!t)
		return t;
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, orig, false), true);
	if (!iter)
		return 0;
	for (;!iter->is_over();iter->next()) {
		//printf ("copy element: %s\n", s->get_idtf(iter->value(0)).c_str());
		sc_addr addr = iter->value(0);
		sc_type type = s->get_type(addr);
		Content cont = s->get_content(addr);
		sc_string idtf = s->get_idtf(addr);
		sc_addr targ = s->create_el(t,type);
		assert(targ);
		rv = s->set_content(targ,cont);
		assert(!rv);
		rv = s->set_idtf(targ,idtf);
		assert(!rv);
	}
	delete iter;
	// pass 1.5 open all segments we point to
	{
		sc_segment::iterator *iter = orig->create_link_iterator();
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();
			s->open_segment(dirname(orig->get_link_target(ll)));
		}
		delete iter;
	}
	// pass 2. set arc ends
	iter = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, orig, false), true);
	for (;!iter->is_over();iter->next()) {
		sc_addr arc = iter->value(0);
		if (!(s->get_type(arc) & SC_CONNECTOR_MASK))
			continue;
		sc_addr beg = s->get_beg(arc);
		sc_addr end = s->get_end(arc);
		if (beg) {
			rv = s->set_beg(__cs_map(s,orig,t,arc),__cs_map(s,orig,t,beg));
			assert(!rv);
		}
		if (end) {
			rv = s->set_end(__cs_map(s,orig,t,arc),__cs_map(s,orig,t,end));
			assert(!rv);
		}
	}
	delete iter;
	return t;
}

sc_retval open_segset(sc_session *s, sc_addr segset, std::set< sc_segment* >* segs)
{
	return open_segset(s, segset, s, segs);
}

sc_retval open_segset(sc_session *s, sc_addr segset, sc_session *root, std::set< sc_segment* >* segs)
{
	if (!root->is_segment_opened(segset->seg))
		return RV_ERR_GEN;

	sc_iterator *it = root->create_iterator(
		sc_constraint_new(CONSTR_3_f_a_a, segset, SC_A_CONST|SC_POS, SC_N_CONST),true);

	sc_for_each (it) {
		sc_addr arc = it->value(1);
		sc_addr sign = root->get_end_spin(arc);
		if (sign) {
			sc_segment *seg = s->open_segment(root->sign2uri(sign));
			if (segs && seg) segs->insert(seg);
		}
	}

	return RV_OK;
}

void mkdir_by_full_uri(sc_session *s, const sc_string &uri)
{
	int i=0,len = uri.size();
	assert(uri[0] == '/');
	do {
		for (i++;i<len;i++)
			if (uri[i] == '/')
				break;
		s->mkdir(uri.substr(0,i));
	} while (i<len);
}
