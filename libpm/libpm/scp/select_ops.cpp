
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
 * @file select_ops.cpp
 * @brief Implementation SCP-operators for elementary associative selecting SC-constructions.
 * @ingroup libpm
 */

#include "precompiled_p.h"

#include <libsc/sc_transaction.h>

static
void free_sc_plain_set(scp_opnd_info *info)
{
	if (info->fixed && info->set)
		info->value->die();
}

struct sc_plain_set_deleter
{
	scp_opnd_info *info;
	sc_plain_set_deleter(scp_opnd_info &_info) : info(&_info)
	{
	}
	~sc_plain_set_deleter()
	{
		free_sc_plain_set(info);
	}
};

sc_addr copy_set(scp_process_info *info, sc_addr set)
{
	sc_session *s = info->session;
	sc_segment *autoseg = s->open_segment(s->sign2uri(info->auto_dirent));
	sc_addr set_copy = s->create_el(autoseg, SC_N_CONST);
	
	sc_iterator *it = s->create_iterator(sc_constraint_new(
		CONSTR_3_f_a_a,
		set,
		0,
		0
		),true);
	
	for (; !it->is_over(); it->next()) {
		s->gen3_f_a_f(set_copy, 0, autoseg, SC_A_CONST|SC_POS, it->value(2));
	}

	delete it;
	return set_copy;
}

static
sc_retval sel_prep_value_y(scp_process_info *info, int index,sc_addr &set,sc_type &mask,bool &delete_set,sc_single_filter **filter)
{
	scp_opnd_info ei,si;
	sc_retval rv;
	sc_session *s = info->session;
	sc_addr autoseg = info->defaultseg_sign;

	//BREAKPOINT();
	delete_set = false;
	if (scp_fill_opndinfo(info,index,&ei))
		return RV_ERR_GEN;
	*filter = 0;
	if (RV_IS_ERR(get_multi_segc_filter(info,index,filter)))
		return RV_ERR_GEN;
	sc_plain_set_deleter d_ei(ei);
	rv = scp_fill_set_opndinfo(info,index,&si);
	if (RV_IS_ERR(rv))
		return RV_ERR_GEN;
	if (rv) { // if have no matching setN_ operand
		if (ei.set) {
			sc_segment *seg = is_segment(info->session,autoseg);
			sc_plain_set s = reinterpret_cast<sc_plain_set>(ei.value->impl);
			set = info->session->create_el(seg,SC_N_CONST);
			if (!set)
				return RV_ERR_GEN;
			delete_set = true;
			while (*s)
				info->session->gen3_f_a_f(set,0,seg,SC_A_CONST|SC_POS,*s++);
			mask = 0;
			return RV_OK;
		}
		if (ei.fixed) {
			sc_segment *seg = is_segment(info->session,autoseg);
			set = s->create_el(seg,SC_N_CONST);
			if (!set)
				return RV_ERR_GEN;
			delete_set = true;
			if (s->gen3_f_a_f(set,0,seg,SC_A_CONST|SC_POS,ei.value))
				return RV_ERR_GEN;
			mask = 0;
			return RV_OK;
		}
		mask = ei.type_mask;
		set = 0;
		return RV_OK;
	}
	mask = ei.type_mask;
	set = si.value;
	return RV_OK;
}


