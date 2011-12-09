
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

using std::list;
using std::pair;

typedef pair <sc_addr, sc_segment *> parse_pair;

typedef list <parse_pair> parse_list;

sc_retval parse_package(sc_segment *seg, const sc_string &place) 
{
	sc_session *s = libsc_login();
	sc_retval rv;
	sc_segment *the_interface;
	sc_segment *implementation;
	s->open_segment(seg->get_full_uri());

	if ((rv = s->_stat(place)) == RV_OK) {
		s->close();
		return RV_ERR_GEN;
	}

	if (RV_IS_ERR(rv)) {
		if (!(the_interface = create_segment_full_path(s, place + "/the_interface"))) {
			return RV_ERR_GEN;
		} 
	} else if (!RV_IS_ERR(s->_stat(place + "/the_interface"))) {
		s->close();
		return RV_ERR_GEN;
	} else if (!(the_interface = s->create_segment(place + "/the_interface"))) {
		s->close();
		return RV_ERR_GEN;
	}

	if (!RV_IS_ERR(s->_stat(place + "/implementation"))) {
		s->unlink(place + "/the_interface");
		s->close();
		return RV_ERR_GEN;
	} else if (!(implementation = s->create_segment(place + "/implementation"))) {
		s->unlink(place + "/the_interface");
		s->unlink(place + "/implementation");
		s->close();
		return RV_ERR_GEN;
	}

	// stage 0.5 . open all segments links point to
	{
		sc_segment::iterator *iter = seg->create_link_iterator();
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();
			sc_string str = seg->get_link_target(ll);
			s->open_segment(dirname(str));
		}
		delete iter;
	}

	parse_list methods;
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, SCP_PROGRAM, 0, 0), true);
	for (; !iter->is_over(); iter->next()) {
		sc_addr prg = iter->value(2);
		sc_string uri = place + "/consts_" + s->get_idtf(prg);
		if (!s->_stat(uri)) {
			continue;
		} 
		
		sc_segment *seg = s->create_segment(uri);
		
		if (!seg) {
			SC_ABORT();
		}

		methods.push_back(parse_pair(prg, seg));
	}
	delete iter;
	//stage 2. separate program signs
	parse_list::iterator i = methods.begin();
	for (; i != methods.end(); i++) {
		sc_addr arc;
		i->first = s->__move_element(i->first, the_interface);
		if (search_oneshot(s, sc_constraint_new(CONSTR_3_f_a_f, INIT, 0, i->first), true, 1, 1, &arc)) {
			continue;
		} 
		
		s->__move_element(arc, the_interface);
	}
	//stage 3. separate program's consts
	i = methods.begin();
	for (; i != methods.end(); i++) {
		sc_addr prg = i->first;
		sc_segment *toseg = i->second;
		sc_addr consts;
		if (search_oneshot(s, sc_constraint_new(CONSTR_5_f_a_a_a_f, prg, 0, SC_N_CONST, 0, CONST_), true, 1, 2, &consts)) {
			printf("Cannot find consts of program %s\n", s->get_idtf(prg).c_str());
			return RV_ERR_GEN;
		}
		iter = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, consts, 0, 0), true);
		for (; !iter->is_over(); iter->next()) {
			sc_addr val = iter->value(2);
			if (val->seg != seg) {
				continue;
			} 
			
			if (!s->__move_element(val, toseg)) {
				SC_ABORT();
			} 
		}
		delete iter;
	}
	// stage 3. move the rest to implementation
	iter = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, seg, false), true);
	for (; !iter->is_over(); iter->next()) {
		if (!s->__move_element(iter->value(0), implementation)) {
			SC_ABORT();
		}
	} 
	
	delete iter;
	s->unlink(seg->get_full_uri());
	s->close();
	return RV_OK;
}

