
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

// Because of constraint system limits it's bad piece of code
class	mt_iterator: public sc_iterator {
typedef sc_transaction::trans_list list;
list	*erased;
sc_iterator *host_i;
	bool is_erased(sc_addr val)
	{
		list::reverse_iterator iter = erased->rbegin();
		while (iter != erased->rend())
			if (*iter++ == val)
				return true;
		return false;
	}
	// silly thing
	bool check_tuple()
	{
		return
			is_erased(host_i->value(0))
			|| is_erased(host_i->value(1))
			|| is_erased(host_i->value(2))
			|| is_erased(host_i->value(3))
			|| is_erased(host_i->value(4))
			|| is_erased(host_i->value(5))
			|| is_erased(host_i->value(6))
			|| is_erased(host_i->value(7))
			|| is_erased(host_i->value(8))
			|| is_erased(host_i->value(9));
	}
public:
	mt_iterator(list *_erased,sc_iterator *hi) 
	{
		SC_ABORT();
		host_i = hi;
		erased = _erased;
		if (check_tuple())
			next();
	}
	sc_retval next()
	{
		sc_retval rv;
		do {
			if (host_i->is_over())
				return host_i->next();
			rv = host_i->next();
		} while (check_tuple());
		return rv;
	}
	bool is_over()
	{
		return host_i->is_over();
	}
	sc_addr value(sc_uint num)
	{
		return host_i->value(num);
	}
	~mt_iterator() {delete host_i;}
};

void sc_transaction::commit()
{
	if (need_rollback) {
		trans_list::reverse_iterator iter = erased_elems.rbegin();
		while (iter != erased_elems.rend()) {
			if (!(*iter)->is_dead())
				host->erase_el(*iter);
			(*iter++)->unref();
		}

		need_rollback = false;
		delete this;
	}
}

// TODO: rollback is not yet tested
void sc_transaction::rollback()
{
	if (need_rollback) {
		trans_pair_list::reverse_iterator piter = orig_beg.rbegin();
		while (piter != orig_beg.rend())	{
			host->set_beg(piter->first,piter->second);
			piter++;
		}
		piter = orig_end.rbegin();
		while (piter != orig_end.rend()) {
			host->set_end(piter->first,piter->second);
			piter++;
		}
		trans_list::reverse_iterator iter = gened_elems.rbegin();
		while (iter != gened_elems.rend())
			host->erase_el(*iter++);

		segment_list::iterator created_seg_iter = created_segments.begin();
		while (created_seg_iter != created_segments.end()) {
			host->unlink((*created_seg_iter)->get_full_uri());
			created_seg_iter++;
		}

		need_rollback = false;
		delete this;
	}
}

sc_retval sc_transaction::set_beg(sc_addr addr,sc_addr beg)
{
	sc_addr obeg = host->get_beg(addr);
	sc_retval rv = host->set_beg(addr,beg);
	if (rv) {
		host->set_beg(addr,obeg);
		return rv;
	}
	orig_beg.push_back(trans_pair(addr,obeg));
	return rv;
}

sc_retval sc_transaction::set_end(sc_addr addr,sc_addr end)
{
	sc_addr oend = host->get_end(addr);
	sc_retval rv = host->set_end(addr,end);
	if (rv) {
		host->set_end(addr,oend);
		return rv;
	}
	orig_end.push_back(trans_pair(addr,oend));
	return rv;
}

sc_iterator *sc_transaction::create_iterator(sc_constraint *c,bool sink)
{
/*	sc_iterator *iter = host->create_iterator(c,sink);
	return new mt_iterator(&erased_elems,iter);*/
	return host->create_iterator(c,sink);
}

sc_addr sc_transaction::create_el(sc_segment *s,sc_type t)
{
	sc_addr rv = host->create_el(s,t);
	if (rv)
		gened_elems.push_back(rv);
	return rv;
}

// bug here (events) (bad event model)
sc_retval sc_transaction::erase_el(sc_addr a)
{
	if (!host->is_segment_opened(a->seg))
		return RV_ERR_GEN;
	a->ref();
	erased_elems.push_back(a);
	return RV_OK;
}

sc_segment *sc_transaction::create_segment(const sc_string &uri)
{
	sc_segment *rv = host->create_segment(uri);

	if (rv)
		created_segments.push_back(rv);

	return rv;
}

