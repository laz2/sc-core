
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

#include "precompiled_p.h"

#include "scp_cache_p.h"

#define rcast(type,e) (reinterpret_cast<type>(e))

#define STRUCT_MEMB_OFFS (str,memb) \
	(rcast(unsigned,(&((str *)0)->memb)))

#define STRUCT_BY_MEMBPTR(ptr,str,memb) \
	(rcast(str *,rcast(char *,ptr) - rcast(unsigned long,&((str *)0)->memb)))

// invalidate cache entry here
void scp_cache_observer(sc_global_addr::die_handle *h,sc_addr a)
{
	scp_cache *cache = reinterpret_cast<scp_cache *>(h->extra);
	scp_cache::centry *ptr = STRUCT_BY_MEMBPTR(h,scp_cache::centry,observer);
	unsigned i = ptr - cache->data;
	cache->data[i].key = 0;
	cache->data[i].value = 0;
}

int cache_hits=0;
int cache_misses=0;
int cache_replaces=0;

scp_cache::scp_cache()
{
	replaces = 0;
	for (int i=0;i<size;i++) {
		data[i].observer.handler = scp_cache_observer;
		data[i].observer.extra = this;
		data[i].key = 0;
		data[i].value = 0;
	}
}

scp_cache::~scp_cache()
{
	invalidate();
}

void scp_cache::invalidate()
{
	for (int i=0;i<size;i++) {
		if (data[i].value) {
			data[i].value->remove_observer(&data[i].observer);
			data[i].value = 0;
		}
		data[i].key = 0;
	}
}

void scp_cache::put(sc_addr key, sc_addr value, sc_addr v2, sc_addr v3)
{
	unsigned i = hash_fn(key);
	if (data[i].value) {
		if (data[i].key != key) {
			cache_replaces++;
			replaces++;
			if (diag_output)
				printf("cache replace: key=%p, value=%p\n",(void *)key,(void *)value);
			fflush(stdout);
		}
		data[i].value->remove_observer(&data[i].observer);
	}
	data[i].key = key;
	data[i].value = value;
	data[i].value2 = v2;
	data[i].value3 = v3;
	if (value)
		value->add_observer(&data[i].observer);
}

void scp_cache::clear(sc_addr key)
{
	unsigned i = hash_fn(key);
	if (data[i].key != key)
		return;
	data[i].key = 0;
	if (data[i].value) {
		data[i].value->remove_observer(&data[i].observer);
		data[i].value = 0;
	}
}

bool scp_cache::get(sc_addr key,sc_addr &value,sc_addr &v2,sc_addr &v3)
{
	unsigned i = hash_fn(key);
	if (data[i].key != key) {
		cache_misses++;
		return false;
	}
	value = data[i].value;
	v2 = data[i].value2;
	v3 = data[i].value3;
	cache_hits++;
	return true;
}

bool scp_cache::get(sc_addr key,sc_addr *value,sc_addr *v2,sc_addr *v3)
{
	unsigned i = hash_fn(key);
	if (data[i].key != key) {
		cache_misses++;
		return false;
	}
	if (value)
		*value = data[i].value;
	if (v2)
		*v2 = data[i].value2;
	if (v3)
		*v3 = data[i].value3;
	cache_hits++;
	return true;
}
