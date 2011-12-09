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
// STD library
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//
// STL
//
#include <cmath>

#include <stdexcept>

#include <sstream>
#include <ostream>
#include <iostream>

#include <algorithm>

#include <memory>

#include <deque>
#include <vector>
#include <list>
#include <map>

//
// Boost
//
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/algorithm/string.hpp>

//
// libsc public
//
#include "config.h"

#include "libsc.h"
#include "sc_types.h"
#include "sc_content.h"
#include "sc_event.h"
#include "segtable.h"
#include "sc_constraint.h"
#include "std_constraints.h"
#include "abort.h"
#include "sc_atoms.h"
#include "sc_keynodes.h"
#include "sc_transaction.h"
#include "pm_keynodes.h"

#include "sc_segment.h"
#include "sc_iterator.h"

#include "advanced_activity.h"
#include "event_stream.h"
#include "fixalloc.h"
#include "idtf_gen.h"
#include "tgf_io.h"

#include "sc_addr.h"

#include "sc_utils.h"
#include "sc_change_utils.h"
#include "sc_print_utils.h"
#include "string_util.h"
#include "segment_utils.h"

#include "sc_generator.h"

#include "sc_relation.h"
#include "sc_set.h"
#include "sc_struct.h"
#include "sc_tuple.h"

#include "sc_smart_addr.h"

#include "sc_assert.h"

//
// libsc private
//

#include "base_iterators_p.h"
#include "custom_iters_p.h"
#include "repository_p.h"
#include "safe_list_p.h"
#include "sc_core_p.h"
#include "sc_fallback_p.h"
#include "sc_segment_base_p.h"