static
sc_retval sel_prep_value(scp_process_info *info,
			 int index,
			 sc_addr &orig_set,
			 sc_addr &search_set,
			 sc_type &mask,
			 sc_single_filter **filter)
{
	scp_opnd_info ei,si;
	sc_retval rv;
	sc_session *s = info->session;
	sc_addr autoseg = info->defaultseg_sign;

	//BREAKPOINT();
	if (scp_fill_opndinfo(info,index,&ei))
		return RV_ERR_GEN;
	sc_plain_set_deleter d_ei(ei);
	*filter = 0;
	orig_set = 0;
	if (RV_IS_ERR(get_multi_segc_filter(info,index,filter)))
		return RV_ERR_GEN;
	rv = scp_fill_set_opndinfo(info,index,&si);
	if (RV_IS_ERR(rv))
		return RV_ERR_GEN;
	if (rv) { // if have no matching setN_ operand
		if (ei.set) {
			sc_segment *seg = is_segment(info->session,autoseg);
			sc_plain_set s = reinterpret_cast<sc_plain_set>(ei.value->impl);
			search_set = info->session->create_el(seg,SC_N_CONST);
			if (!search_set)
				return RV_ERR_GEN;
			while (*s)
				info->session->gen3_f_a_f(search_set,0,seg,SC_A_CONST|SC_POS,*s++);
			mask = 0;
			return RV_OK;
		}
		if (ei.fixed) {
			sc_segment *seg = is_segment(info->session,autoseg);
			search_set = s->create_el(seg,SC_N_CONST);
			if (!search_set)
				return RV_ERR_GEN;
			if (s->gen3_f_a_f(search_set,0,seg,SC_A_CONST|SC_POS,ei.value))
				return RV_ERR_GEN;
			mask = 0;
			return RV_OK;
		}
		mask = ei.type_mask;
		search_set = 0;
		return RV_OK;
	}
	mask = ei.type_mask;
	orig_set = si.value;
	search_set = copy_set(info,orig_set);
	return RV_OK;
}

static
void	erase_occurences(sc_session *s,sc_addr _set,sc_addr el)
{
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_f,
							_set,
							SC_A_CONST|SC_POS,
							el
						),true);
	for (;!it->is_over();it->next())
		s->erase_el(it->value(1));
	delete it;
}

static
int compute_select_state(sc_session *s,sc_addr sets[])
{
	int i;
	for (i=0;sets[i];i++) {
		if (!search_oneshot(s,sc_constraint_new(
					CONSTR_in_set,
					sets[i],
					0
				),true,0))
			return SCP_STATE_THEN;
	}
	return SCP_STATE_ELSE;
}

SCP_OPERATOR_IMPL(scp_oper_selectNStr3)
{
	/*
	SCP_OPND_REQ_ASSIGN_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_ASSIGN_FIXED(info, 3, opnd3);
	*/

	sc_addr set[3];
	sc_addr orig_set[3];
	sc_type mask[3];
	sc_single_filter *filters[3];
	sc_iterator *iter = 0;
	sc_iterator_complex_filter<3> *filter;
	sc_retval rv;

	if (sel_prep_value(info,1,orig_set[0],set[0],mask[0],filters+0))
		return RV_ERR_GEN;

	if (sel_prep_value(info,2,orig_set[1],set[1],mask[1],filters+1))
		return RV_ERR_GEN;

	if (sel_prep_value(info,3,orig_set[2],set[2],mask[2],filters+2))
		return RV_ERR_GEN;

	SCP_CHECK_OPND_EXCESSIVE(info, N4_);
	SCP_CHECK_OPND_EXCESSIVE(info, N5_);
	SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[4]);
	SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[5]);

	if (set[0]) {
		iter = info->session->create_iterator(sc_constraint_new(
					CONSTR_sely3_p1,
					set[0], mask[0],
					set[1], mask[1],
					set[2], mask[2]
				),true);
	} else if (set[1]) {
		iter = info->session->create_iterator(sc_constraint_new(
					CONSTR_sely3_u1p2,
					mask[0],
					set[1], mask[1],
					set[2], mask[2]
				),true);
	} else if (set[2]) {
		iter = info->session->create_iterator(sc_constraint_new(
					CONSTR_sely3_u1u2p3,
					mask[0],
					mask[1],
					set[2], mask[2]
				),true);
	} else {
		rv = RV_ERR_GEN;
		goto __delete_sets;
	}

	filter = new sc_iterator_complex_filter<3>(iter,true);
	filter->set_filter(0,filters[0]);
	filters[0] = 0;
	filter->set_filter(1,filters[1]);
	filters[1] = 0;
	filter->set_filter(2,filters[2]);
	filters[2] = 0;

	for (;!filter->is_over();filter->next()) {
		if (orig_set[0])
			erase_occurences(info->session, orig_set[0], filter->value(0));
		if (orig_set[1])
			erase_occurences(info->session, orig_set[1], filter->value(1));
		if (orig_set[2])
			erase_occurences(info->session, orig_set[2], filter->value(2));
	}

	delete filter;
	rv = RV_OK;
