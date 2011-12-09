
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

#include "sc_queue.h"

static sc_addr pm_link_new(sc_session *s, sc_segment *seg) 
{
	sc_addr link = s->create_el(seg, SC_N_CONST);
	if (s->get_error()) {
		return 0;
	} 
	
	s->gen5_f_a_f_a_f(link, 0, seg, SC_A_CONST|SC_POS, link, 0, seg, SC_A_CONST|SC_POS, QUEUE_PREV_);
	if (s->get_error()) {
		return 0;
	} 
	
	s->gen5_f_a_f_a_f(link, 0, seg, SC_A_CONST|SC_POS, link, 0, seg, SC_A_CONST|SC_POS, QUEUE_NEXT_);
	if (s->get_error()) {
		return 0;
	}

	s->gen5_f_a_f_a_f(link, 0, seg, SC_A_CONST|SC_POS, QUEUE_NIL, 0, seg, SC_A_CONST|SC_POS, QUEUE_VALUE_);
	if (s->get_error()) {
		return 0;
	} 
	
	return link;
}

static sc_retval pm_link_get_attr(sc_session *s, sc_addr queue, sc_addr attr, sc_addr &value) 
{
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, queue, SC_A_CONST|SC_POS, 0, SC_A_CONST|SC_POS, attr), true);
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	value = iter->value(2);
	delete iter;
	return RV_OK;
}

static sc_retval pm_link_set_attr(sc_session *s, sc_addr queue, sc_addr attr, sc_addr val) 
{
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, queue, SC_A_CONST|SC_POS, 0, SC_A_CONST|SC_POS, attr), true);
	if (iter->is_over()) {
		delete iter;
		return RV_ERR_GEN;
	}
	sc_addr arc = iter->value(1);
	delete iter;
	s->set_end(arc, val);
	return RV_OK;
}

sc_retval pm_link_append(sc_session *s, sc_addr link, sc_addr link2) 
{
	sc_retval rv;
	sc_addr next;
	if ((rv = pm_link_get_attr(s, link, QUEUE_NEXT_, next))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, next, QUEUE_PREV_, link2))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link, QUEUE_NEXT_, link2))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link2, QUEUE_PREV_, link))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link2, QUEUE_NEXT_, next))) {
		return rv;
	} 
	
	return RV_OK;
}

static sc_retval pm_link_prepend(sc_session *s, sc_addr link, sc_addr link2) 
{
	sc_retval rv;
	sc_addr prev;
	if ((rv = pm_link_get_attr(s, link, QUEUE_PREV_, prev))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, prev, QUEUE_NEXT_, link2))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link, QUEUE_PREV_, link2))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link2, QUEUE_NEXT_, link))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link2, QUEUE_PREV_, prev))) {
		return rv;
	} 
	
	return RV_OK;
}

static sc_retval pm_link_remove(sc_session *s, sc_addr link) 
{
	sc_retval rv;
	sc_addr next, prev;
	if ((rv = pm_link_get_attr(s, link, QUEUE_PREV_, prev))) {
		return rv;
	} 
	
	if ((rv = pm_link_get_attr(s, link, QUEUE_NEXT_, next))) {
		return rv;
	} 
	
	if (prev == link) {
		assert(next == link);
	} else {
		if ((rv = pm_link_set_attr(s, prev, QUEUE_NEXT_, next))) {
			return rv;
		} if ((rv = pm_link_set_attr(s, next, QUEUE_PREV_, prev))) {
			return rv;
		} 
	}
	if ((rv = pm_link_set_attr(s, link, QUEUE_PREV_, link))) {
		return rv;
	} 
	
	if ((rv = pm_link_set_attr(s, link, QUEUE_NEXT_, link))) {
		return rv;
	} 
	
	return RV_OK;
}

static sc_retval pm_link_delete(sc_session *s, sc_addr link) 
{
	if (pm_link_remove(s, link)) {
		return RV_ERR_GEN;
	} 
	return s->erase_el(link);
}

sc_retval sc_queue_new(sc_session *_session, sc_addr &queue, sc_segment *seg) 
{
	sc_addr listhead;
	sc_transaction *session = new sc_transaction(_session);
	queue = session->create_el(seg, SC_N_CONST);
	if (session->get_error()) {
		goto __rollback;
	}

	listhead = pm_link_new(session, seg);
	if (!listhead) {
		goto __rollback;
	} 
	session->gen5_f_a_f_a_f(queue, 0, seg, SC_A_CONST|SC_POS, listhead, 0, seg, SC_A_CONST|SC_POS, QUEUE_HEAD_);
	
	if (session->get_error()) {
		goto __rollback;
	} 
	session->commit();
	return RV_OK;
	
__rollback: 
	sc_retval rv = session->get_error();
	session->rollback();
	return rv;
}

