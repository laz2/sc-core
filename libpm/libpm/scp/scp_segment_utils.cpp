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

/// Adds signs of segment (@p sign) and its META-segment in opened segments set of SCP-process.
static void __add_sign(scp_process_info *info, sc_addr sign)
{
	sc_addr signset = scp_process_info::get_opened_segs(info->process);

	if (search_3_f_cpa_f(system_session, signset, 0, sign)) {
		gen3_f_cpa_f(system_session, signset, 0, sign);
	}

	sc_addr signmeta = seg2sign(info->session, sign->seg);
	if (search_3_f_cpa_f(system_session, signset, 0, signmeta)) {
		gen3_f_cpa_f(system_session, signset, 0, signmeta);
	}
}

/// Removes segments @p sign from opened segments set of SCP-process.
static void __remove_sign(scp_process_info *info, sc_addr sign)
{
	sc_addr signset = scp_process_info::get_opened_segs(info->process);

	// TODO: removes of META-segments signs?
	sc_set::exclude_from(system_session, sign, signset);
}

sc_retval scp_open_segment_uri(scp_process_info *info, sc_addr ptr, sc_addr var)
{
	Content cont;
	sc_session *s = info->session;

	cont = s->get_content(ptr);
	if (cont.type() != TCSTRING) {
		return RV_ERR_GEN;
	}

	sc_segment *seg;

	if (!(seg = s->open_segment(cont))) {
		return RV_ELSE_GEN;
	}

	sc_addr sign = s->uri2sign(seg->get_full_uri());
	if (!sign) {
		SC_ABORT();
	}

	__add_sign(info, sign);

	if (scp_set_variable(info, var, sign)) {
		return RV_ERR_GEN;
	}

	return RV_OK;
}

sc_retval scp_open_segment(scp_process_info *info, sc_addr sign)
{
	sc_session *s = info->session;
	sc_segment *seg;

	if (!(seg = s->open_segment(s->sign2uri(sign)))) {
		return RV_ELSE_GEN;
	}

	__add_sign(info, sign);

	return RV_OK;
}

sc_retval scp_close_segment(scp_process_info *info, sc_addr sign)
{
	__remove_sign(info, sign);

	sc_string str = info->session->sign2uri(sign);
	if (!str.size()) {
		return RV_ERR_GEN;
	}

	info->session->close_segment(str);
	return RV_OK;
}

sc_retval scp_create_segment(scp_process_info *info, sc_addr ptr, sc_addr var)
{
	Content cont;
	sc_session *s = info->session;

	cont = s->get_content(ptr);
	if (cont.type() != TCSTRING) {
		return RV_ERR_GEN;
	}

	sc_segment *seg;

	sc_string str = cont;

	if (!(seg = create_segment_full_path(s, str))) {
		return RV_ELSE_GEN;
	}

	sc_addr sign = s->uri2sign(seg->get_full_uri());
	if (!sign) {
		SC_ABORT();
	}

	__add_sign(info, sign);

	if (scp_set_variable(info, var, sign)) {
		return RV_ERR_GEN;
	}

	return RV_OK;
}

sc_retval scp_create_unique_segment(scp_process_info *info, sc_addr var)
{
	sc_session *s = info->session;

	sc_segment *seg = create_unique_segment(s, dirname(s->sign2uri(info->defaultseg_sign)));

	if (!seg) {
		return RV_ELSE_GEN;
	}

	sc_addr sign = s->uri2sign(seg->get_full_uri());
	if (!sign) {
		SC_ABORT();
	}

	__add_sign(info, sign);

	if (scp_set_variable(info, var, sign)) {
		return RV_ERR_GEN;
	}

	return RV_OK;
}

static void span_include_in_set(sc_session *s, sc_segment *ds, sc_addr S, sc_addr x)
{
	if (s->gen3_f_a_f(S, 0, ds, SC_A_CONST|SC_POS, x)) {
		SC_ABORT();
	}
}

