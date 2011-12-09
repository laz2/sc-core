
/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

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

sc_segment *libsc_default_creator_segment_var;

basic_advanced_activity::basic_advanced_activity() : sign(0)
{
}

basic_advanced_activity::~basic_advanced_activity()
{
	done();
}

sc_retval basic_advanced_activity::init(sc_addr _this)
{
	sign = _this;
	return RV_OK;
}

void basic_advanced_activity::done(void)
{
	if (!sign)
		return;
	sign = 0;
}

