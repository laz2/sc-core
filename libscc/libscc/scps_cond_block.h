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

/**
 * @file scps_cond_block.hpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCPS_COND_BLOCK_HPP_INCLUDED__
#define __SCC_SCPS_COND_BLOCK_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include "config.h"
#include "scps_language.h"

namespace scc
{
	class LIBSCC_API complex_cond_block : public complex_code_block
	{
	public:
		complex_cond_block(const sc_string& _name) : complex_code_block(_name) {}

		code_block* get_then() const
		{
			return blocks.empty() ? 0 : blocks.back()->get_then();
		}

		code_block* get_else() const
		{
			return blocks.empty() ? 0 : blocks.back()->get_else();
		}

		code_block* get_goto() const
		{
			SC_ASSERTS(false, "cond_block doesn't support goto_ branch.");
			throw new std::logic_error("cond_block doesn't support goto_ branch.");
		}

		void set_goto(code_block *blk)
		{
			SC_ASSERTS(false, "cond_block doesn't support goto_ branch.");
			throw new std::logic_error("cond_block doesn't support goto_ branch.");
		}
	};

	class LIBSCC_API or_cond_block : public complex_cond_block
	{
	public:
		or_cond_block() : complex_cond_block("or_cond_block") {}

		void set_then(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "or_cond_block is empty");
			code_blocks_list::iterator it = blocks.begin();
			for (; it != blocks.end(); ++it)
				blocks.back()->set_then(blk);
		}

		void set_else(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "or_cond_block is empty");
			blocks.back()->set_else(blk);
		}

		void add(code_block *blk)
		{
			code_block *prev_last = 0;
			if (!blocks.empty())
				prev_last = blocks.back();

			blocks.push_back(blk);

			if (prev_last)
				prev_last->set_else(blk);
		}
	};

	class LIBSCC_API and_cond_block : public complex_cond_block
	{
	public:
		and_cond_block() : complex_cond_block("and_cond_block") {}

		void set_then(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "or_cond_block is empty");
			code_blocks_list::iterator it = blocks.begin();
			blocks.back()->set_then(blk);
		}

		void set_else(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "or_cond_block is empty");
			code_blocks_list::iterator it = blocks.begin();
			for (; it != blocks.end(); ++it)
				blocks.back()->set_else(blk);
		}

		void add(code_block *blk)
		{
			code_block *prev_last = 0;
			if (!blocks.empty())
				prev_last = blocks.back();

			blocks.push_back(blk);

			if (prev_last)
				prev_last->set_then(blk);
		}
	};

	class LIBSCC_API not_cond_block : public code_block
	{
	public:
		not_cond_block(code_block *_blk) : code_block("not_cond_block"), blk(_blk)
		{
			SC_ASSERT(blk);
		}

		~not_cond_block()
		{
			delete blk;
		}

		sc_addr get_first_scp_op() const { return blk->get_first_scp_op(); }

		code_block* get_then() const { return blk->get_else(); }

		code_block* get_else() const { return blk->get_then(); }

		code_block* get_goto() const { return blk->get_goto(); }

		void set_then(code_block *_blk) { blk->set_else(_blk); }

		void set_else(code_block *_blk) { blk->set_then(_blk); }

		void set_goto(code_block *_blk)
		{
			SC_ASSERTS(false, "Try to set goto_ branch for not_cond_block.");
			throw std::logic_error("Try to set goto_ branch for not_cond_block.");
		}

		void add(code_block *blk)
		{
			SC_ASSERTS(false, "Try to add subblock to not_cond_block.");
			throw std::logic_error("Try to add subblock to not_cond_block.");
		}

		void print(std::ostream &os, size_t level)
		{
			blk->print(os, level);
		}

	private:
		code_block *blk;
	};
} // namespace scps

/// @}

#endif // __SCC_SCPS_COND_BLOCK_HPP_INCLUDED__