sc_addr spawn_package(const sc_string &place) 
{
	sc_session *s = libsc_login();
	if (!s->open_segment(INIT->seg->get_full_uri())) {
		SC_ABORT();
	} 
	
	if (!s->open_segment(place + "/the_interface")) {
__e_ret: 
		s->close();
		return 0;
	}

	if (!s->open_segment(place + "/implementation")) {
		goto __e_ret;
	} 
	
	sc_addr prg;
	if (search_oneshot(s, sc_constraint_new(CONSTR_3_f_a_a, INIT, 0, 0), true, 1, 2, &prg)) {
		goto __e_ret;
	} 
	
	sc_addr proc;
	s->close_segment(place + "/implementation");
	if (scp_create_process(s, prg, 0, 0, &proc)) {
		goto __e_ret;
	} 
	
	if (pm_sched_add_agent(proc, SCHED_CLASS_NORMAL)) {
		fprintf(stderr, "Cannot start process\n");
		goto __e_ret;
	}

	return proc;
}

sc_addr spawn_package_ex(const sc_string &place, sc_addr owner_agent) {
	sc_session *s = libsc_login();
	if (!s->open_segment(INIT->seg->get_full_uri())) {
		SC_ABORT();
	} 
	
	if (!s->open_segment(place + "/the_interface")) {
__e_ret: 
		s->close();
		return 0;
	}
	
	if (!s->open_segment(place + "/implementation")) {
		goto __e_ret;
	} 
	
	sc_addr prg;
	if (search_oneshot(s, sc_constraint_new(CONSTR_3_f_a_a, INIT, 0, 0), true, 1, 2, &prg)) {
		goto __e_ret;
	} 
	
	sc_addr proc;
	s->close_segment(place + "/implementation");
	if (scp_create_process(s, prg, 0, 0, &proc)) {
		goto __e_ret;
	} 
	
	if (pm_sched_add_agent(proc, SCHED_CLASS_NORMAL)) {
		fprintf(stderr, "Cannot start process\n");
		goto __e_ret;
	}

	return proc;
}

sc_session *__system;

void scp_package_init(sc_session *_system) 
{
	__system = _system;
}

/**
 * Запускает scp-пакет с uri @place.
 */
sc_retval run_package(const sc_string &place) 
{
	sc_addr process = spawn_package(place);
	if (!process) {
		return RV_ERR_GEN;
	} 

	while (!scp_check_state(__system, process, PROCESS_STATE_DEAD)) {
		if (!pm_sched_do_step()) {
			return RV_ELSE_GEN;
		} 
	}
		
	__system->activate(process, 0);
	return RV_OK;
}

static
void add_to_set_arcs_from(sc_session *s,
							 sc_addr el,
							 std::set<sc_addr> &a_set,
							 sc_addr from)
{
	sc_iterator *it;
	it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_f,
		from,
		0,
		el
		),true);
	for (;!it->is_over();it->next())
		a_set.insert(it->value(1));
	delete it;
}

static
void add_to_set_classic_tuple_arcs(sc_session *s,
									  sc_addr tuple,
									  std::set<sc_addr> &a_set)
{
	sc_retval rv;
	for (int i = 1;i<=32;i++) {
		sc_addr arc,arc2;
		rv = search_oneshot(s,sc_constraint_new(CONSTR_5_f_a_a_a_f,
			tuple,
			0,
			0,
			0,
			pm_numattr[i]
		),true,2,1,&arc,3,&arc2);
		if (rv)
			break;
		a_set.insert(arc);
		a_set.insert(arc2);
	}
}

/**
 * Заносит знаки элементов, составляющих текст scp-программы, в a_set и operators.
 *
 * @param *s sc-сессия.
 * @param program знак программы.
 * @param[out] &a_set множество всех знаков элементов, составляющих текст scp-программы.
 * @param[out] *operators множество всех знаков операторов. Может быть NULL. 
 * @return код возврата.
 */