__delete_sets:
	delete filters[0];
	delete filters[1];
	delete filters[2];
	int i;
	for (i=0;i<3;i++)
		if (set[i])
			info->session->erase_el(set[i]);
	if (rv)
		return rv;
	sc_addr sets[4];
	int cnt = 0;
	for (i=0;i<3;i++)
		if (orig_set[i])
			sets[cnt++]=orig_set[i];
	sets[cnt] = 0;
	scp_set_state(info->process,compute_select_state(info->session,sets));
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_selectNStr5)
{
	sc_addr orig_set[5];
	sc_addr set[5];
	sc_type mask[5];
	sc_single_filter *filters[5];
	sc_session *sess = info->session;
	sc_transaction *s = new sc_transaction(info->session);
	sc_iterator *iter = 0;
	sc_iterator_complex_filter<5> *filter;
	sc_retval rv;

	info->session = s;
	if (sel_prep_value(info,1,orig_set[0],set[0],mask[0],filters+0)) {
	__rollback:
		s->rollback();
		return RV_ERR_GEN;
	}
	if (sel_prep_value(info,2,orig_set[1],set[1],mask[1],filters+1))
		goto __rollback;
	if (sel_prep_value(info,3,orig_set[2],set[2],mask[2],filters+2))
		goto __rollback;
	if (sel_prep_value(info,4,orig_set[3],set[3],mask[3],filters+3))
		goto __rollback;
	if (sel_prep_value(info,5,orig_set[4],set[4],mask[4],filters+4))
		goto __rollback;
	s->commit();

	if (diag_output) {
		SCP_CHECK_OPND_EXCESSIVE(info, N6_);
		SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[6]);
	}

	info->session = sess;

	if (set[0])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_p1,
					set[0],mask[0],
					set[1],mask[1],
					set[2],mask[2],
					set[3],mask[3],
					set[4],mask[4]
				),true);
	else if (set[1])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1p2,
					mask[0],
					set[1],mask[1],
					set[2],mask[2],
					set[3],mask[3],
					set[4],mask[4]
				),true);
	else if (set[2])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1u2p3,
					mask[0],
					mask[1],
					set[2],mask[2],
					set[3],mask[3],
					set[4],mask[4]
				),true);
	else if (set[3])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1u2u3p4,
					mask[0],
					mask[1],
					mask[2],
					set[3],mask[3],
					set[4],mask[4]
				),true);
	else if (set[4])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1u2u3u4p5,
					mask[0],
					mask[1],
					mask[2],
					mask[3],
					set[4],mask[4]
				),true);
	else {
		rv = RV_ERR_GEN;
		goto __delete_sets;
	}

	filter = new sc_iterator_complex_filter<5>(iter,true);
	filter->set_filter(0,filters[0]);
	filters[0] = 0;
	filter->set_filter(1,filters[1]);
	filters[1] = 0;
	filter->set_filter(2,filters[2]);
	filters[2] = 0;
	filter->set_filter(3,filters[3]);
	filters[3] = 0;
	filter->set_filter(4,filters[4]);
	filters[4] = 0;

	for (;!filter->is_over();filter->next()) {
		if (orig_set[0])
			erase_occurences(sess,orig_set[0],filter->value(0));
		if (orig_set[1])
			erase_occurences(sess,orig_set[1],filter->value(1));
		if (orig_set[2])
			erase_occurences(sess,orig_set[2],filter->value(2));
		if (orig_set[3])
			erase_occurences(sess,orig_set[3],filter->value(3));
		if (orig_set[4])
			erase_occurences(sess,orig_set[4],filter->value(4));
	}

	delete filter;
	rv = RV_OK;
