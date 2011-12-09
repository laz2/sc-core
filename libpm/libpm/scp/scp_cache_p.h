
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


#ifndef _SCP_CACHE_H__
#define _SCP_CACHE_H__
#include <libsc/sc_addr.h>
#include <stdio.h>

// forward declaration
void scp_cache_observer(sc_global_addr::die_handle *h,sc_addr a);

extern int cache_hits;
extern int cache_misses;
extern int cache_replaces;

// scp_cache is cache of different searches
class scp_cache {
	friend void scp_cache_observer(sc_global_addr::die_handle *,sc_addr);
	enum {size = 4096};
	struct centry {
		sc_addr key;
		sc_addr value;
		sc_addr value2;
		sc_addr value3;
		sc_global_addr::die_handle observer;
	} data[size];
// TODO: play with hash_fn and sc_global_addr size
unsigned hash_fn(sc_addr a)
{
	return  (unsigned long)(a)/sizeof(sc_global_addr)%size;
//	return  (unsigned)(a)%size;
}
public:
	int replaces;

	scp_cache();

	~scp_cache();

	void invalidate();

	void put(sc_addr key,sc_addr value,sc_addr v2,sc_addr v3);

	void clear(sc_addr key);
	
	bool get(sc_addr key,sc_addr &value,sc_addr &v2,sc_addr &v3);

	bool get(sc_addr key,sc_addr *value,sc_addr *v2,sc_addr *v3);
};

#endif //_SCP_CACHE_H__
