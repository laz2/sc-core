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

/**
 * @addtogroup librgp
 * @{
 */

/**
 * @file rgp_server.h
 * @author Dmitry Lazurkin
 */

#ifndef __LIBRGP_RGP_SERVER_H_INCLUDED__
#define __LIBRGP_RGP_SERVER_H_INCLUDED__

#include <libsc/libsc.h>

#include "config.h"

LIBRGP_API sc_retval init_rgp_server(sc_session *init_session, int port=47805);

#endif // __LIBRGP_RGP_SERVER_H_INCLUDED__

/**@}*/