__delete_sets:
	delete filters[0];
	delete filters[1];
	delete filters[2];
	delete filters[3];
	delete filters[4];
	int i;
	for (i=0;i<5;i++)
		if (set[i])
			sess->erase_el(set[i]);
	if (rv)
		return rv;
	sc_addr sets[6];
	int cnt=0;
	for (i=0;i<5;i++) {
		if (orig_set[i])
			sets[cnt++] = orig_set[i];
	}
	sets[cnt] = 0;
	scp_set_state(info->process,compute_select_state(info->session,sets));
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_selectYStr3)
{
	sc_addr sets[3];
	sc_type mask[3];
	bool delete_set[3];
	sc_single_filter *filters[3];
	sc_session *sess = info->session;
	sc_transaction *s = new sc_transaction(info->session);
	sc_iterator *iter = 0;
	sc_iterator_complex_filter<3> *filter;
	sc_retval rv;
	std::set<sc_addr> toleave[3];
	int i;

	info->session = s;
	if (sel_prep_value_y(info,1,sets[0],mask[0],delete_set[0],filters+0)) {
	__rollback:
		s->rollback();
		return RV_ERR_GEN;
	}
	if (sel_prep_value_y(info,2,sets[1],mask[1],delete_set[1],filters+1))
		goto __rollback;
	if (sel_prep_value_y(info,3,sets[2],mask[2],delete_set[2],filters+2))
		goto __rollback;
	s->commit();

	if (diag_output) {
		SCP_CHECK_OPND_EXCESSIVE(info, N4_);
		SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[4]);
	}

	info->session = sess;

	if (sets[0])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely3_p1,
					sets[0],mask[0],
					sets[1],mask[1],
					sets[2],mask[2]
				),true);
	else if (sets[1])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely3_u1p2,
					mask[0],
					sets[1],mask[1],
					sets[2],mask[2]
				),true);
	else if (sets[2])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely3_u1u2p3,
					mask[0],
					mask[1],
					sets[2],mask[2]
				),true);
	else {
		rv = RV_ERR_GEN;
		goto __delete_sets;
	}

	filter = new sc_iterator_complex_filter<3>(iter,true);
	filter->set_filter(0,filters[0]);
	filters[0] = 0;
	filter->set_filter(1,filters[1]);
	filters[1] = 0;
	filter->set_filter(2,filters[2]);
	filters[2] = 0;

	for (;!filter->is_over();filter->next()) {
		if (sets[0])
			toleave[0].insert(filter->value(0));
		if (sets[1])
			toleave[1].insert(filter->value(1));
		if (sets[2])
			toleave[2].insert(filter->value(2));
	}

	for (i=0;i<3;i++) {
		if (!sets[i] || delete_set[i])
			continue;
		sc_iterator *iter = sess->create_iterator(sc_constraint_new(
					CONSTR_3_f_a_a,
					sets[i],
					SC_A_CONST,
					0
				),true);
		for (;!iter->is_over();iter->next()) {
			sc_addr val = iter->value(2);
			std::set<sc_addr>::iterator _iter = toleave[i].find(val);
			if (_iter != toleave[i].end())
				continue;
			sess->erase_el(iter->value(1));
		}
		delete iter;
	}

	delete filter;
	rv = RV_OK;
