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

#include "scps_language.h"

using namespace scc;

void code_block::set_next(code_block *blk)
{
	if (!get_goto()) {
		code_block *then_blk = get_then();
		code_block *else_blk = get_else();

		if (then_blk && !else_blk) {
			set_else(blk);
		} else if (else_blk && !then_blk) {
			set_then(blk);
		} else {
			set_goto(blk);
		}
	}
}

void complex_code_block::print(std::ostream &os, size_t level)
{
	print_indent(os, level);
	os << "// <" << get_name() << ">" << std::endl;

	code_blocks_list::iterator it = blocks.begin();
	for (; it != blocks.end(); ++it) {
		(*it)->print(os, level + 1);
		os << std::endl;
	}

	print_indent(os, level);
	os << "// </" << get_name() << ">" << std::endl;
}

complex_code_block::~complex_code_block()
{
	std::for_each(blocks.begin(), blocks.end(),
		boost::checked_deleter<code_block>());
}

void sequence_code_block::add(code_block *blk)
{
	code_block *prev_last = 0;
	if (!blocks.empty())
		prev_last = blocks.back();

	complex_code_block::add(blk);

	if (prev_last)
		prev_last->set_next(blk);
}