/**
@code
Data:
	S set of opened segments signs
	L FIFO
	s sign of first segment
	max_lev
Algorithm:
	gen {s,L}, [<s,0> <- L];
	gen {S,sg} [S->sg];
	while ((<sg,lev> = (find x, {L},[L -> x]))) {
		open_segment(sg);
		gen {S,sg} [S->sg];
		lev = find x, {lev,next_number},[next_number -> <lev,x>];
		if (exist {lev,maxlev,">"}, [">" -> <lev,maxlev>])
			continue;
		foreach x ( search {sg,have_links_to_},
						[ sg -> have_links_to_:x] ) {
			if ( exist {S,x}, [S -> x] )
				continue;
			gen {s,L}, [L -> x];
			gen {S,sg} [S->sg];
		}
	}
	return S;
@endcode
*/
sc_retval scp_span_segments(scp_process_info *info, sc_addr s, sc_addr S, int maxlev)
{
	typedef std::pair<sc_addr, int> span_pair;
	typedef std::deque<span_pair> span_fifo;
	span_fifo L;
	int lev;
	sc_addr sg;
	sc_session *ses = info->session;
	sc_segment *defseg = ses->find_segment(ses->sign2uri(info->defaultseg_sign));
	sc_segment *seg;
	std::auto_ptr<segment_set> segset(new segment_set());

	if (!defseg) {
		SC_ABORT();
	}

	L.push_back(span_pair(s, 0));
	span_include_in_set(ses, defseg, S, s);
	segset->set(ses->open_segment(ses->sign2uri(s))->index);

	while (!L.empty()) {
		span_pair l = L.front();
		L.pop_front();
		lev = l.second;
		sg = l.first;

		if (!(seg = ses->open_segment(ses->sign2uri(sg)))) {
			SC_ABORT();
		}

		__add_sign(info, sg);

		lev++;
		if (maxlev > 0 && lev > maxlev) {
			continue;
		}

		sc_segment::iterator *iter = seg->create_link_iterator();
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();
			sc_addr addr = seg->get_element_addr(ll);
			sc_segment *oseg;
			sc_addr osign;
			if (!addr) {
				continue;
			}

			oseg = addr->seg;
			osign = ses->uri2sign(oseg->get_full_uri());
			if (!osign) {
				std::cout << "[SCP] While spanning segments for \"" << SC_URI(s) << "\" not found segment sign of link target \""
					<< SC_URI(addr) << "\", referenced from \"" << SC_URI(seg) << "\"" <<std::endl;
				continue;
			}
			if (segset->is_set(oseg->index)) {
				continue;
			}

			if (oseg->ring_0) {
				continue;
			}
			// if (span_is_in_set(ses,S,osign))
			// 	continue;
			segset->set(oseg->index);
			span_include_in_set(ses, defseg, S, osign);
			L.push_back(span_pair(osign, lev));
		}
		delete iter;
	}
	return RV_OK;
}

sc_retval scp_opendir_uri(scp_process_info *info, sc_addr ptr, sc_addr var)
{
	Content cont;
	sc_session *s = info->session;

	cont = s->get_content(ptr);
	if (cont.type() != TCSTRING) {
		return RV_ERR_GEN;
	}

	sc_segment *seg;

	sc_string str = cont;

	if (!(seg = s->open_segment(str + "/META"))) {
		return RV_ELSE_GEN;
	}

	sc_addr sign = s->uri2sign(seg->get_full_uri());
	if (!sign) {
		SC_ABORT();
	}

	__add_sign(info, sign);

	if (scp_set_variable(info, var, sign)) {
		return RV_ERR_GEN;
	}

	return RV_OK;
}

sc_retval scp_opendir(scp_process_info *info, sc_addr sign, sc_addr var)
{
	sc_session *s = info->session;
	sc_segment *seg;

	if (!(seg = s->open_segment(s->sign2uri(sign) + "/META"))) {
		return RV_ELSE_GEN;
	}

	sign = seg2sign(s, seg);
	assert(sign);

	__add_sign(info, sign);

	if (scp_set_variable(info, var, sign)) {
		return RV_ERR_GEN;
	}

	return RV_OK;
}
