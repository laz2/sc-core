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

#ifndef __LIBRGP_LOG_H_INCLUDED__
#define __LIBRGP_LOG_H_INCLUDED__

#if 0
#include <boost/logging/format/named_write_fwd.hpp>

namespace bl = boost::logging;
typedef bl::named_logger<>::type logger_type;
typedef bl::level::holder filter_type;

BOOST_DECLARE_LOG_FILTER(g_l_level, filter_type)
BOOST_DECLARE_LOG(g_l, logger_type)
#endif

#if 0
#define L_(lvl) BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_level(), lvl)
#else
#define L_(lvl) std::cout
#endif

#endif // __LIBRGP_LOG_H_INCLUDED__
