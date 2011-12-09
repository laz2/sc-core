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
 * @file scps_operator.hpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCPS_OPERATOR_HPP_INCLUDED__
#define __SCC_SCPS_OPERATOR_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include <libsc/libsc.h>
#include <libsc/sc_assert.h>
#include <libsc/pm_keynodes.h>
#include <libsc/sc_set.h>

#include "config.h"
#include "scps_language.h"
#include "scs_language.h"

namespace scc
{
	/// Move input executing flow with attribute @p attr from scp-operator @p from to scp-operator @p to.
	LIBSCC_API void move_input_branch(sc_addr from, sc_addr to, sc_addr attr);

	/// Move input executing flow from scp-operator @p from to scp-operator @p to.
	/// @note Use <code>goto_</code>, <code>then_</code>, <code>else_</code>.
	LIBSCC_API void move_input_branches(sc_addr from, sc_addr to);

	class program;

	class LIBSCC_API custom_operator : public code_block
	{
	public:
		static bool is_scp_op(sc_addr addr)
		{
			return sc_set::is_in(system_session, addr, SCP_OPERATOR_TYPE) != 0;
		}

	public:
		custom_operator(program *_prg);

		sc_addr get_first_scp_op() const { return sign; }

		void print(std::ostream &os, size_t level);

	protected:
		sc_session *s;
		sc_segment *wseg;
		sc_addr sign;

		program *prg;

	/// @name Management of operator type
	/// @{
	public:
		sc_addr get_type() const { return type; }

		void set_type(sc_addr new_type);

	protected:
		sc_addr type;
		sc_addr type_arc;
	/// @}

	/// @name Management of control flow branches
	/// @{
	public:
		code_block* get_then() const { return then_blk; }

		code_block* get_else() const { return else_blk; }

		code_block* get_goto() const { return goto_blk; }

		bool is_immutable_then() const { return immutable_then; }

		bool is_immutable_else() const { return immutable_else; }

		bool is_immutable_goto() const { return immutable_goto; }

		void set_then(code_block *blk);

		void set_else(code_block *blk);

		void set_goto(code_block *blk);

	protected:
		void set_then(sc_addr op);

		void set_else(sc_addr op);

		void set_goto(sc_addr op);

	protected:
		bool immutable_then;
		bool immutable_else;
		bool immutable_goto;

		code_block *then_blk;
		code_block *else_blk;
		code_block *goto_blk;
	/// @}

	public:
		void add(code_block *blk)
		{
		}

		void add_opnd(const sc_attributes *attrs, sc_addr opnd);
	};

	class LIBSCC_API call_operator : public custom_operator
	{
	public:
		call_operator(program *_prg);
	};
} // namespace scps

/// @}

#endif // __SCC_SCPS_OPERATOR_HPP_INCLUDED__
