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
 * @file ext_dynmod.cpp
 * @brief Example of dynamic module for PM with one external function.
 *
 * @author Dmitry Lazurkin
 */

#include <libsc/libsc.h>
#include <libsc/pm_keynodes.h>

/*
#define EXT_DYNMOD_EXPORT __declspec(dllexport)

extern "C" EXT_DYNMOD_EXPORT sc_retval Test_ext_prg(sc_session *s, addr_vector &prms)
{
	prms[1] = IN_;
	return RV_OK;
}
*/
