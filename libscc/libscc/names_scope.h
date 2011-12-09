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
 * @file names_scope.hpp
 * @ingroup scc
 * @brief Utility for holding nested names scopes.
 * @author Dmitry Lazurkin
 */

#ifndef __SCC_NAMES_SCOPE_HPP_INCLUDED__
#define __SCC_NAMES_SCOPE_HPP_INCLUDED__

/// @addtogroup scc
/// @{

#include <map>
#include <set>

#include "config.h"
#include "scps_language.h"

namespace scc
{
	/// Hold name binding for current scope and have access to parent scopes.
	/// Binding in nested scope must not override unique binding in parent scope.
	/// Methods with "down" at the end look up names to tail of scopes list.
	/// Methods with "up" at the end look up names to head of scopes list.
	class LIBSCC_API names_scope
	{
	public:
		/// Bind name to language object.
		class binding
		{
			friend class names_scope;
		private:
			char        *name_;
			bool         unique_;
			lang_object *obj_;

		private:
			binding(char *_name, lang_object *_obj, bool _unique=false)
				: name_(_name), obj_(_obj), unique_(_unique)
			{
				SC_ASSERT(name_);
				SC_ASSERT(obj_);
			}

		public:
			~binding() { delete[] name_; }

			const char *name() const { return name_; }

			bool is_unique() const { return unique_; }

			lang_object *obj() { return obj_; }
		};

		static names_scope *create()
		{ return new names_scope(); }

		~names_scope();

		names_scope *push()
		{ return new names_scope(this); }

		names_scope *pop() throw(lang_error)
		{
			if (is_head())
				SCPS_THROW_ERROR("Try to pop root scope.");

			names_scope *p = parent;
			delete this;

			return p;
		}

		binding *put(const char *name, lang_object *obj, bool unique=false) throw (lang_error);

		binding *put(const sc_string &name, lang_object *obj, bool unique=false) throw (lang_error)
		{
			return put(name.c_str(), obj, unique);
		}

		bool replace_down(lang_object *obj1, lang_object *obj2, names_scope *to_scope=0);

		bool rebind(const char *name, lang_object *obj);

		bool rebind(const sc_string &name, lang_object *obj)
		{
			return rebind(name.c_str(), obj);
		}

		binding *get_up(const char *name, const names_scope *to_scope=0);

		binding *get_up(const sc_string &name, const names_scope *to_scope=0)
		{ return get_up(name.c_str(), to_scope); }

		binding *get_down(const char *name, const names_scope *to_scope=0);

		binding *get_down(const sc_string &name, const names_scope *to_scope=0)
		{ return get_down(name.c_str(), to_scope); }

		bool contains_up(const char *name, const names_scope *to_scope=0)
		{ return get_up(name, to_scope) != 0; }

		bool contains_down(const char *name, const names_scope *to_scope=0)
		{ return get_down(name, to_scope) != 0; }

		bool is_unique_up(const char *name, const names_scope *to_scope=0) const;

		bool is_unique_down(const char *name, const names_scope *to_scope=0) const;

		names_scope *get_parent() { return parent; }

		names_scope *get_nested() { return nested; }

		bool is_head() const { return parent == 0; }

		bool is_tail() const { return nested == 0; }

	protected:
		names_scope(names_scope *_parent=0)
			: parent(_parent), nested(0)
		{}

		typedef std::map<const char *, binding *>  name_bindings_map;
		name_bindings_map bindings;

		typedef std::set<const char *> uniques_set;
		uniques_set uniques;

		names_scope *parent;
		names_scope *nested;
	};
} // namespace scc

/// @}

#endif // __SCC_NAMES_SCOPE_HPP_INCLUDED__
