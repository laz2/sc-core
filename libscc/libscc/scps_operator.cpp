/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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

#include "scps_operator.h"
#include "scps_program.h"

using namespace scc;

void scc::move_input_branch(sc_addr from, sc_addr to, sc_addr attr)
{
	bool scps = true;
	sc_iterator *it = system_session->create_iterator(sc_constraint_new(
		CONSTR_5_a_a_f_a_f,
		0,
		0,
		from,
		0,
		attr), true);
	sc_for_each (it) system_session->set_end(it->value(1), to);
}

void scc::move_input_branches(sc_addr from, sc_addr to)
{
	move_input_branch(from, to, GOTO_);
	move_input_branch(from, to, THEN_);
	move_input_branch(from, to, ELSE_);
}

custom_operator::custom_operator(program *_prg)
	: code_block("custom_operator"), s(_prg->get_session()),
	wseg(_prg->get_segment()), prg(_prg), type(0),
	type_arc(0), then_blk(0), else_blk(0), goto_blk(0),
	immutable_goto(false), immutable_then(false), immutable_else(false)
{
	sign = s->create_el(wseg, SC_N_CONST);
	s->set_idtf(sign, prg->new_op_id());
}

void custom_operator::set_type(sc_addr new_type)
{
	SC_ASSERTS(!new_type || sc_set::is_in(s, new_type, SCP_OPERATOR_TYPE),
		"'" << SC_URI(new_type) << "' isn't scp-operator type");

	if (new_type) {
		if (type) {
			s->set_beg(type_arc, new_type);
		} else {
			type_arc = sc_set::include_in(s, sign, new_type);
		}
		type = new_type;
	} else {
		if (type)
			s->erase_el(type_arc);
		type_arc = SCADDR_NIL;
		type = SCADDR_NIL;
	}
}

void custom_operator::print(std::ostream &os, size_t level)
{
	print_indent(os, level);

	os << s->get_idtf(type)
		<< " -> " << s->get_idtf(sign)
		<< " = { ";

	bool has_prev = false;

	sc_addr branch = sc_tup::at(s, sign, GOTO_);
	if (branch) {
		os << s->get_idtf(GOTO_) << ": " << s->get_idtf(branch);
		has_prev = true;
	}

	branch = sc_tup::at(s, sign, THEN_);
	if (branch) {
		if (has_prev) os << ", ";
		os << s->get_idtf(THEN_) << ": " << s->get_idtf(branch);
		has_prev = true;
	}

	branch = sc_tup::at(s, sign, ELSE_);
	if (branch) {
		if (has_prev) os << ", ";
		os << s->get_idtf(ELSE_) << ": " << s->get_idtf(branch);
	}

	os << " };";
}

void custom_operator::add_opnd(const sc_attributes *attrs, sc_addr opnd)
{
	SC_ASSERT(attrs);
	SC_ASSERT(opnd);

	//
	// Check branch opnd with attribute goto_, then_, else_.
	//
	if (attrs->has_const(GOTO_)) {
		// TODO: error if already has setted goto_ branch
		// TODO: error if then_ and else_ attributes presented
		
	}
}

void custom_operator::set_then(code_block *blk)
{
	if (!immutable_goto && !immutable_then) {
		then_blk = blk;
		if (blk) {
			set_then(blk->get_first_scp_op());
		} else {
			set_then(SCADDR_NIL);
		}
	}
}

void custom_operator::set_else(code_block *blk)
{
	if (!immutable_goto && !immutable_else) {
		else_blk = blk;
		if (blk) {
			set_else(blk->get_first_scp_op());
		} else {
			set_else(SCADDR_NIL);
		}
	}
}

void custom_operator::set_goto(code_block *blk)
{
	if (!immutable_goto && !immutable_then && !immutable_else) {
		goto_blk = blk;
		if (blk) {
			set_goto(blk->get_first_scp_op());
		} else {
			set_goto(SCADDR_NIL);
		}
	}
}

void custom_operator::set_then(sc_addr op)
{
	sc_addr arc = 0;

	if (sc_tup::at(s, sign, THEN_, &arc))
		s->erase_el(arc);

	if (sc_tup::at(s, sign, GOTO_, &arc))
		s->erase_el(arc);

	if (op)
		sc_tup::add_c(s, sign, THEN_, op);
}

void custom_operator::set_else(sc_addr op)
{
	sc_addr arc = 0;

	if (sc_tup::at(s, sign, ELSE_, &arc))
		s->erase_el(arc);

	if (sc_tup::at(s, sign, GOTO_, &arc))
		s->erase_el(arc);

	if (op)
		sc_tup::add_c(s, sign, ELSE_, op);
}

void custom_operator::set_goto(sc_addr op)
{
	sc_addr arc = 0;

	if (sc_tup::at(s, sign, THEN_, &arc))
		s->erase_el(arc);

	if (sc_tup::at(s, sign, ELSE_, &arc))
		s->erase_el(arc);

	if (sc_tup::at(s, sign, GOTO_, &arc))
		s->erase_el(arc);

	if (op)
		sc_tup::add_c(s, sign, GOTO_, op);
}

call_operator::call_operator(program *_prg) : custom_operator(_prg)
{
	set_type(s->find_by_idtf("callReturn", operators_segment));
}
