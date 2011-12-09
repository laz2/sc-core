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

%{
	#include <libsc/pm_keynodes.h>
%}

%immutable;
extern LIBSC_API sc_addr pm_keynodes[381];
extern LIBSC_API sc_addr pm_numattr[];
extern LIBSC_API sc_addr pm_num_segc_attrs[];
extern LIBSC_API sc_segment *pm_keynodes_segment;
extern LIBSC_API sc_addr pm_num_set_attrs[];
%mutable;

%inline %{
	sc_addr pm_keynodes_item(int i) {
		if((i >= 0) && (i < 381)) {
			return pm_keynodes[i];
		} else { 
			return NULL;
		}
	}

	sc_addr pm_numattr_item(int i) {
		if ((i >= 0) && (i < 257)) {
			return pm_numattr[i];
		} else { 
			return NULL;
		}
	}

%}

LIBSC_API void pm_keynodes_init(sc_session *system);

