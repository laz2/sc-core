
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



#ifndef __CUSTOM_ITERS_H__
#define __CUSTOM_ITERS_H__

#include "sc_constraint.h"

sc_iterator *std5_f_a_a_a_f_factory(sc_session *, sc_constraint *);
sc_iterator *std3_f_a_f_factory(sc_session *, sc_constraint *);

/// Factory for #CONSTR_ON_SEGMENT.
sc_iterator *std_on_segment_factory(sc_session *, sc_constraint *);

#endif // __CUSTOM_ITERS_H__
