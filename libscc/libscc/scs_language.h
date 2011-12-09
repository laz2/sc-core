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
 * @file scs_language.hpp
 * @ingroup cc
 * @brief Common classes of SCs language.
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_SCS_LANGUAGE_HPP_INCLUDED__
#define __SCC_SCS_LANGUAGE_HPP_INCLUDED__

/// @addtogroup scc
/// @{

#include <memory>
#include <stdexcept>
#include <sstream>
#include <map>

#include <libsc/sc_types.h>
#include <libsc/sc_assert.h>
#include <libsc/sc_utils.h>
#include <libsc/stl_utils.h>

#include "config.h"
#include "scps_language.h"

namespace scc
{
	/// Hold language objects for attributes in SCs.
	class LIBSCC_API sc_attributes
	{
	public:
		sc_attributes() {}

		void add(lang_object *attr, sc_type arc_type);

		const lang_objs_list& consts() const { return consts_; }

		const lang_objs_list& vars() const { return vars_; }

		const lang_objs_list& metavars() const { return metavars_; }

		bool empty() const { return consts_.empty() && vars_.empty() && metavars_.empty(); }

	private:
		lang_objs_list consts_;
		lang_objs_list vars_;
		lang_objs_list metavars_;

	/// @name Methods check if this #sc_attributes object contains passed #lang_object or #sc_addr.
	/// @{
	public:
		bool has_const(lang_object *attr) const { return contains(consts_, attr); }

		bool has_var(lang_object *attr) const { return contains(vars_, attr); }

		bool has_metavar(lang_object *attr) const { return contains(metavars_, attr); }

		bool has_const(sc_addr attr) const { return has(consts_, attr); }

		bool has_var(sc_addr attr) const { return has(vars_, attr); }

		bool has_metavar(sc_addr attr) const { return has(metavars_, attr); }

	private:
		static bool has(const lang_objs_list &container, sc_addr attr);
	/// @}

	/// @name Methods return count of passed #lang_object or #sc_addr in this #sc_attributes object.
	/// @{
	public:
		size_t count_const(lang_object *attr) const { return std::count(consts_.begin(), consts_.end(), attr); }

		size_t count_var(lang_object *attr) const { return std::count(vars_.begin(), vars_.end(), attr); }

		size_t count_metavar(lang_object *attr) const { return std::count(metavars_.begin(), metavars_.end(), attr); }

		size_t count_const(sc_addr attr) const { return count(consts_, attr); }

		size_t count_var(sc_addr attr) const { return count(vars_, attr); }

		size_t count_metavar(sc_addr attr) const { return count(metavars_, attr); }

	private:
		static size_t count(const lang_objs_list &container, sc_addr attr);
	/// @}
	};

	/// Use it if you need just empty #sc_attributes object.
	extern LIBSCC_API sc_attributes empty_sc_attributes;

	/// Delete memory from pointer @p attrs if it doesn't point to #empty_sc_attributes.
	static inline
	void delete_not_empty(sc_attributes *attrs)
	{
		if (attrs != &empty_sc_attributes)
			delete attrs;
	}

	class LIBSCC_API sc_ident
	{
	public:
		sc_ident(sc_attributes *_attrs, lang_object_ref *_ident)
			: ident_(_ident)
		{
			SC_ASSERT(ident_);

			if (_attrs)
				attrs_ = _attrs;
			else
				attrs_ = &empty_sc_attributes;
		}

		~sc_ident()
		{
			delete_not_empty(attrs_);
		}

		lang_object_ref *ident() { return ident_; }

		const sc_attributes *attrs() const { return attrs_; }

	private:
		sc_attributes   *attrs_;
		lang_object_ref *ident_;
	};

	typedef std::list<sc_ident *> sc_idents_list;

	class LIBSCC_API sc_alias
	{
	private:
		const char *name_;
		const file_position *pos_;

	public:
		sc_alias(const char *_name, const file_position *_pos)
			: name_(_name), pos_(_pos)
		{}

		~sc_alias() {}

		const char *name() const { return name_; }

		const file_position *pos() const { return pos_; }
	};

	typedef std::list<sc_alias *> sc_aliases_list;

	template<class T>
	void delete_members_and_container(const T *c)
	{
		typename T::const_iterator it = c->begin();
		for (; it != c->end(); ++it)
			delete *it;

		delete c;
	}

	class LIBSCC_API sc_element : public lang_object
	{
	public:
		sc_element(sc_addr _sign)
			: sign_(_sign)
		{}

		~sc_element() {}

		object_type type() const { return OBJ_SC_ELEMENT; }

		sc_addr sign() const { return sign_; }

		void print(std::ostream &os) { os << SC_QURI(sign_); }

		inline static
		sc_element* from(lang_object *obj)
		{
			SC_ASSERT(is_a(obj, OBJ_SC_ELEMENT));
			return static_cast<sc_element *>(obj);
		}

	protected:
		sc_addr sign_;
	};

	class LIBSCC_API special_node : public sc_element
	{
	protected:
		sc_type new_type_;
		sc_type mask_;

	public:
		special_node(sc_addr _sign, sc_type _new_type, sc_type _mask)
			: sc_element(_sign), new_type_(_new_type), mask_(_mask)
		{
		}

		object_type type() const { return OBJ_SPEC_NODE; }

		sc_type new_type() const { return new_type_; }

		sc_type mask() const { return mask_; }

		virtual bool is_allowed(sc_type old_type) =0;
	};

	/// Special scs-node class for fuzzynness subtypes ("@pos", "@neg", "@fuz").
	class LIBSCC_API fuz_special_node : public special_node
	{
	public:
		fuz_special_node(sc_addr _sign, sc_type _new_type, sc_type _mask)
			: special_node(_sign, _new_type, _mask) {}

		bool is_allowed(sc_type old_type)
		{
			if (old_type == new_type_)
				return true;

			if (!(old_type & SC_FUZZYNESS_MASK))
				return true;

			return false;
		}
	};

	/// Special scs-node class for syntactic subtypes ("@node", "@undf", "@arc-general", "@arc-main").
	class LIBSCC_API syntactic_special_node : public special_node
	{
	public:
		syntactic_special_node(sc_addr _sign, sc_type _new_type, sc_type _mask)
			: special_node(_sign, _new_type, _mask) {}

		bool is_allowed(sc_type old_type)
		{
			if (old_type == new_type_)
				return true;

			if (old_type == SC_UNDF)
				if (new_type_ == SC_NODE || new_type_ == SC_ARC)
					return true;

			if (old_type == SC_ARC)
				if (new_type_ == SC_ARC_MAIN)
					return true;

			return false;
		}
	};
} // namespace scc

/// @}

#endif // __SCC_SCS_LANGUAGE_HPP_INCLUDED__
