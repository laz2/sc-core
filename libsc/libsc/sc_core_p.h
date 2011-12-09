
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


#ifndef __SC_CORE_H__
#define __SC_CORE_H__

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif


// utility function for representations
// it releases all addreses used by segment
// if erase is true, then all known links inside this segment will be deleted
void release_segment_addreses(sc_segment *seg, bool erase);

/// Initialize sc-segment @p seg. Return true if segment successful initialized else false.
/// @details
/// @li Create address structure for non-links elements.
/// @li Try to resolve and to set target address for links.
/// 
/// @see allow_unresoled_links
bool init_segment(sc_segment *seg);

sc_iterator *create_input_iterator(sc_session *s,sc_addr e);
sc_iterator *create_output_iterator(sc_session *s,sc_addr e);

#endif // __SC_CORE_H__
