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
 * @file scps_language.hpp
 * @ingroup SCC
 * @brief Common classes of SCPs language.
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCPS_LANGUAGE_HPP_INCLUDED__
#define __SCC_SCPS_LANGUAGE_HPP_INCLUDED__

/// @addtogroup SCC
/// @{

#include <memory>
#include <stdexcept>
#include <sstream>
#include <map>

#include <boost/format.hpp>
#include <boost/cstdint.hpp>

#include <libsc/sc_types.h>
#include <libsc/sc_assert.h>
#include <libsc/sc_utils.h>

#include "config.h"

#if defined(_MSC_VER)
// disable warning for MSVC C++ exception specification
// ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
#endif

namespace scc
{
	class lang_error : public std::domain_error
	{
	public:
		explicit lang_error(const std::string &_msg)
			: domain_error(_msg)
		{}

		virtual ~lang_error() throw()
		{}
	};


	/// Throw #scps.lang_error. @p out expects cpp stream operators for formating.
	#define SCPS_THROW_ERROR(out) \
		SC_THROW_EXCEPTION(lang_error, out)

	/// Throw #scps.lang_error. Use printf formating style.
	#define SCPS_THROW_ERROR_F(fmt, ...) \
		SCPS_THROW_ERROR(print_f(fmt, __VA_ARGS__))

	/// Throw #scps.lang_error. Use boost::format formating style.
	#define SCPS_THROW_ERROR_BF(fmt, ...) \
		SCPS_THROW_ERROR(boost::format(fmt) % __VA_ARGS__)


	/// Type of language object.
	typedef boost::uint16_t object_type;

	/// @name Types of various objects.
	/// @{
	
	// TODO: think about type checking

	const object_type OBJ_SC_ELEMENT = 0x01;
	const object_type OBJ_SPEC_NODE  = 0x02|OBJ_SC_ELEMENT;
	const object_type OBJ_PROGRAM    = 0x04|OBJ_SC_ELEMENT;
	const object_type OBJ_VARIABLE   = 0x08|OBJ_SC_ELEMENT;
	const object_type OBJ_LABEL      = 0x10;
	const object_type OBJ_PARAMETER  = 0x20|OBJ_VARIABLE;
	/// @}

	/// Position in source file.
	class LIBSCC_API file_position
	{
	private:
		const char *stream_;
		size_t      line_;
		size_t      begin_;

	public:
		file_position(const char *_stream, size_t _line, size_t _begin)
			: stream_(_stream), line_(_line), begin_(_begin)
		{}

		~file_position() {}

		const char *stream() const { return stream_; }

		size_t line() const { return line_; }

		size_t begin() const { return begin_; }
	};

	/// Base class of all objects in SCPs source.
	class LIBSCC_API lang_object
	{
	public:
		virtual ~lang_object() {}

		virtual object_type type() const =0;

		virtual sc_addr sign() const =0;

		virtual void print(std::ostream &os) =0;

		static
		bool is_a(lang_object *obj, object_type t) { return (obj->type() & t) != 0; }

		bool is_a(object_type t) { return is_a(this, t); }
	};

	class LIBSCC_API lang_object_ref
	{
	private:
		const file_position *pos_;
		lang_object         *obj_;
		const char          *name_;

	public:
		lang_object_ref(const file_position *_pos, lang_object *_obj, const char *_name)
			: pos_(_pos), obj_(_obj), name_(_name)
		{
		}

		~lang_object_ref() {}

		const file_position *pos() const { return pos_; }

		lang_object *obj() const { return obj_; }

		const char *name() const { return name_; }
	};

	typedef std::list<lang_object *>   lang_objs_list;
	typedef std::set<lang_object *>    lang_objs_set;
	typedef std::vector<lang_object *> lang_objs_vector;

	class LIBSCC_API code_block
	{
	public:
		code_block(const sc_string& _name) : name(_name) {}

		virtual ~code_block() {};

		virtual sc_addr get_first_scp_op() const =0;

		virtual code_block* get_then() const =0;

		virtual code_block* get_else() const =0;

		virtual code_block* get_goto() const =0;

		virtual void set_then(code_block *blk)=0;

		virtual void set_else(code_block *blk)=0;

		virtual void set_goto(code_block *blk)=0;

		virtual void add(code_block *blk)=0;

		void set_next(code_block *blk);

		virtual void print(std::ostream &os, size_t level)=0;

		const sc_string& get_name() const { return name; }

		void set_name(const sc_string &_name) { name = _name; }

	protected:
		static void print_indent(std::ostream &os, size_t level)
		{
			while (level-- > 0) os << "   ";
		}

	protected:
		sc_string name;
	};


	typedef std::list<code_block *> code_blocks_list;


	class LIBSCC_API complex_code_block : public code_block
	{
	public:
		complex_code_block(const sc_string& _name) : code_block(_name) {}

		~complex_code_block();

		sc_addr get_first_scp_op() const
		{
			if (blocks.empty()) {
				return SCADDR_NIL;
			} else {
				return blocks.front()->get_first_scp_op();
			}
		}

		void print(std::ostream &os, size_t level);

		void add(code_block *blk)
		{ blocks.push_back(blk); }

	protected:
		code_blocks_list blocks;
	};


	class LIBSCC_API sequence_code_block : public complex_code_block
	{
	public:
		sequence_code_block() : complex_code_block("sequence_code_block") {}

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
	};

} // namespace scc

/// @}

#endif // __SCC_SCPS_LANGUAGE_HPP_INCLUDED__
