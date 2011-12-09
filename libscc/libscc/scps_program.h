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
 * @file scps_program.hpp
 * @ingroup SCC
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCPS_PROGRAM_HPP_INCLUDED__
#define __SCC_SCPS_PROGRAM_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include <iosfwd>

#include <libsc/libsc.h>
#include <libsc/sc_assert.h>
#include <libsc/sc_set.h>

#include "config.h"
#include "scps_language.h"
#include "scs_language.h"
#include "names_scope.h"
#include "scps_operator.h"

namespace scc
{
	/// Represent SCPs program.
	/// Constants, variables, parameters, visibility scopes and labels management.
	class LIBSCC_API program : public sc_element
	{
	public:
		program(sc_session *_s, sc_segment *_seg, names_scope *_scope, sc_addr _sign=0);

		sc_session* get_session() const { return s; }

		sc_segment* get_segment() const { return wseg; }

		sc_string get_name() const { return s->get_idtf(sign_); }

		code_block* get_main_block() const { return main_block; }

		void set_main_block(code_block *blk);

		sc_string new_op_id()
		{
			std::ostringstream os;
			os << s->get_idtf(sign_) << "_" << cur_op_id++;
			return  os.str();
		}

		object_type type() const { return OBJ_PROGRAM; }

		sc_addr sign() { return sign_; }

		void print(std::ostream &os);

	private:
		sc_session *s;
		sc_segment *wseg;

		code_block *main_block;
		size_t cur_op_id;

		names_scope *root_scope;

	/// @name Constants management
	/// @{
	public:
		bool is_const(sc_addr constant) const { return sc_set::is_in(s, constant, consts) != SCADDR_NIL; }

		void add_const(sc_addr constant) { sc_set::include(s, consts, constant); }

		sc_addr get_consts() const { return consts; }

	private:
		sc_addr consts;
	/// @}

	/// @name Variables management
	/// @{
	public:
		bool is_var(sc_addr var) const { return sc_set::is_in(s, var, vars) != SCADDR_NIL; }

		void add_var(sc_addr var) { sc_set::include(s, vars, var); }

	private:
		sc_addr vars;
	/// @}

	/// @name Program parameters management
	/// @{
	public:
		typedef std::pair<sc_addr, int> prm_pair;
		typedef std::vector<prm_pair> prm_vector;

		static const int IN_PRM  = 1;
		static const int OUT_PRM = 1 << 1;

		void add_prm(sc_addr prm, int prm_spec);

		const prm_vector& get_prms() const { return prms_cache; }

		void add_prm(sc_addr prm, const sc_attributes &spec_attrs);

	private:
		sc_addr prms;
		prm_vector prms_cache;
	/// @}

	/// @name Labels management
	/// @{
	public:
		code_block* create_label(const sc_string &lbl);

		code_block* get_label(const sc_string &lbl) const
		{
			return get_pvalue(labels, lbl);
		}

		bool is_label(const sc_string &lbl) const
		{
			return get_label(lbl) != 0;
		}

	private:
		typedef std::map<sc_string, code_block*> labels_t;
		labels_t labels;
	/// @}

		friend LIBSCC_API std::ostream& operator <<(std::ostream &os, program &prg);
	};

	LIBSCC_API std::ostream& operator <<(std::ostream &os, program &prg);

}

/// @}

#endif // __SCC_SCPS_PROGRAM_HPP_INCLUDED__
