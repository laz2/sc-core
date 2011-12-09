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
 * @file scps_compiler.hpp
 * @author Dmitry Lazurkin
 * @ingroup scc
 */

#ifndef __SCC_SCPS_COMPILER_HPP_INCLUDED__
#define __SCC_SCPS_COMPILER_HPP_INCLUDED__

/// @defgroup scc scc - Semantic Code Compiler
/// @{

/// @namespace scps
/// @brief Functional for compiling SCPs sources.

#include <sstream>

#include <boost/filesystem.hpp>

#include <libsc/libsc.h>
#include <libsc/sc_assert.h>
#include <libsc/sc_generator.h>
#include <libsc/sc_utils.h>

#include "config.h"
#include "scps_language.h"
#include "scs_language.h"
#include "names_scope.h"
#include "scps_program.h"

namespace fs=boost::filesystem;

struct SCPsLexer_Ctx_struct;
struct SCPsParser_Ctx_struct;

namespace scc
{
	class LIBSCC_API compiler
	{
	private:
		sc_session *s;
		sc_segment *wseg;
		sc_generator gen;

		sc_segment *proc_seg;

		fs::path cur_dir;

		bool debug_mode_;
		bool new_sc_aplha_;

	public:
		compiler(sc_session *_s, sc_segment *_seg);

		~compiler();

		bool debug_mode() const { return debug_mode_; }

		void debug_mode(bool mode) { debug_mode_ = mode; }

		void set_cur_dir(const fs::path &p) { cur_dir = p; }

		const fs::path& get_cur_dir() const { return cur_dir; }

		sc_session* session() { return s; }

		sc_generator &generator() { return gen; }

		bool new_sc_aplha() const { return new_sc_aplha_; }

		void new_sc_aplha(bool _new_sc_alpha) { new_sc_aplha_ = _new_sc_alpha; }

		void compile(const fs::path &p) throw (lang_error);

		void compile() throw (lang_error);

	/// @name Methods for compiling SCs source.
	/// @{
	private:
		bool expect_label;

	public:
		void set_expect_label(bool _f) { expect_label = _f; }

		bool is_expect_label() const { return expect_label; }

		lang_object_ref *term_idtf(const char *idtf, const file_position *pos);

		lang_object_ref *term_simple(lang_object_ref *ref);

		lang_object_ref *term_from_content(const file_position *pos);

		lang_object_ref *enter_term_set(sc_type t, const file_position *pos);

		void leave_term_set();

		lang_object_ref *enter_term_sys(sc_type t, const file_position *pos);

		void leave_term_sys();

		lang_object_ref *enter_term_ord_tuple(sc_type t, const file_position *pos);

		void leave_term_ord_tuple();

		lang_object_ref *assign_aliases(lang_object_ref *ref, sc_aliases_list &aliases);

		void assign_string_content(lang_object_ref *ref, const char *str);

		void assign_integer_content(lang_object_ref *ref, const char *str, const file_position *pos);

		void assign_double_content(lang_object_ref *ref, const char *str, const file_position *pos);

		void assign_file_content(lang_object_ref *ref, const char *str, const file_position *pos);

		lang_object_ref *connector(sc_ident *from, sc_type arc_type,
			const file_position *arc_pos, sc_ident *to);

		void scsentence(const sc_idents_list &from, sc_type t, const file_position *pos,
			const sc_idents_list &to);

		void change_type(sc_addr addr, sc_type t);

		void scattribute(lang_object_ref *ref, sc_type t, const file_position *pos);

	private:
		inline static
		bool is_ref_idtf(const sc_string &idtf)
		{
			SC_ASSERT(!idtf.empty());
			return idtf[0] == '$';
		}

	///@{
	///
		lang_object *term_idtf_full_uri(const char *idtf, const file_position *pos);

		lang_object *term_idtf_short(const char *idtf, const file_position *pos, const char *&name);
	///
	/// @}

		void set_alias_as_idtf(sc_addr addr, const sc_string &alias, bool &has_idtf);

		static
		sc_type get_type_from_idtf(const sc_string &idtf);

		template<class T>
		void create_special_node(const sc_string &name, sc_type new_type, sc_type mask);