sc_retval scp_package_locate_program_text(sc_session *s, sc_addr program, std::set<sc_addr> &a_set, std::set<sc_addr> *operators) 
{
	// we don't have defines for SCP operators. Have to check by idtf
	sc_segment *scp_keynodes_seg = s->open_segment("/proc/keynode");
	sc_addr a, a2, a3;
	sc_retval rv;
	sc_iterator *it;
	sc_addr variables;
	sc_addr constants;
	sc_addr parameters;
	typedef std::deque < sc_addr > mydeque;
	mydeque deque;
	int i;

	// find and add variables set
	rv = search_oneshot(s, sc_constraint_new(CONSTR_5_f_a_a_a_f, program, 0, 0, 0, VAR_), true, 3, 1, &a, 2, &variables, 3, &a3);
	if (rv) {
		return RV_ERR_GEN;
	} 
	a_set.insert(a);
	a_set.insert(variables);
	a_set.insert(a3);
	// find and add constants set
	rv = search_oneshot(s, sc_constraint_new(CONSTR_5_f_a_a_a_f, program, 0, 0, 0, CONST_), true, 3, 1, &a, 2, &constants, 3, &a3);
	if (rv) {
		return RV_ERR_GEN;
	} 
	a_set.insert(a);
	a_set.insert(constants);
	a_set.insert(a3);
	// locate and add operators
	// we have to do it first 'cause operators may be in constants set
	// moreover operators may be in variables set
	// find first operator
	rv = search_oneshot(s, sc_constraint_new(CONSTR_5_f_a_a_a_f, program, 0, 0, 0, INIT_), true, 3, 1, &a, 2, &a2, 3, &a3);
	if (rv) {
		return RV_ERR_GEN;
	} 
	a_set.insert(a);
	a_set.insert(a2);
	a_set.insert(a3);
	deque.push_back(a2);
	while (!deque.empty()) {
		sc_addr op = *(deque.begin());
		sc_addr op_type;
		// add op to operators if needed
		if (operators) {
			operators->insert(op);
		} 
		deque.pop_front();
		// find all goto_, then_, and else_ successors
		a = sc_tup::at(s, op, GOTO_);
		if (a && a_set.find(a) == a_set.end()) {
			a_set.insert(a);
			deque.push_back(a);
		}
		a = sc_tup::at(s, op, THEN_);
		if (a && a_set.find(a) == a_set.end()) {
			a_set.insert(a);
			deque.push_back(a);
		}
		a = sc_tup::at(s, op, ELSE_);
		if (a && a_set.find(a) == a_set.end()) {
			a_set.insert(a);
			deque.push_back(a);
		}
		// add arc from operator type
		rv = search_oneshot(s, sc_constraint_new(CONSTR_3l2_f_a_a_a_f, SCP_OPERATOR_TYPE, 0, 0, 0, op), true, 2, 3, &a, 2, &op_type);
		if (rv) {
			return RV_ERR_GEN;
		} 
		a_set.insert(a);
		// add all arcs from operator with their "important" attribute arcs
		it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, op, 0, 0), true);
		for (; !it->is_over(); it->next()) {
			a = it->value(1);
			a_set.insert(a);
			for (int i = 1; i <= 16; i++) {
				add_to_set_arcs_from(s, a, a_set, pm_numattr[i]);
				add_to_set_arcs_from(s, a, a_set, pm_num_set_attrs[i]);
				add_to_set_arcs_from(s, a, a_set, pm_num_segc_attrs[i]);
			}
			add_to_set_arcs_from(s, a, a_set, CONST_);
			add_to_set_arcs_from(s, a, a_set, VAR_);
			add_to_set_arcs_from(s, a, a_set, METAVAR_);
			add_to_set_arcs_from(s, a, a_set, POS_);
			add_to_set_arcs_from(s, a, a_set, FUZ_);
			add_to_set_arcs_from(s, a, a_set, NEG_);
			add_to_set_arcs_from(s, a, a_set, ARC_);
			add_to_set_arcs_from(s, a, a_set, NODE_);
			add_to_set_arcs_from(s, a, a_set, UNDF_);
			add_to_set_arcs_from(s, a, a_set, PERMANENT_);
			add_to_set_arcs_from(s, a, a_set, TEMPORARY_);
			add_to_set_arcs_from(s, a, a_set, ACTUAL_);
			add_to_set_arcs_from(s, a, a_set, PHANTOM_);
			add_to_set_arcs_from(s, a, a_set, GOTO_);
			add_to_set_arcs_from(s, a, a_set, THEN_);
			add_to_set_arcs_from(s, a, a_set, ELSE_);
			add_to_set_arcs_from(s, a, a_set, FIXED_);
			add_to_set_arcs_from(s, a, a_set, ASSIGN_);
		}
		delete it;
		// now lets consider some operators specifics
		if (op_type->seg != scp_keynodes_seg) {
			continue;
		} 
		
		if (s->get_idtf(op_type) == "call") {
			sc_addr op2 = sc_tup::at(s, op, N2_);
			// call's operand 2 is really classic tuple
			/*if (!search_oneshot(s,sc_constraint_new(CONSTR_3_f_a_f,
			constants,
			0,
			op2
			),true,0))*/
			a_set.insert(op2);
			add_to_set_classic_tuple_arcs(s, op2, a_set);
		} else if (s->get_idtf(op_type) == "sys_set_event_handler") {
			// 3rd operand for set_event_handler is a tuple
			sc_addr op3 = sc_tup::at(s, op, N3_);
			if (!search_oneshot(s, sc_constraint_new(CONSTR_3_f_a_f, constants, 0, op3), true, 0)) {
				add_to_set_classic_tuple_arcs(s, op3, a_set);
			} 
		} else if (s->get_idtf(op_type) == "sys_wait") {
			// 1st operand of sys_wait is a classic relation
			sc_addr op1 = sc_tup::at(s, op, N1_);
			if (search_oneshot(s, sc_constraint_new(CONSTR_3_f_a_f, constants, 0, op1), true, 0)) {
				continue;
			} it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, op1, 0, 0), true);
			for (; !it->is_over(); it->next()) {
				a_set.insert(it->value(1));
				add_to_set_classic_tuple_arcs(s, it->value(2), a_set);
			}
			delete it;
		}
		//out_specifics:
	}
	// add {programSCP,init} arcs
	add_to_set_arcs_from(s, program, a_set, SCP_PROGRAM);
	add_to_set_arcs_from(s, program, a_set, INIT);
	a_set.insert(program);
	// find and add parameters tuple
	rv = search_oneshot(s, sc_constraint_new(CONSTR_5_f_a_a_a_f, program, 0, 0, 0, PRM_), true, 3, 1, &a, 2, &parameters, 3, &a3);
	if (!rv) {
		a_set.insert(a);
		a_set.insert(parameters);
		a_set.insert(a3);
		// add parameters arcs
		for (i = 1; i <= 32; i++) {
			sc_addr arc, arc2;
			rv = search_oneshot(s, sc_constraint_new(CONSTR_5_f_a_a_a_f, parameters, 0, 0, 0, pm_numattr[i]), true, 2, 1, &arc, 3, &arc2);
			if (rv) {
				break;
			} 
			a_set.insert(arc);
			a_set.insert(arc2);
			add_to_set_arcs_from(s, arc, a_set, IN_);
			add_to_set_arcs_from(s, arc, a_set, OUT_);
		}
	}
	// add all variables
	it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, variables, 0, 0), true);
	for (; !it->is_over(); it->next()) {
		a_set.insert(it->value(1));
		a_set.insert(it->value(2));
	}
	delete it;
	// add all constant's arcs
	it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, constants, 0, 0), true);
	for (; !it->is_over(); it->next()) {
		a_set.insert(it->value(1));
	} 
	delete it;
	return RV_OK;
}

struct program_copy_cb_data 
{
	sc_addr program;
	sc_addr program_copy;
};

#include "scp_debugger.h"

sc_retval scp_package_run_program(sc_session *parent, sc_addr program, sc_addr parent_process, sc_addr params, sc_addr *process_) 
{
	assert(parent && "SC-session must present");
	assert(program && "SCP-program must present");

	sc_session *s = parent->__fork();

	sc_addr process = 0;
	sc_retval rv = scp_create_process(s, program, parent_process, params, &process);
	if (rv) {
		// TODO: leak
		return rv;
	} 

	pm_sched_add_agent(process, SCHED_CLASS_NORMAL);
	
	if (scp_debugger::get_instance())
		scp_debugger::get_instance()->suspend(process);
	
	if (process_)
		*process_ = process;
	
	return RV_OK;
}
