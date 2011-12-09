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

#include "scs_language.h"

using namespace scc;

void sc_attributes::add(lang_object *attr, sc_type arc_type)
{
	if (arc_type & SC_CONST) {
		consts_.push_back(attr);
	} else if (arc_type & SC_VAR) {
		vars_.push_back(attr);
	} else if (arc_type & SC_METAVAR) {
		metavars_.push_back(attr);
	} else {
		SC_ASSERTF(false, "Not handled type \"%s\" for attribute arc", SC_TYPEc(arc_type));
	}
}

class sign_is_equal : std::unary_function<lang_object *, bool>
{
public:
	sign_is_equal(sc_addr _sign) : sign(_sign)
	{}

	bool operator() (const lang_object *obj)
	{
		return obj->sign() == sign;
	}

private:
	sc_addr sign;
};

bool sc_attributes::has(const lang_objs_list &container, sc_addr attr)
{
	return contains_if(container, sign_is_equal(attr));
}

size_t sc_attributes::count(const lang_objs_list &container, sc_addr attr)
{
	return std::count_if(container.begin(), container.end(), sign_is_equal(attr));
}

sc_attributes scc::empty_sc_attributes;