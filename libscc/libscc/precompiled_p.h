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

//
// STL
//
#include <cmath>

#include <stdexcept>

#include <sstream>
#include <ostream>
#include <iostream>
#include <fstream>

#include <algorithm>

#include <memory>

#include <deque>
#include <vector>
#include <list>
#include <map>

//
// Boost
//
#include <boost/checked_delete.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#define foreach         BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

//
// antlr3c
//
#include <antlr3.h>

//
// libsc
//
#include <libsc/libsc.h>
#include <libsc/pm_keynodes.h>
#include <libsc/sc_assert.h>
#include <libsc/std_constraints.h>
#include <libsc/sc_print_utils.h>
#include <libsc/sc_utils.h>
#include <libsc/stl_utils.h>
#include <libsc/segment_utils.h>
#include <libsc/string_util.h>
#include <libsc/sc_tuple.h>
#include <libsc/sc_set.h>
#include <libsc/sc_print_utils.h>

//
// libpm
//
#include <libpm/pm.h>
#include <libpm/scp/scp_core.h>
#include <libpm/scp/scp_operator.h>
