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

#include "scps_compiler.h"
#include "scps_program.h"

using namespace scc;

program::program(sc_session *_s, sc_segment *_seg, names_scope *_scope, sc_addr _sign)
	: sc_element(_sign), s(_s), wseg(_seg), root_scope(_scope), main_block(0), cur_op_id(0)
{
	if (!sign_)
		sign_ = s->create_el(wseg, SC_N_CONST);
	sc_set::include_in(s, sign_, SCP_PROGRAM);

	consts = s->create_el(wseg, SC_N_CONST);
	vars = s->create_el(wseg, SC_N_CONST);
	prms = s->create_el(wseg, SC_N_CONST);
	sc_tup::add(s, sign_, CONST_, consts);
	sc_tup::add(s, sign_, VAR_, vars);
	sc_tup::add(s, sign_, PRM_, prms);
}

void program::set_main_block(code_block *blk)
{
	if (main_block)
		sc_set::exclude_from(s, main_block->get_first_scp_op(), sign_);

	main_block = blk;
	if (main_block) {
		SC_ASSERTS(main_block->get_first_scp_op(), "main_block is empty.");
		sc_tup::add_c(s, sign_, INIT_, main_block->get_first_scp_op());
	}
}

void program::add_prm(sc_addr prm, int prm_spec)
{
	add_var(prm);

	sc_addr arc = sc_set::include_in(s, prm, prms);
	if (prm_spec & IN_PRM)
		sc_set::include_in(s, arc, IN_);
	if (prm_spec & OUT_PRM)
		sc_set::include_in(s, arc, OUT_);

	prms_cache.push_back(std::make_pair(prm, prm_spec));
}

void program::add_prm(sc_addr prm, const sc_attributes &spec_attrs)
{
	int prm_spec = 0;
	if (spec_attrs.has_const(IN_))
		prm_spec |= IN_PRM;
	if (spec_attrs.has_const(OUT_))
		prm_spec |= OUT_PRM;
	add_prm(prm, prm_spec);
}

namespace scc
{
	class label_block : public code_block
	{
	public:
		label_block(sc_session *_s, sc_segment *_wseg, const sc_string &_lbl)
			: code_block(_lbl), s(_s), wseg(_wseg), blk(0), dummy_op(0)
		{
			SC_ASSERT(s);
			SC_ASSERT(wseg);
		}

		~label_block()
		{
			if (dummy_op && !dummy_op->is_dead())
				s->erase_el(dummy_op);
		}

		sc_addr get_first_scp_op() const
		{
			if (blk) {
				return blk->get_first_scp_op();
			} else {
				if (!dummy_op)
					dummy_op = s->create_el(wseg, SC_N_CONST);
				return dummy_op;
			}
		}

		code_block* get_then() const
		{ return 0; }

		code_block* get_else() const
		{ return 0; }

		code_block* get_goto() const
		{ return blk; }

		void set_then(code_block *_blk)
		{ SCPS_THROW_ERROR_BF("Try to set then_ branch of label_block %1%.", get_name()); }

		void set_else(code_block *_blk)
		{ SCPS_THROW_ERROR_BF("Try to set else_ branch of label_block %1%.", get_name()); }

		void set_goto(code_block *_blk)
		{
			SC_ASSERT(_blk);
			SC_ASSERT(!blk && "Next block for label already setted.");

			sc_merge_elements(s, _blk->get_first_scp_op(), get_first_scp_op());
			blk = _blk;

			s->erase_el(dummy_op);
			dummy_op = SCADDR_NIL;
		}

		void add(code_block *blk)
		{ SCPS_THROW_ERROR_BF("Try to add subblock to label_block %1%.", get_name()); }

		void print(std::ostream &os, size_t level)
		{
			print_indent(os, level);
			os << get_name() << ":";
		}

	private:
		sc_session *s;
		sc_segment *wseg;
		code_block *blk;
		mutable sc_addr dummy_op;
	};
}

code_block* program::create_label(const sc_string &lbl)
{
	code_block *lbl_block = get_label(lbl);

	if (lbl_block != 0) {
		// TODO: Error: duplicated label
	}

	lbl_block = new label_block(s, wseg, lbl);
	labels.insert(labels_t::value_type(lbl, lbl_block));
	return lbl_block;
}

void program::print(std::ostream &os)
{
	os << get_name() << std::endl;
	os << "{" << std::endl;
	get_main_block()->print(os, 1);
	os << "}" << std::endl;
}

std::ostream& scc::operator <<(std::ostream &os, program &prg)
{
	prg.print(os);
	return os;
}