__delete_sets:
	delete filters[0];
	delete filters[1];
	delete filters[2];
	for (i=0;i<3;i++)
		if (delete_set[i])
			sess->erase_el(sets[i]);
	if (rv)
		return rv;
	sc_addr _sets[4];
	int cnt=0;
	for (i=0;i<3;i++)
		if (sets[i] && !delete_set[i])
			_sets[cnt++] = sets[i];
	_sets[cnt] = 0;
	scp_set_state(info->process,compute_select_state(info->session,_sets));
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_selectYStr5)
{
	sc_addr sets[5];
	sc_type mask[5];
	bool delete_set[5];
	sc_single_filter *filters[5];
	sc_session *sess = info->session;
	sc_transaction *s = new sc_transaction(info->session);
	sc_iterator *iter = 0;
	sc_iterator_complex_filter<5> *filter;
	sc_retval rv;
	std::set<sc_addr> toleave;
	int i;

	info->session = s;
	if (sel_prep_value_y(info,1,sets[0],mask[0],delete_set[0],filters+0)) {
	__rollback:
		s->rollback();
		return RV_ERR_GEN;
	}
	if (sel_prep_value_y(info,2,sets[1],mask[1],delete_set[1],filters+1))
		goto __rollback;
	if (sel_prep_value_y(info,3,sets[2],mask[2],delete_set[2],filters+2))
		goto __rollback;
	if (sel_prep_value_y(info,4,sets[3],mask[3],delete_set[3],filters+3))
		goto __rollback;
	if (sel_prep_value_y(info,5,sets[4],mask[4],delete_set[4],filters+4))
		goto __rollback;
	s->commit();
	if (diag_output) {
		SCP_CHECK_OPND_EXCESSIVE(info, N6_);
		SCP_CHECK_OPND_EXCESSIVE(info, pm_num_set_attrs[6]);
	}
	info->session = sess;

	if (sets[0])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_p1,
					sets[0],mask[0],
					sets[1],mask[1],
					sets[2],mask[2],
					sets[3],mask[3],
					sets[4],mask[4]
				),true);
	else if (sets[1])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1p2,
					mask[0],
					sets[1],mask[1],
					sets[2],mask[2],
					sets[3],mask[3],
					sets[4],mask[4]
				),true);
	else if (sets[2])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1u2p3,
					mask[0],
					mask[1],
					sets[2],mask[2],
					sets[3],mask[3],
					sets[4],mask[4]
				),true);
	else if (sets[3])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1u2u3p4,
					mask[0],
					mask[1],
					mask[2],
					sets[3],mask[3],
					sets[4],mask[4]
				),true);
	else if (sets[4])
		iter = sess->create_iterator(sc_constraint_new(
					CONSTR_sely5_u1u2u3u4p5,
					mask[0],
					mask[1],
					mask[2],
					mask[3],
					sets[4],mask[4]
				),true);
	else {
		rv = RV_ERR_GEN;
		goto __delete_sets;
	}

	filter = new sc_iterator_complex_filter<5>(iter,true);
	filter->set_filter(0,filters[0]);
	filters[0] = 0;
	filter->set_filter(1,filters[1]);
	filters[1] = 0;
	filter->set_filter(2,filters[2]);
	filters[2] = 0;
	filter->set_filter(3,filters[3]);
	filters[3] = 0;
	filter->set_filter(4,filters[4]);
	filters[4] = 0;

	for (;!filter->is_over();filter->next()) {
		if (sets[0])
			toleave.insert(filter->value(0));
		if (sets[1])
			toleave.insert(filter->value(1));
		if (sets[2])
			toleave.insert(filter->value(2));
		if (sets[3])
			toleave.insert(filter->value(3));
		if (sets[4])
			toleave.insert(filter->value(4));
	}

	for (i=0;i<5;i++) {
		if (!sets[i] || delete_set[i])
			continue;
		sc_iterator *iter = sess->create_iterator(sc_constraint_new(
					CONSTR_3_f_a_a,
					sets[i],
					SC_A_CONST,
					0
				),true);
		for (;!iter->is_over();iter->next()) {
			sc_addr val = iter->value(2);
			std::set<sc_addr>::iterator iter2 = toleave.find(val);
			if (iter2 != toleave.end())
				continue;
			sess->erase_el(iter->value(1));
		}
		delete iter;
	}

	delete filter;
	rv = RV_OK;
__delete_sets:
	delete filters[0];
	delete filters[1];
	delete filters[2];
	delete filters[3];
	delete filters[4];
	for (i=0;i<5;i++)
		if (delete_set[i])
			sess->erase_el(sets[i]);
	if (rv)
		return rv;
	sc_addr _sets[6];
	int cnt = 0;
	for (i=0;i<5;i++)
		if (sets[i] && !delete_set[i])
			_sets[cnt++] = sets[i];
	_sets[cnt] = 0;
	scp_set_state(info->process,compute_select_state(info->session,_sets));
	return RV_OK;
}