sc_retval sc_queue_delete(sc_session *session, sc_addr queue) 
{
	sc_transaction *s = new sc_transaction(session);
	sc_addr curr;
	sc_addr listhead;
	if (search_5_f_cpa_cna_cpa_f(s, queue, 0, &listhead, 0, QUEUE_HEAD_)) {
		goto __rollback;
	} 
	
	if (s->erase_el(queue)) {
		goto __rollback;
	} 
	
	if (pm_link_get_attr(s, listhead, QUEUE_NEXT_, curr)) {
		goto __rollback;
	} 

	while (curr != listhead) {
		sc_addr next;
		
		if (pm_link_get_attr(s, listhead, QUEUE_NEXT_, next)) {
			goto __rollback;
		} 
		
		if (pm_link_remove(s, curr)) {
			goto __rollback;
		} 
		
		if (pm_link_delete(s, curr)) {
			goto __rollback;
		} 
		
		curr = next;
	}
	if (pm_link_delete(s, listhead)) {
		goto __rollback;
	} 
	
	s->commit();
	return RV_OK;
__rollback: 
	s->rollback();
	return RV_ERR_GEN;
}

sc_retval sc_queue_append(sc_session *_session, sc_addr queue, sc_addr element) 
{
	sc_transaction *s = new sc_transaction(_session);
	sc_segment *seg = queue->seg;
	sc_addr listhead;
	sc_addr link = pm_link_new(s, seg);
	if (!link) {
		goto __rollback;
	} 
	
	if (s->gen3_f_a_f(queue, 0, seg, SC_A_CONST|SC_POS, link)) {
		goto __rollback;
	} 
	
	if (pm_link_set_attr(s, link, QUEUE_VALUE_, element)) {
		goto __rollback;
	} 
	
	if (search_5_f_cpa_cna_cpa_f(s, queue, 0, &listhead, 0, QUEUE_HEAD_)) {
		goto __rollback;
	} 
	
	if (pm_link_prepend(s, listhead, link)) {
		goto __rollback;
	} 
	
	s->commit();
	return RV_OK;
__rollback: 
	s->rollback();
	return RV_ERR_GEN;
}

sc_retval sc_queue_prepend(sc_session *_s, sc_addr queue, sc_addr element) 
{
	sc_transaction *s = new sc_transaction(_s);
	sc_segment *seg = queue->seg;
	sc_addr listhead;
	sc_addr link = pm_link_new(s, seg);
	if (!link) {
		goto __rollback;
	} 
	
	if (s->gen3_f_a_f(queue, 0, seg, SC_A_CONST|SC_POS, link)) {
		goto __rollback;
	} 
	
	if (pm_link_set_attr(s, link, QUEUE_VALUE_, element)) {
		goto __rollback;
	} 
	
	if (search_5_f_cpa_cna_cpa_f(s, queue, 0, &listhead, 0, QUEUE_HEAD_)) {
		goto __rollback;
	} 
	
	if (pm_link_append(s, listhead, link)) {
		goto __rollback;
	} 
	
	s->commit();
	return RV_OK;
__rollback: 
	s->rollback();
	return RV_ERR_GEN;
}

sc_retval sc_queue_peek(sc_session *session, sc_addr queue, sc_addr &element) 
{
	sc_addr first, listhead;
	element = 0;
	if (search_5_f_cpa_cna_cpa_f(session, queue, 0, &listhead, 0, QUEUE_HEAD_)) {
		return RV_ERR_GEN;
	} 
	
	if (pm_link_get_attr(session, listhead, QUEUE_NEXT_, first)) {
		return RV_ERR_GEN;
	} 
	
	if (first == listhead) {
		return RV_ELSE_GEN;
	} 
	
	if (pm_link_get_attr(session, first, QUEUE_VALUE_, element)) {
		return RV_ERR_GEN;
	} 
	
	return RV_OK;
}

sc_retval sc_queue_get(sc_session *session, sc_addr queue, sc_addr &element) {
	sc_addr first, listhead;
	element = 0;
	if (search_5_f_cpa_cna_cpa_f(session, queue, 0, &listhead, 0, QUEUE_HEAD_)) {
		return RV_ERR_GEN;
	} 
	
	if (pm_link_get_attr(session, listhead, QUEUE_NEXT_, first)) {
		return RV_ERR_GEN;
	} 
	
	if (first == listhead) {
		return RV_ELSE_GEN;
	} 
	
	if (pm_link_get_attr(session, first, QUEUE_VALUE_, element)) {
		return RV_ERR_GEN;
	} 
	
	sc_transaction *safe = new sc_transaction(session);
	if (pm_link_delete(safe, first)) {
		safe->rollback();
		return RV_ERR_GEN;
	}
	safe->commit();
	return RV_OK;
}

sc_retval sc_queue_remove(sc_session *ses, sc_addr queue, sc_addr element) 
{
	sc_addr link;
	if (search_oneshot(ses, sc_constraint_new(CONSTR_3l2_f_a_a_a_f, queue, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, element), true, 1, 2, &link)) {
		return RV_ERR_GEN;
	} 
	
	sc_transaction *s = new sc_transaction(ses);
	if (pm_link_delete(s, link)) {
		s->rollback();
		return RV_ERR_GEN;
	}
	s->commit();
	return RV_OK;
}
