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

#include <libsc/libsc.h>
#include <libsc/std_constraints.h>
#include <libsc/sc_print_utils.h>
#include <libsc/sc_utils.h>
#include <libsc/segment_utils.h>

#include <libpm/pm.h>
#include <libpm/pm_sched.h>

#include <boost/checked_delete.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/scoped_array.hpp>

#include <sstream>
#include <ostream>
#include <vector>
#include <list>
#include <map>

#include "rgp.h"
#include "rgp_log_p.h"
#include "io_p.h"
