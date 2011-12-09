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
 * @file scps_complex_structure.hpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCPS_COMPLEX_STRUCTURE_HPP_INCLUDED__
#define __SCC_SCPS_COMPLEX_STRUCTURE_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include "config.h"
#include "scps_language.h"

namespace scc
{
	class LIBSCC_API if_struct : public code_block
	{
	public:
		if_struct() : code_block("if_structure") {}

		code_block* get_then() const
		{
			throw std::logic_error("if_structure doesn't support then_ branch.");
		}

		code_block* get_else() const
		{
			throw std::logic_error("if_structure doesn't support else_ branch.");
		}

		code_block* get_goto() const
		{
			return blocks.empty() ? 0 : blocks.back()->get_goto();
		}

		void set_then(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "Plaint code block is empty");
			blocks.back()->set_then(blk);
		}

		void set_else(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "Plaint code block is empty");
			blocks.back()->set_else(blk);
		}

		void set_goto(code_block *blk)
		{
			SC_ASSERTS(!blocks.empty(), "Plaint code block is empty");
			blocks.back()->set_goto(blk);
		}

		void add(code_block *blk);

	private:
	};
} // namespace scps

/// @}

#endif // __SCC_SCPS_COMPLEX_STRUCTURE_HPP_INCLUDED__