		bool is_type_conversion(const lang_object_ref *from, const lang_object_ref *to,
			sc_type arc_type, const file_position *arc_pos);

		void apply_type_conversion(const lang_object_ref *from, const lang_object_ref *to,
			const file_position *arc_pos);

		sc_addr gen_connector(sc_ident *from, sc_type arc_type, const file_position *arc_pos, sc_ident *to);
	/// @}

	/// @name Scopes management
	/// @{
	public:
		void push_scope() { scope = scope->push(); }

		void pop_scope() { scope = scope->pop(); }

		void push_program(program *prg)
		{
			SC_ASSERT(prg);
			prgs_stack.push(prg);
		}

		void pop_program()
		{
			SC_ASSERT(!prgs_stack.empty());
			prgs_stack.pop();
		}

	private:
		/// @note Alias may be idtf only in first scope.
		names_scope *scope;

		typedef std::stack<program *> programs_stack;
		programs_stack prgs_stack;
	/// @}

	/// @name Includes management.
	/// @{
	public:
		/// Add directory with path @p p to include directories list.
		void add_include_dir(const fs::path &p) throw (lang_error);

		/// Find include file with relative or absolute path @p p in include directories.
		/// Throw error if file not found.
		fs::path find_include_file(const fs::path &p) throw (lang_error);

	private:
		typedef std::list<fs::path> include_dirs_list;
		include_dirs_list include_dirs;
	/// @}

	/// @name Language objects management.
	/// @{
	private:
		typedef std::map<sc_addr, lang_object *> sign2obj_map;
		sign2obj_map objs_registry;

	public:
		void registry(lang_object *obj)
		{
			sc_addr sign = obj->sign();
			if (sign)
				objs_registry.insert(sign2obj_map::value_type(sign, obj));
		}

		void deregistry(lang_object *obj)
		{
			sc_addr sign = obj->sign();
			if (sign)
				objs_registry.erase(sign);
		}

		lang_object* get_obj(sc_addr sign) { return get_pvalue(objs_registry, sign); }

		sc_element* create_sc_element(sc_addr sign)
		{
			sc_element *obj = new sc_element(sign);
			registry(obj);
			return obj;
		}

		program* create_program(sc_addr sign)
		{
			program *obj = new program(s, wseg, scope, sign);
			registry(obj);
			return obj;
		}

		file_position *new_position(const char *stream, size_t line, size_t begin)
		{
			return new file_position(stream, line, begin);
		}

		lang_object_ref *new_obj_ref(const file_position *pos, lang_object *obj, const char *name)
		{
			return new lang_object_ref(pos, obj, name);
		}
	
		bool is_external(const lang_object *obj) const
		{
			return obj->sign()->seg != wseg;
		}
	/// @}

	/// @name Messages management.
	/// @{
	public:
		void report_error(const std::string &msg, const file_position *pos);

		void report_warning(const std::string &msg, const file_position *pos);
	/// @}

	};

	extern LIBSCC_API compiler *cc;
}

#define SCC_REPORT_ERROR(pos, out) \
	{ \
		std::ostringstream __out_buf; \
		__out_buf << out; \
		scc::cc->report_error(__out_buf.str(), pos); \
	}

#define SCC_REPORT_ERROR_BF(pos, fmt, ...) \
	SCC_REPORT_ERROR(pos, boost::format(fmt) % __VA_ARGS__)

#define SCC_REPORT_WARNING(pos, out) \
	{ \
		std::ostringstream __out_buf; \
		__out_buf << out; \
		scc::cc->report_warning(__out_buf.str(), pos); \
	}

#define SCC_REPORT_WARNING_BF(pos, fmt, ...) \
	SCC_REPORT_WARNING(pos, boost::format(fmt) % __VA_ARGS__)

#define SCC_ASSERT(exp, pos, out) \
	{ \
		if (!(exp)) { \
			SCC_REPORT_ERROR(pos, "bug : " << out); \
		} \
	}

#define SCC_ASSERT_BF(exp, pos, fmt, ...) \
	SCC_ASSERT(exp, pos, boost::format(fmt) % __VA_ARGS__)

/// @}

#endif // __SCC_SCPS_COMPILER_HPP_INCLUDED__
