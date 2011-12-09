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

#include "names_scope.h"

using namespace scc;

names_scope::~names_scope()
{
	std::list<binding *> tmp;

	std::transform(bindings.begin(), bindings.end(), std::back_inserter(tmp),
		boost::bind(&name_bindings_map::value_type::second, boost::lambda::_1));

	std::for_each(tmp.begin(), tmp.end(), boost::checked_deleter<binding>());
}

names_scope::binding *names_scope::put(const char *name, lang_object *obj, bool unique)
throw (lang_error)
{
	if (is_unique_up(name))
		SCPS_THROW_ERROR_BF("Try to override unique name: %1%.", name);

	std::auto_ptr<binding> b(new binding(strdup(name), obj, unique));

	std::pair<name_bindings_map::iterator, bool> rv =
		bindings.insert(name_bindings_map::value_type(b->name(), b.get()));

	if (!rv.second)
		SCPS_THROW_ERROR_BF("Name redefinition: %1%.", b->name());

	if (unique)
		uniques.insert(b->name());

	return b.release();
}

bool names_scope::replace_down(lang_object *obj1, lang_object *obj2, names_scope *to_scope)
{
	names_scope *c  = this;
	bool         rv = false;

	while (c) {
		foreach (name_bindings_map::value_type p, c->bindings) {
			if (p.second->obj_ == obj1) {
				p.second->obj_ = obj2;
				rv = true;
			}
		}

		if (c == to_scope)
			break;

		c = c->parent;
	}

	return rv;
}

bool names_scope::rebind(const char *name, lang_object *obj)
{
	SC_ASSERT(obj);

	binding *bnd = get_pvalue(bindings, name);
	if (bnd) {
		bnd->obj_ = obj;
		return true;
	} else {
		return false;
	}
}

bool names_scope::is_unique_up(const char *name, const names_scope *to_scope) const
{
	const names_scope *c = this;

	while (c) {
		if (::contains(c->uniques, name))
			return true;

		if (c == to_scope)
			break;

		c = c->parent;
	}

	return false;
}

bool names_scope::is_unique_down(const char *name, const names_scope *to_scope) const
{
	const names_scope *c = this;

	while (c) {
		if (::contains(c->uniques, name))
			return true;

		if (c == to_scope)
			break;

		c = c->nested;
	}

	return false;
}

names_scope::binding* names_scope::get_up(const char *name, const names_scope *to_scope)
{
	names_scope *c = this;

	while (c) {
		binding *b = get_pvalue(c->bindings, name);
		if (b != 0)
			return b;

		if (c == to_scope)
			break;

		c = c->parent;
	}

	return 0;
}

names_scope::binding* names_scope::get_down(const char *name, const names_scope *to_scope)
{
	names_scope *c = this;

	while (c) {
		binding *b = get_pvalue(c->bindings, name);
		if (b != 0)
			return b;

		if (c == to_scope)
			break;

		c = c->nested;
	}

	return 0;
}
