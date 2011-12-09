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
 * @file sc_core.cpp
 * @brief Core of sc-memory.
 * @ingroup libsc
 */

/// @addtogroup libsc
/// @{

#include "precompiled_p.h"

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

sc_session::sc_session() : cwd("/")
{
	refcnt = 1;
	dead = 0;
	error = RV_OK;

#ifdef COUNT_ELEMENTS
	ses_cnt++;
#endif 
}

sc_session::~sc_session() 
{
#ifdef COUNT_ELEMENTS
	ses_cnt--;
#endif 
}

sc_session *sc_session::ref() 
{
	refcnt++;
	return this;
}

void sc_session::unref() 
{
	assert(refcnt > 0);
	if (!--refcnt && dead) {
		delete this;
	} else {
		assert(refcnt > 0);
	} 
}

void sc_session::commit() 
{
	close();
}

void sc_session::rollback() 
{
	abort();
}

sc_retval sc_session::chdir(const sc_string &uri)
{
	sc_string newcwd;
	if (uri[0] == '/')
		newcwd = uri;
	else
		newcwd = cwd + uri;
	if (newcwd[newcwd.size()-1] != '/')
		newcwd += '/';
	if (_stat(newcwd) != RV_ELSE_GEN)
		return RV_ERR_GEN;
	cwd = newcwd;
	return RV_OK;
}

class null_repo : public sc_repo {
public:
	sc_retval _stat(const sc_string &dir)
	{
		return RV_ERR_GEN;
	}
	sc_dir_iterator *search_dir(const sc_string &dir)
	{
		return 0;
	}

	sc_retval create_segment(const sc_string &uri)
	{
		return RV_ERR_GEN;
	}
	sc_retval unlink(const sc_string &uri)
	{
		return RV_ERR_GEN;
	}
	sc_retval rename(const sc_string &old,const sc_string &)
	{
		return RV_ERR_GEN;
	}
	sc_retval mkdir(const sc_string &uri)
	{
		return RV_ERR_GEN;
	}

	sc_segment * map_segment(const sc_string &uri)
	{
		return 0;
	}
	sc_retval unmap_segment(sc_segment *seg)
	{
		return RV_ERR_GEN;
	}

	sc_string get_uri()
	{
		return "";
	}
	void set_uri(const sc_string &a)
	{
	}

	void flush()
	{
	}

	~null_repo() {}
} null_repo;


DEFINE_FIXALLOCER(sc_global_addr)

#ifdef COUNT_ELEMENTS
int sc_global_addr::addr_cnt = 0;
int sc_segment::seg_cnt = 0;
int sc_iterator::iter_cnt = 0;
int sc_session::ses_cnt = 0;
#endif

using std::list;
using std::map;
using std::set;

static bool libsc_inited = false;

typedef list<sc_segment *> segment_list;

sc_session *system_session;

//export it later
sc_segment *info_seg;

sc_repo *root;
sc_custom_repo *tmp;
sc_custom_repo *proc;

int event_lock = 0;

segment_set *system_segset;

segment_table segtable;

//
// sc_addr
// {
//

void __sc_addr_erase_el(sc_global_addr *addr)
{
#ifdef DEBUG_DEAD
	addr->refcnt = -0xDEAD;
#endif
	if (addr->seg == SC_SET_SEG) {
		free(addr->impl);
		delete addr;
		return;
	}
	if (!addr->zombie) {
		addr->seg->erase_el(addr->impl);
	}
	sc_link_addr *link = addr->links;
	while (link) {
		sc_link_addr *next = link->next;
		link->seg->erase_el(link->impl);
		delete link;
		link = next;
	}
	delete addr;
}

void sc_global_addr::die()
{
	if (dead)
		return;

	die_handle *p = observers,  *next;

	while (p) {
		next = p->next;
		p->handler(p, this);
		p = next;
	}

	dead = true;
	refcnt -= SC_ADDR_REFCNT_BARRIER;
	if (!refcnt)
		__sc_addr_erase_el(this);
}

void sc_global_addr::unref()
{
	--refcnt;
#if SC_ADDR_CHECK_REFCNT
	if (!dead && refcnt < SC_ADDR_REFCNT_BARRIER)
		SC_ABORT();
#endif 
	if (!refcnt)
		__sc_addr_erase_el(this);
}

bool sc_global_addr::activate(sc_wait_type type, sc_param *params, int len)
{
	if (event_lock)
		return false;
	return waits.activate(type, params, len);
}

void sc_global_addr::add_observer(die_handle *h) throw()
{
	h->next = observers;
	observers = h;
}

bool sc_global_addr::remove_observer(die_handle *h) throw()
{
	die_handle **prev,  *next = observers;
	// frequently there will be only one observer
	if (next == h) {
		observers = h->next;
		return true;
	}
	do {
		prev = &next->next;
		next = next->next;
		if (!next) {
			return false;
		} 
	} while (next != h);
	*prev = h->next;
	return true;
}

sc_link_addr* sc_global_addr::find_link(sc_segment *seg)
{
	sc_link_addr *addr = links;
	while (addr && addr->seg < seg)
		addr = addr->next;

	if (!addr || addr->seg != seg)
		return 0;

	return addr;
}

void sc_global_addr::add_link(sc_link_addr *la)
{
	sc_link_addr **place = &links;
	sc_link_addr *val =  *place;

	while (val && val->seg < la->seg) {
		place = &val->next;
		val =  *place;
	}

	insert_link(place, la);

	++links_qnt;
	in_qnt += la->seg->get_in_qnt(la->impl);
	out_qnt += la->seg->get_out_qnt(la->impl);
}

void sc_global_addr::remove_link(sc_link_addr *link)
{
	if (((*link->prev) = link->next))
		link->next->prev = link->prev;

	link->next = 0;
	link->prev = 0;

	--links_qnt;
}

//
// }
//

inline
void notify_genel(event_fifo *str,sc_segment *seg,sc_addr_ll ll,sc_type type)
{
	if (!str)
		return;
	sc_string idtf = seg->get_idtf(ll);
	str->put_genel(idtf,type);
}

inline
void notify_setcontent(event_fifo *str,sc_segment *seg,sc_addr_ll ll,const Content &c)
{
	if (!str)
		return;
	sc_string idtf = seg->get_idtf(ll);
	str->put_setcont(idtf,c);
}

inline
void notify_setbeg(event_fifo *str,sc_segment *seg,sc_addr_ll ll,sc_addr_ll to)
{
	if (!str)
		return;
	sc_string idtf = seg->get_idtf(ll);
	sc_string aux;
	if (to)
		if (seg->is_link(to))
			aux = seg->get_link_target(to);
		else
			aux = seg->get_idtf(to);
	str->put_setbeg(idtf,aux);
}

inline
void notify_setend(event_fifo *str,sc_segment *seg,sc_addr_ll ll,sc_addr_ll to)
{
	if (!str)
		return;
	sc_string idtf = seg->get_idtf(ll);
	sc_string aux;
	if (to)
		if (seg->is_link(to))
			aux = seg->get_link_target(to);
		else
			aux = seg->get_idtf(to);
	str->put_setend(idtf,aux);
}

sc_repo* find_repo(const sc_string &str, int &pos)
{
	if (!libsc_inited)
		return pos = 1, root;

	SC_ASSERTS(str.size() && str[0] == '/', "URI " << str << " doesn't start with '/'");

	if (!str.compare(0, 5, "/tmp/"))
		return pos = 5, tmp;

	if (!str.compare(0, 6, "/proc/"))
		return pos = 6, proc;

	pos = 1;
	return root;
}

class session_base : public sc_session
{
protected:
#ifdef SYNC_MEMORY
	ReentrantLock lock;

	#define CUR_SESSION_LOCK_SECTION LOCK_SECTION(lock)
	#define CUR_SESSION_LOCAL_LOCK LOCAL_LOCK(lock)
#else
	#define CUR_SESSION_LOCK_SECTION
	#define CUR_SESSION_LOCAL_LOCK
#endif

protected:
	void reap_good_waits()
	{
		sc_wait_session_list *next = good_waits.get_next_session();
		sc_wait_session_list *p;
		while (next != &good_waits) {
			p = next;
			next = next->get_next_session();
			delete (sc_wait *)p;
		}
	}
	void scan_new_events(sc_segment *seg)
	{
		sc_wait *w = good_waits.get_next_session();
		while ((sc_wait_session_list *)w != &good_waits) {
			sc_param prm[5];
			assert(w->session == this);
			sc_addr e = w->element;
			sc_addr a1,a2;
			sc_iterator *it;
			prm[2].addr = e;
			switch (w->type) {
			case SC_WAIT_ARC_BEG:
				a1 = prm[1].addr = get_beg(e);
				if (a1 && a1->seg == seg)
					w->activate(w->type,prm+1,2);
				break;
			case SC_WAIT_ARC_END:
				a1 = prm[3].addr = get_end(e);
				if (a1 && a1->seg == seg)
					w->activate(w->type,prm+2,2);
				break;
			case SC_WAIT_ARCS_ENDS:
				a1 = prm[1].addr = get_beg(e);
				a2 = prm[3].addr = get_end(e);
				if (a1 && a2 && (a1->seg == seg || a2->seg == seg))
					w->activate(w->type,prm+1,1);
				break;
			case SC_WAIT_ATTACH_INPUT:
				it = create_iterator(sc_constraint_new(CONSTR_ALL_INPUT,
									   e),true);
				for (;!it->is_over();it->next()) {
					a1 = prm[1].addr = it->value(1);
					if (a1->seg == seg)
						w->activate(w->type,prm+1,2);
				}
				delete it;
				break;
			case SC_WAIT_ATTACH_OUTPUT:
				it = create_iterator(sc_constraint_new(CONSTR_ALL_OUTPUT,
									   e),true);
				for (;!it->is_over();it->next()) {
					a1 = prm[3].addr = it->value(1);
					if (a1->seg == seg)
						w->activate(w->type,prm+2,2);
				}
				delete it;
				break;
			case SC_WAIT_HACK_SET_MEMBER:
				it = create_iterator(sc_constraint_new(CONSTR_3_f_a_a,
									   e,0,0),true);
				for (;!it->is_over();it->next()) {
					a1 = prm[3].addr = it->value(1);
					a2 = prm[4].addr = it->value(2);
					if (a1->seg == seg || a2->seg == seg)
						w->activate(w->type,prm+2,3);
				}
				delete it;
				break;
			case SC_WAIT_HACK_IN_SET:
				it = create_iterator(sc_constraint_new(CONSTR_3_a_a_f,
									   0,0,e),true);
				for (;!it->is_over();it->next()) {
					a1 = prm[0].addr = it->value(0);
					a2 = prm[1].addr = it->value(1);
					if (a1->seg == seg || a2->seg == seg)
						w->activate(w->type,prm,3);
				}
				delete it;
			default:
				break;
			}
			w = w->get_next_session();
		}
	}
	virtual sc_retval __open_segment(sc_segment *seg) = 0;

	sc_retval __access(sc_addr addr)
	{
		if (!addr)
			return error = RV_ERR_GEN;
	#ifdef	ACCESS_DEAD_CHECK
			assert(addr->refcnt>0);
			assert(addr->refcnt<2000000);
			assert(!addr->is_dead());
	#endif
		if (!addr || addr->is_dead() || !addr->seg || !addr->impl)
			return error = RV_ERR_GEN;

		return error = is_segment_opened(addr->seg)?RV_OK:RV_ERR_GEN;
	}

	int __validate_type(sc_type type)
	{
		return sc_type_valid_table[type & SC_POOR_MASK];
	}

	// removes extra / from path
	sc_string normalize_uri(const sc_string &uri)
	{
		int i,k,n=uri.size();
		sc_string res(uri.size(),' ');
		for (k=i=0;i<n;i++) {
			res[k++] = uri[i];
			if (uri[i] == '/')
				for (;i<n-1;i++)
					if (uri[i+1] != '/')
						break;
		}
		res.erase(k);
		return res;
	}

	sc_segment *find_meta(const sc_string &_uri,sc_string &res)
	{
		sc_string uri = normalize_uri(_uri);
		if (uri == "/") {// special case
			res = "ROOT";
			return system_session->open_segment("/META");
		}
		int len = uri.size()-1;
		while (len>=0 && uri[len] == '/')
			len--;
		int pos = uri.rfind('/',len);
		res = uri.substr(pos+1,len-pos);
		return system_session->open_segment(uri.substr(0,pos+1)+"META");
	}

	sc_retval
	__create_sign(sc_segment *meta,const sc_string &name)
	{
		sc_addr addr = system_session->find_by_idtf(name,meta);
		if (addr) {
			sc_iterator *it;
			it = system_session->create_iterator(sc_constraint_new(CONSTR_3_f_a_f,
										   DIRENT,
										   0,
										   addr
									 ),true);
			bool over = it->is_over();
			delete it;
			if (over)
				goto gen_arc;
			return RV_OK;
		}
		addr = system_session->create_el(meta,SC_N_CONST);
		if (!addr)
			return RV_ERR_GEN;
		if (system_session->set_idtf(addr,name)) {
			system_session->erase_el(addr);
			SC_ABORT();
		}
	gen_arc:
		if (system_session->gen3_f_a_f(DIRENT,0,meta,SC_A_CONST|SC_POS,addr))
			SC_ABORT();
		return RV_OK;
	}

public:
	sc_retval create_sign(const sc_string &uri)
	{
		sc_string name;
		sc_segment *meta = find_meta(uri,name);
		if (!meta)
			return RV_ERR_GEN;
		if (uri != "META")
			__create_sign(meta,"META");
		return __create_sign(meta,name);
	}

	sc_retval erase_sign(const sc_string &uri)
	{
		sc_string name;
		sc_segment *meta = find_meta(uri,name);
		if (!meta)
			return RV_ERR_GEN;
		sc_addr_ll ll = meta->find_by_idtf(name);
		if (!ll)
			return RV_ERR_GEN;
		sc_addr addr = meta->get_element_addr(ll);
		if (!addr)
			return RV_ERR_GEN;
		assert(addr->seg == meta); // must not be link
		if (addr->refcnt <= SC_ADDR_REFCNT_BARRIER+1)
			system_session->erase_el(addr);
		return RV_OK;
	}

	bool is_segment_opened(sc_segment *seg) 
	{
		CUR_SESSION_LOCAL_LOCK;
		return opened_segments.is_set(seg->index) != 0;
	}

	sc_retval get_error() 
	{
		CUR_SESSION_LOCAL_LOCK;
		return error;
	}

	int get_out_qnt(sc_addr a)
	{
		sc_link_addr *next = a->links;
		sc_addr_store *addr = a;
		int cnt = 0;
		while (next) {
			cnt += addr->seg->get_out_qnt(addr->impl);
			addr = next;
			next = next->next;
		}
		cnt += addr->seg->get_out_qnt(addr->impl);
		return cnt;
	}

	int get_in_qnt(sc_addr a)
	{
		sc_link_addr *next = a->links;
		sc_addr_store *addr = a;
		int cnt = 0;
		while (next) {
			cnt += addr->seg->get_in_qnt(addr->impl);
			addr = next;
			next = next->next;
		}
		cnt += addr->seg->get_in_qnt(addr->impl);
		return cnt;
	}

	// creates and opens segment
	sc_segment *create_segment(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		if (repo->create_segment(uri.substr(pos))) {
			error = RV_ERR_GEN;
			return 0;
		}
		sc_segment *seg = repo->map_segment(uri.substr(pos));
		if (!seg) {
			error = RV_ERR_GEN;
			return 0;
		}
		create_sign(uri);
		if (__open_segment(seg)) {
			unlink(uri);
			return 0;
		}
		return seg;
	}

	sc_segment *open_segment(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		sc_segment *seg = repo->map_segment(uri.substr(pos));
		if (!seg) {
			error = RV_ERR_GEN;
			return 0;
		}
		error = RV_OK;
		if (is_segment_opened(seg))
			return seg;
		if (__open_segment(seg))
			return 0;
		scan_new_events(seg);
		return seg;
	}

	sc_segment *__map_segment(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		return repo->map_segment(uri.substr(pos));
	}

	// FIX: complete
	sc_segment *open_segment_spider(const sc_string &uri,bool complete)
	{
		set<sc_segment *> S;
		sc_segment *given = __map_segment(uri);

		if (!given) {
			error = RV_ERR_GEN;
			return 0;
		}

		// put `given' to S
		S.insert(given);

		while (!S.empty()) {
			// peek segment
			sc_segment *s = *(S.begin());
			S.erase(S.begin());
			// open it ignoring error code (could be opened already)
			__open_segment(s);
			// process all links in `s'
			sc_segment::iterator *iter = s->create_link_iterator();
			while (!iter->is_over()) {
				// FIX: support relative addressing
				sc_segment *seg = __map_segment(s->get_link_target(iter->next()));
				if (!seg)
					if (!complete)
						continue;
					else
						return 0;
				// if segment is already processed ignore
				if (S.find(seg) != S.end())
					continue;
				S.insert(seg);
			}
		}

		return given;
	}

	// yet only to check segment presence
	// returns THEN if uri points to segment, ELSE - directory
	sc_retval		_stat(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		return repo->_stat(uri.substr(pos));
	}
	sc_retval		close_segment(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		sc_segment *seg = repo->map_segment(uri.substr(pos));
		if (!seg)
			return RV_ERR_GEN;
		// it seems that g++ 2.95.3 has a bug.
		// it thinks that close_segment(sc_segment *)
		//  must be located in session_base, but it's in sc_session
		// CURIOUS: check it with 2.95.2 on Mdk 7.0 and g++ 3.x
		// DON'T EDIT
		sc_retval rv = ((sc_session *)this)->close_segment(seg);
		repo->unmap_segment(seg);
		return rv;
	}

	sc_retval unlink(const sc_string &uri)
	{
		int pos;
		sc_repo *repo = find_repo(uri,pos);
		sc_retval rv = repo->_stat(uri.substr(pos));
		if (!rv) { // uri is segment
			sc_segment *seg = repo->map_segment(uri.substr(pos));
			assert(seg);
			//BUG
			if (seg->evstream) {
				seg->evstream->put_unlink();
				seg->unlisten_segment(seg->evstream);
			}
			((sc_session *)this)->close_segment(seg);
			erase_sign(uri);
			repo->unmap_segment(seg);
			return repo->unlink(uri.substr(pos));
		} else if (!RV_IS_ERR(rv)) {
			sc_dir_iterator *iter = repo->search_dir(uri.substr(pos));
			if (iter->value() != "META") {
				delete iter;
				return error = RV_ERR_GEN;
			}
			iter->next();
			if (!iter->is_over()) {
				delete iter;
				return error = RV_ERR_GEN;
			}
			delete iter;
			repo->unlink(uri.substr(pos)+"/META");
		} else
			return error = RV_ERR_GEN;
		erase_sign(uri);
		return error = repo->unlink(uri.substr(pos));
	}
	// BUGGY. 
	sc_retval rename(const sc_string &oldname, const sc_string &newname)
	{
		sc_repo *repo;
		int pos,pos2;
		repo = find_repo(oldname,pos);
		if (find_repo(newname,pos2) != repo)
			return error=RV_ERR_GEN;
		error = repo->rename(oldname.substr(pos),newname.substr(pos2));
		if (error)
			return error;
		erase_sign(oldname);
		create_sign(newname);
		return error = RV_OK;
	}

	sc_retval mkdir(const sc_string &uri)
	{
		int pos;
		sc_repo *repo = find_repo(uri, pos);

		error = repo->mkdir(uri.substr(pos));
		if (error)
			return error;
		create_sign(uri);

		sc_string meta_uri = uri + "/META";
		repo = find_repo(meta_uri, pos);
		repo->create_segment(meta_uri.substr(pos));
		create_sign(meta_uri);

		return error = RV_OK;
	}

	sc_dir_iterator *search_dir(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		sc_dir_iterator *iter = repo->search_dir(uri.substr(pos));
		error = iter?RV_OK:RV_ERR_GEN;
		return iter;
	}

	// returns ptr to opened segment
	virtual sc_segment *find_segment(const sc_string &uri)
	{
		sc_repo *repo;
		int pos;
		repo = find_repo(uri,pos);
		sc_segment *seg = repo->map_segment(uri.substr(pos));
		if (!seg || !is_segment_opened(seg)) {
			error = RV_ERR_GEN;
			return 0;
		}
		repo->unmap_segment(seg);
		error = RV_OK;
		return seg;
	}

	sc_addr get_end(sc_addr addr)
	{
		if (__access(addr))
			return 0;

		sc_segment *seg = addr->seg;

		if (!(seg->get_type(addr->impl) & SC_CONNECTOR_MASK)) {
			error = RV_ERR_GEN;
			return 0;
		}

		sc_addr_ll ll = seg->get_end(addr->impl);
		if (!ll)
			return 0;

		sc_addr end = seg->get_element_addr(ll);
		if (__access(end))
			return 0;

		return end;
	}

	sc_addr get_beg(sc_addr addr)
	{
		if (__access(addr))
			return 0;

		sc_segment *seg = addr->seg;

		if (!(seg->get_type(addr->impl) & SC_CONNECTOR_MASK)) {
			error = RV_ERR_GEN;
			return 0;
		}

		sc_addr_ll ll = seg->get_beg(addr->impl);
		if (!ll)
			return 0;

		sc_addr beg = seg->get_element_addr(ll);
		if (__access(beg))
			return 0;

		return beg;
	}

	sc_addr get_beg_spin(sc_addr addr)
	{
		if (__access(addr))
			return 0;

		if (!(addr->seg->get_type(addr->impl) & SC_CONNECTOR_MASK)) {
			error = RV_ERR_GEN;
			return 0;
		}

		sc_addr_ll ll = addr->seg->get_beg(addr->impl);
		if (!ll)
			return 0;
		sc_addr beg = addr->seg->get_element_addr(ll);
		assert(beg);
		if (is_segment_opened(beg->seg))
			return beg;
		open_segment(beg->seg->get_full_uri());
		return beg;
	}

	sc_addr get_end_spin(sc_addr addr)
	{
		if (__access(addr))
			return 0;
		if (!(addr->seg->get_type(addr->impl) & SC_CONNECTOR_MASK)) {
			error = RV_ERR_GEN;
			return 0;
		}
		sc_addr_ll ll = addr->seg->get_end(addr->impl);
		if (!ll)
			return 0;
		sc_addr end = addr->seg->get_element_addr(ll);
		if (!end) {
			fprintf(stderr,"Warning: some segment sign does not exist (%s)\n",addr->seg->get_link_target(ll).c_str());
			return 0;
		}
		if (is_segment_opened(end->seg))
			return end;
		open_segment(end->seg->get_full_uri());
		return end;
	}

	/// Ensure @p seg contains link to @p addr.
	/// Create new link if it isn't exist.
	static sc_addr_ll ensure_link_to(sc_segment *seg, sc_addr addr)
	{
		if (addr->seg == seg)
			return addr->impl;

		sc_link_addr *link = addr->find_link(seg);
		if (link)
			return link->impl;

		sc_string str = addr->seg->get_idtf(addr->impl);
		sc_addr_ll ll = seg->create_link(addr->seg->get_full_uri() + "/" + str);
		if (!ll)
			return 0;

		link = new sc_link_addr(seg, ll);
		addr->add_link(link);

		seg->set_element_addr(ll, addr);

		return ll;
	}

	/// Erase link of @p addr in segment if link is lonely
	/// (hasn't in/out arcs in this segment).
	/// @warning This method may erase element from @p seg,
	///          so don't use it if you iterate over @p seg elements.
	static void erase_lonely_link_to(sc_segment *seg, sc_addr addr)
	{
		if (addr->seg != seg) {
			sc_link_addr *link = addr->find_link(seg);
			if (link) {
				sc_addr_ll link_ll = link->impl;

				bool lonely = (seg->get_in_qnt(link_ll) + seg->get_out_qnt(link_ll)) == 0;
				if (lonely) {
					seg->erase_el(link_ll);
					addr->remove_link(link);
					delete link;
				}
			}
		}
	}

	bool set_beg_pre(sc_addr arc,sc_addr beg)
	{
		sc_segment *seg = arc->seg;
		sc_addr_ll ll = seg->get_beg(arc->impl);
		sc_param prm[3];

		if (!beg) {
			sc_addr a;
			if (ll && (a = seg->get_element_addr(ll))) {
				prm[0].addr = a;
				prm[1].addr = arc;
				return a -> activate(SC_WAIT_DETACH_OUTPUT_PRE,prm,2);
			}
			return false;
		}
		prm[0].addr = beg;
		prm[1].addr = arc;
		if (beg->activate(SC_WAIT_ATTACH_OUTPUT_PRE,prm+1,2))
			return true;
		sc_addr_ll ll2 = seg->get_end(arc->impl);
		if (ll2 && (prm[2].addr = seg->get_element_addr(ll2))) {
			if (beg->activate(SC_WAIT_HACK_SET_MEMBER_PRE,prm,3))
				return true;
			if (prm[2].addr->activate(SC_WAIT_HACK_IN_SET_PRE,prm,3))
				return true;
		}
		return false;
	}

	bool set_end_pre(sc_addr arc,sc_addr end)
	{
		sc_segment *seg = arc->seg;
		sc_addr_ll ll = seg->get_end(arc->impl);
		sc_param prm[3];

		if (!end) {
			sc_addr a;
			if (ll && (a = seg->get_element_addr(ll))) {
				prm[1].addr = a;
				prm[0].addr = arc;
				return a -> activate(SC_WAIT_DETACH_INPUT_PRE,prm,2);
			}
			return false;
		}
		prm[2].addr = end;
		prm[1].addr = arc;
		if (end -> activate(SC_WAIT_ATTACH_INPUT_PRE,prm+1,2))
			return true;
		sc_addr_ll ll2 = seg->get_beg(arc->impl);
		if (ll2 && (prm[0].addr = seg->get_element_addr(ll2))) {
			if (end->activate(SC_WAIT_HACK_IN_SET_PRE,prm,3))
				return true;
			if (prm[0].addr->activate(SC_WAIT_HACK_SET_MEMBER_PRE,prm,3))
				return true;
		}
		return false;
	}

	sc_retval __set_beg(sc_addr arc,sc_addr beg)
	{
		sc_addr_ll ll;
		sc_param prm[3];
		sc_addr a=0;
		sc_segment *arc_seg = arc->seg;
		sc_retval error;

		ll = arc_seg->get_beg(arc->impl);
		if (ll && (a = arc_seg->get_element_addr(ll)) == beg)
			return RV_OK;

		if (!beg) {
			arc_seg->set_beg(arc->impl, 0);

			// detach output
			if (a) {
				a->down_out_qnt();

				prm[0].addr = arc;
				prm[1].addr = a;
				a->activate(SC_WAIT_DETACH_OUTPUT,prm,2);
			}
			ll = 0;
			goto ___event;
		}

		{
			sc_type beg_type = get_type(beg);

			if (beg_type & SC_CONNECTOR_MASK)
				return RV_ERR_GEN;

			if (beg_type & SC_UNDF)
				change_type(beg,(beg_type & ~SC_SYNTACTIC_MASK) | SC_NODE);
		}

		assert(!beg->locked);
		ll = ensure_link_to(arc->seg,beg);
		if (!ll)
			return RV_ERR_GEN;
		error = arc->seg->set_beg(arc->impl,ll);
		if (error)
			return error;

	___event:
		notify_setbeg(arc->seg->evstream,arc->seg,arc->impl,ll);
		prm[0].addr = beg;
		prm[1].addr = arc;
		arc->activate(SC_WAIT_ARC_BEG,prm,2);
		if (beg) {
			beg->up_out_qnt();

			beg->activate(SC_WAIT_ATTACH_OUTPUT,prm,2);
			ll = arc_seg->get_end(arc->impl);
			if (ll && (prm[2].addr = arc_seg->get_element_addr(ll))) {
				arc->activate(SC_WAIT_ARCS_ENDS,&SC_ADDR_AS_PARAM(arc),1);
				beg->activate(SC_WAIT_HACK_SET_MEMBER,prm,3);
				prm[2].addr->activate(SC_WAIT_HACK_IN_SET,prm,3);
			}
		}
		return RV_OK;
	}

	sc_retval set_beg(sc_addr arc, sc_addr beg)
	{
		if (__access(arc) || (beg && __access(beg)))
			return error;

		if (!(get_type(arc) & SC_CONNECTOR_MASK))
			return error = RV_ERR_GEN;

		if (set_beg_pre(arc, beg))
			return error = RV_ELSE_GEN;

		error = __set_beg(arc, beg);

		return error;
	}

	sc_retval __set_end(sc_addr arc, sc_addr end)
	{
		sc_addr_ll ll ;
		sc_param prm[3];
		sc_segment *seg = arc->seg;
		sc_addr a=0;
		sc_retval error;


		ll = seg->get_end(arc->impl);
		if (ll && (a = seg->get_element_addr(ll)) == end)
			return RV_OK;

		if (!end) {
			arc->seg->set_end(arc->impl,0);
			// detach input
			if (a) {
				a->down_in_qnt();

				prm[0].addr = arc;
				prm[1].addr = a;
				a->activate(SC_WAIT_DETACH_INPUT,prm,2);
			}
			ll = 0;
			goto ___event;
		}
		assert(!end->locked);
		ll = ensure_link_to(seg,end);
		if (!ll)
			return error = RV_ERR_GEN;
		error = seg->set_end(arc->impl,ll);
		if (error)
			return error;

	___event:
		notify_setend(arc->seg->evstream,seg,arc->impl,ll);
		prm[1].addr = arc;
		prm[2].addr = end;
		arc->activate(SC_WAIT_ARC_END,prm+1,2);
		if (end) {
			end->up_in_qnt();

			end->activate(SC_WAIT_ATTACH_INPUT,prm+1,2);
			ll = seg->get_beg(arc->impl);
			if (ll && (prm[0].addr = seg->get_element_addr(ll))) {
				arc->activate(SC_WAIT_ARCS_ENDS,&SC_ADDR_AS_PARAM(arc),1);
				end->activate(SC_WAIT_HACK_IN_SET,prm,3);
				prm[0].addr->activate(SC_WAIT_HACK_SET_MEMBER,prm,3);
			}
		}
		return RV_OK;
	}

	sc_retval set_end(sc_addr arc, sc_addr end)
	{
		if (__access(arc) || (end && __access(end)))
			return error;

		if (!(get_type(arc) & SC_CONNECTOR_MASK))
			return error = RV_ERR_GEN;

		if (set_end_pre(arc,end))
			return error = RV_ELSE_GEN;

		error = __set_end(arc,end);

		return error;
	}

	// not optimal
	sc_iterator* create_iterator(sc_constraint *c,bool sink)
	{
		if (!c) {
			error = RV_ERR_GEN;
			return 0;
		}

		if (c->id != CONSTR_ALL_INPUT && c->id != CONSTR_ALL_OUTPUT) {
			sc_iterator *it = sc_constraint_create_iterator(this, c, sink);
			if (!it)
				error = RV_ERR_GEN;
			return it;
		} else {
			sc_addr addr = c->params[0].addr;
			sc_iterator *iter;
			if (c->id == CONSTR_ALL_INPUT)
				iter = new meta_input_iterator(addr,this);
			else
				iter = new meta_output_iterator(addr,this);
			if (sink)
				sc_constraint_free(c);
			return iter;
		}
	}

protected:
	/// Return non SC_EMPTY if verification is successful.
	/// Add default values for properties.
	sc_type verify_type(sc_type type)
	{
		type = sc_type_add_default(type, SC_CONSTANCY_MASK, SC_CONST);
		type = sc_type_add_default(type, SC_SYNTACTIC_MASK, SC_UNDF);

		if (sc_isa(type, SC_ARC_ACCESSORY)) {
			type = sc_type_add_default(type, SC_FUZZYNESS_MASK, SC_FUZ);
			type = sc_type_add_default(type, SC_PERMANENCY_MASK, SC_PERMANENT);
		} else {
			if ((type & SC_FUZZYNESS_MASK)
					|| (type & SC_PERMANENCY_MASK)) {
				error = RV_ERR_GEN;
				return SC_EMPTY;
			}
		}

		return type;
	}

public:
	sc_addr create_el(sc_segment *seg,sc_type type)
	{
		if (!seg || !is_segment_opened(seg)) {
			error = RV_ERR_GEN;
			return 0;
		}

		type = verify_type(type);
		if (!type) {
			error = RV_ERR_GEN;
			return 0;
		}

		sc_addr_ll ll = seg->gen_el(type);
		if (!ll) {
			error = RV_ERR_GEN;
			return 0;
		}

		sc_global_addr *addr = new sc_global_addr(seg,ll);
		seg->set_element_addr(ll,addr);
		notify_genel(seg->evstream,seg,ll,type);
		{
			sc_wait *p = seg->on_add.get_next();
			sc_param prm[2];
			prm[0].seg = seg; prm[1].addr = addr;
			while (p != &seg->on_add) {
				sc_wait *next = p->get_next();
				p->activate(SC_WAIT_SEGMENT_ON_ADD,prm,2);
				p = next;
			}
		}
		error = RV_OK;
		return addr;
	}

	sc_retval gen3_f_a_f(sc_addr e1,sc_addr *e2,sc_segment *s2,sc_type t2,sc_addr e3)
	{
		sc_addr ce2;
		error = RV_OK;
		// if (!check_sc_type(get_type(e1),SC_NODE))
		// 	return error = RV_ERR_GEN;
		if (__access(e1) || __access(e3))
			return error;
		ce2 = create_el(s2,t2);
		if (error)
			return error;
		if (set_beg(ce2,e1) || set_end(ce2,e3)) {
			sc_retval err = error;
			return erase_el(ce2), error = err;
		}
		if (e2)
			*e2 = ce2;
		return error = RV_OK;
	}

	sc_retval gen5_f_a_f_a_f(sc_addr e1,sc_addr *e2,sc_segment *s2,sc_type t2,sc_addr e3,sc_addr *e4,sc_segment *s4,sc_type t4,sc_addr e5)
	{
		sc_addr ce2,ce4;
		if (gen3_f_a_f(e1,&ce2,s2,t2,e3))
			return error;
		if (gen3_f_a_f(e5,&ce4,s4,t4,ce2)) {
			erase_el(ce2);
			return error;
		}
		if (e2)
			*e2 = ce2;
		if (e4)
			*e4 = ce4;
		return error = RV_OK;
	}

	// for supported types param count is only one
	sc_retval attach_wait(sc_wait_type type,sc_param *prm,int len,sc_wait *w)
	{
		if (len < 1)
			return error = RV_ERR_GEN;

		if (!w)
			return error = RV_ERR_GEN;

		if (type < 0)
			return error = RV_ERR_GEN;

		if (type < SC_WAIT_COUNT) {
			prm[0].addr->get_waits()->add(type,w);
		} else if (type == SC_WAIT_SEGMENT_CREATE) {

		} else {
			return error = RV_ERR_GEN;
		}

		return error = RV_OK;
	}

	sc_retval attach_good_wait(sc_wait_type type,sc_param *prm,int len,sc_wait *w)
	{
		attach_wait(type,prm,len,w);

		if (error)
			return error;

		w->type = type;
		w->session = this;
		good_waits.append_session(w);

		return error = RV_OK;
	}

	// think more about waits
	sc_retval detach_wait(sc_wait *w)
	{
		if (!w)
			return error = RV_ERR_GEN;
		w->remove();
		w->remove_session();
		return error = RV_OK;
	}

	sc_type get_type(sc_addr addr)
	{
		if (__access(addr))
			return error;
		error = RV_OK;
		return addr->seg->get_type(addr->impl);
	}

protected:
	/// Return true if conversation from type of @p addr to @p new_type is allowed.
	bool verify_type_changing(sc_addr addr, sc_type new_type)
	{
		sc_type cur_type = addr->seg->get_type(addr->impl);
		sc_type new_syn_type = new_type & SC_SYNTACTIC_MASK;

		switch (cur_type & SC_SYNTACTIC_MASK) {
		case SC_UNDF:
			// From SC_UNDF all conversations are allowed
			//
			break;
		case SC_NODE:
			if (!sc_isa(new_type, SC_NODE)) {
				// Convert sc-node to not-sc-node only
				// if node doesn't have output arcs.
				//
				// TODO: What about output SC_ARC?
				//
				if (addr->get_out_qnt() != 0)
					return false;
			}
			break;
		case SC_EDGE:
		case SC_ARC:
		case SC_ARC_ACCESSORY:
			if (!(new_type & SC_CONNECTOR_MASK)) {
				// Convert sc-connector to not-sc-connector only
				// if sc-connector doesn't have begin and end.
				//
				if (addr->seg->get_beg(addr->impl) != 0
						|| addr->seg->get_end(addr->impl) != 0)
					return false;
			}
			break;
		case SC_LINK:
			// From sc-link all conversation are allowed
			break;
		}

		return true;
	}

public:
	sc_retval change_type(sc_addr addr, sc_type new_type)
	{
		if (__access(addr))
			return error;

		new_type = verify_type(new_type);
		if (!new_type)
			return error = RV_ERR_GEN;

		sc_type cur_type = addr->seg->get_type(addr->impl);

		// May be type changing not needed?
		//
		if (cur_type == new_type)
			return error = RV_OK;

		if (!verify_type_changing(addr, new_type))
			return error = RV_ERR_GEN;

		if (addr->activate(SC_WAIT_CHANGE_TYPE_PRE, &SC_ADDR_AS_PARAM(addr), 1))
			return error = RV_ELSE_GEN;

		if (addr->seg->evstream) {
			sc_segment *seg = addr->seg;
			seg->evstream->put_changetype(seg->get_idtf(addr->impl),
						seg->get_type(addr->impl), new_type);
		}

		error = addr->seg->change_type(addr->impl,new_type);

		addr->activate(SC_WAIT_CHANGE_TYPE,&SC_ADDR_AS_PARAM(addr),1);

		return error;
	}

	bool erase_el_pre(sc_addr addr)
	{
		if (addr->activate(SC_WAIT_DIE_PRE_REAL,&SC_ADDR_AS_PARAM(addr),1)) return true;

		if (get_type(addr) & SC_CONNECTOR_MASK) {
			set_beg_pre(addr,0);
			set_beg_pre(addr,0);
		}

		sc_link_addr *link = addr->links;
		sc_addr_ll ll = addr->impl;
		sc_segment *seg = addr->seg;
		sc_iterator *iter;
		for (;;) {
			iter = seg->create_output_arcs_iterator(ll);
			for (;!iter->is_over();iter->next()){
				if (system_session->erase_el_pre(iter->value(0))) {
					delete iter;
					return true;
				}
			}
			delete iter;
			iter = seg->create_input_arcs_iterator(ll);
			for (;!iter->is_over();iter->next()){
				if (system_session->erase_el_pre(iter->value(0))) {
					delete iter;
					return true;
				}
			}
			delete iter;
			if (!link)
				break;
			ll = link->impl;
			seg = link->seg;
			link = link->next;
		}
		return false;
	}

	// far from perfect recursive algorithm
	sc_retval __erase_el(sc_addr addr)
	{
		addr->activate(SC_WAIT_DIE,&SC_ADDR_AS_PARAM(addr),1);

		// stage 1. If it's arc - forget it's ends
		if (get_type(addr) & SC_CONNECTOR_MASK) {
			__set_end(addr,0);
			__set_beg(addr,0);
		}

		// stage 2 remove all input and output arcs
		sc_link_addr *link = addr->links;
		sc_addr_ll ll = addr->impl;
		sc_segment *seg = addr->seg;
		sc_iterator *iter;
		for (;;) {
			iter = seg->create_output_arcs_iterator(ll);
			for (;!iter->is_over();iter->next())
				system_session->__erase_el(iter->value(0));
			delete iter;
			iter = seg->create_input_arcs_iterator(ll);
			for (;!iter->is_over();iter->next())
				system_session->__erase_el(iter->value(0));
			delete iter;
			if (!link)
				break;
			ll = link->impl;
			seg = link->seg;
			link = link->next;
		}
		// now all known incident arcs are dead
		if (addr->seg->evstream) {
			sc_segment *seg = addr->seg;
			seg->evstream->put_erase_el(seg->get_idtf(addr->impl));
		}
		// stage 2.5. erase_idtf. Now element with same idtf may exist
		erase_idtf(addr);
		// stage 3 mark element as dead

		reimplement(addr,0);
		addr->die();
		return error = RV_OK;
	}

	sc_retval erase_el(sc_addr addr)
	{
		if (__access(addr))
			return error;
		if (erase_el_pre(addr))
			return error = RV_ELSE_GEN;
		{
			sc_segment *seg = addr->seg;
			sc_wait *p = seg->on_remove.get_next();
			sc_param prm[2];
			prm[0].seg = seg; prm[1].addr = addr;
			while (p != &seg->on_remove) {
				sc_wait *next = p->get_next();
				p->activate(SC_WAIT_SEGMENT_ON_REMOVE,prm,2);
				p = next;
			}
		}
		return __erase_el(addr);
	}

	sc_string get_idtf(const sc_addr addr)
	{
		if (__access(addr))
			return sc_string();
		error = RV_OK;
		return addr->seg->get_idtf(addr->impl);
	}

	static
	sc_retval __set_idtf(sc_addr addr, const sc_string &idtf)
	{
		sc_retval error = addr->seg->set_idtf(addr->impl, idtf);
		if (error)
			return error;
		sc_string full_uri = addr->seg->get_full_uri();
		full_uri += "/";
		full_uri += idtf;
		sc_link_addr *link = addr->links;
		while (link) {
			link->seg->set_link_target(link->impl,full_uri);
			link = link->next;
		}
		return error;
	}

	sc_retval set_idtf(sc_addr addr, const sc_string &idtf)
	{
		if (__access(addr))
			return error;
		if (addr->seg->evstream) {
			sc_string oldidtf = addr->seg->get_idtf(addr->impl);
			error = __set_idtf(addr,idtf);
			if (error)
				return error;
			addr->seg->evstream->put_setidtf(oldidtf,idtf);
		} else {
			error = __set_idtf(addr,idtf);
			if (error)
				return error;
		}
		addr->activate(SC_WAIT_IDTF,&SC_ADDR_AS_PARAM(addr),1);
		return error = RV_OK;
	}

	sc_retval erase_idtf(sc_addr addr)
	{
		if (__access(addr))
			return error;
		if (addr->seg->evstream) {
			sc_segment *seg = addr->seg;
			sc_string oldidtf = seg->get_idtf(addr->impl);
			error = seg->erase_idtf(addr->impl);
			if (error)
				return error;
			sc_string idtf = seg->get_idtf(addr->impl);
			seg->evstream->put_setidtf(oldidtf,idtf);
		} else {
			error = addr->seg->erase_idtf(addr->impl);
			if (error)
				return error;
		}
		addr->activate(SC_WAIT_IDTF,&SC_ADDR_AS_PARAM(addr),1);
		return error = RV_OK;
	}

	Content get_content(sc_addr addr)
	{
		if (__access(addr))
			return Content();
		error = RV_OK;
		return addr->seg->get_content(addr->impl);
	}

	const Content *get_content_const(sc_addr addr)
	{
		if (__access(addr))
			return 0;
		error = RV_OK;
		return addr->seg->get_content_const(addr->impl);
	}

	sc_retval set_content(sc_addr addr,const Content &c)
	{
		if (__access(addr))
			return error;
		error = addr->seg->set_content(addr->impl,c);
		if (error)
			return error;
		addr->activate(SC_WAIT_CONT,&SC_ADDR_AS_PARAM(addr),1);
		notify_setcontent(addr->seg->evstream,addr->seg,addr->impl,c);
		return error = RV_OK;
	}

	sc_retval erase_content(sc_addr addr)
	{
		if (__access(addr))
			return error;
		error = addr->seg->erase_content(addr->impl);
		if (error)
			return error;
		addr->activate(SC_WAIT_CONT,&SC_ADDR_AS_PARAM(addr),1);
		if (addr->seg->evstream) {
			Content c;
			addr->seg->evstream->put_setcont(addr->seg->get_idtf(addr->impl),c);
		}
		return error = RV_OK;
	}

	bool find_by_content(const Content &content, addr_list &result)
	{
		bool rv = false;
		sc_segment *seg = 0;

		FOR_ALL_SEGMENTS(seg, opened_segments) {
			ll_list ll_result;
			if (seg->find_by_content(content, ll_result)) {
				rv = true;
				for (ll_list::const_iterator it = ll_result.begin();
					it != ll_result.end(); ++it)
					result.push_back(seg->get_element_addr(*it));
			}
		}

		return rv;
	}

	sc_retval activate(sc_addr addr,sc_addr prm1,sc_addr prm2,sc_addr prm3)
	{
		if (__access(addr))
			return error;
		error = RV_OK;
		if (!addr->activity)
			return RV_OK;
		return addr->activity->activate(this,addr,prm1,prm2,prm3);
	}

	sc_retval reimplement(sc_addr addr,sc_activity *new_activity)
	{
		sc_retval rv;
		sc_activity *old_activity = addr->activity;
		addr->activity = new_activity;
		if (new_activity) {
			rv = new_activity->init(addr);
			if (rv) {
				addr->activity = old_activity;
				return error = rv;
			}
			new_activity->ctx = 0;
		}
		if (old_activity)
			old_activity->done();
		return error = RV_OK;
	}

	sc_addr __move_element(sc_addr addr,sc_segment *to)
	{
		if (__access(addr) || !is_segment_opened(to)) {
			error = RV_ERR_GEN;
			return 0;
		}
		sc_retval erased_idtf = addr->seg->has_erased_idtf(addr->impl);
		sc_string idtf;
		sc_type type;
		sc_addr_ll newll;
		Content cont;
		sc_addr newaddr;
		// TODO: check RV_ERR_GEN here
		if (erased_idtf) {
			idtf = addr->seg->get_idtf(addr->impl);
			if (to->find_by_idtf(idtf) != SCADDR_LL_NIL) {
				error = RV_ERR_GEN;
				return 0;
			}
		}
		type = addr->seg->get_type(addr->impl);
		assert(__validate_type(type));
		newll = to->gen_el(type);
		if (!newll) {
			error = RV_ERR_GEN;
			return 0;
		}
		if (erased_idtf && to->set_idtf(newll,idtf))
			SC_ABORT();
		cont = addr->seg->get_content(addr->impl);
		if (to->set_content(newll,cont))
			SC_ABORT();
		newaddr = new sc_global_addr(to,newll);
		to->set_element_addr(newll,newaddr);
		addr->activate(SC_WAIT_DIE,&SC_ADDR_AS_PARAM(addr),1);

		if (type & SC_CONNECTOR_MASK) {
			sc_addr beg,end;
			beg = get_beg(addr);
			end = get_end(addr);
			set_beg(addr,0);
			set_end(addr,0);
			set_beg(newaddr,beg);
			set_end(newaddr,end);
		}

		// now move all arcs
		sc_iterator *iter = system_session->create_iterator(sc_constraint_new(
						CONSTR_ALL_INPUT,
						addr
					),true);
		for (;!iter->is_over();iter->next())
			system_session->set_end(iter->value(1),newaddr);
		delete iter;
		iter = system_session->create_iterator(sc_constraint_new(
						CONSTR_ALL_OUTPUT,
						addr
					),true);
		for (;!iter->is_over();iter->next())
			system_session->set_beg(iter->value(1),newaddr);
		delete iter;
		// and remove original element
		if (addr->seg->evstream) {
			sc_segment *seg = addr->seg;
			seg->evstream->put_erase_el(seg->get_idtf(addr->impl));
		}
		erase_idtf(addr);
		addr->die();
		error = RV_OK;
		return newaddr;
	}

	sc_addr uri2sign(const sc_string &uri)
	{
		sc_string name;
		sc_segment *meta = find_meta(uri,name);
		if (!meta) {
		__error:
			error = RV_ERR_GEN;
			return 0;
		}
		sc_addr_ll ll = meta->find_by_idtf(name);
		if (!ll)
			goto __error;
		sc_addr addr = meta->get_element_addr(ll);
		if (!addr)
			goto __error;
		assert(addr->seg == meta);
		__open_segment(meta);
		error = RV_OK;
		return addr;
	}

	sc_string sign2uri(sc_addr addr)
	{
		sc_string metauri = addr->seg->get_full_uri();
		error = RV_OK;
		//BUG: if metauri.size < 5 
		if (metauri.substr(metauri.size()-5) != "/META") {
			error = RV_ERR_GEN;
			return sc_string();
		}
		//assert(addr->seg->has_erased_idtf(addr->impl));
		sc_string idtf = addr->seg->get_idtf(addr->impl);
		// special case for ROOT sign
		if (metauri == "/META" && idtf == "ROOT")
			return "/";
		return metauri.substr(0,metauri.size()-4) + addr->seg->get_idtf(addr->impl);
	}

	sc_addr find_by_idtf(const sc_string &name,sc_segment *seg)
	{
		if (!seg || !is_segment_opened(seg)) {
			error = RV_ERR_GEN;
			return 0;
		}
		sc_addr_ll ll = seg->find_by_idtf(name);
		if (!ll) {
			error = RV_ELSE_GEN;
			return 0;
		}
		sc_addr addr = seg->get_element_addr(ll);
		assert(addr);
		error = RV_OK;
		return addr;
	}

};

class normal_session: public session_base 
{
public:
	sc_retval __open_segment(sc_segment *seg)
	{
		if (seg->ring_0)
			return RV_ELSE_GEN;
		if (!seg->sign) {
			sc_string res;
			sc_segment *meta = find_meta(seg->get_full_uri(),res);
			assert(meta);
			sc_addr sign = system_session->find_by_idtf(basename(seg->get_full_uri()),meta);
			SC_ASSERTS(sign, "Not found sign for segment " << seg->get_full_uri());
			seg->sign = sign;
		}
		seg->sign->ref();
		seg->ref();
		system_segset->set(seg->index);
		opened_segments.set(seg->index);
		return RV_OK;
	}

	sc_retval close_segment(sc_segment *seg)
	{
		if (!is_segment_opened(seg))
			return RV_ERR_GEN;
		assert(seg->sign);
		opened_segments.reset(seg->index);
		seg->sign->unref();
		seg->unref();
		return RV_OK;
	}

	void close()
	{
		sc_segment *seg;
		FOR_ALL_SEGMENTS(seg,opened_segments) {
			close_segment(seg);
		}
		dead = 1;
		unref();
	}

	sc_session *__fork()
	{
		normal_session *s = new normal_session;
		sc_segment *seg;
		FOR_ALL_SEGMENTS(seg,opened_segments) {
			seg->sign->ref();
			s->__open_segment(seg);
		}
		return s;
	}
};

class sc_system_session: public session_base 
{
public:
	sc_system_session()
	{
		system_segset = &opened_segments;
	}
	
	sc_retval __open_segment(sc_segment *seg)
	{
		opened_segments.set(seg->index);
		return RV_OK;
	}

	sc_retval close_segment(sc_segment *seg)
	{
		return RV_OK; // or answer ERR like in prev. version ?
	}

	void close()
	{
		dead = 1, unref();
	}
	
	sc_session *__fork()
	{
		SC_ABORT();
	}
};

sc_addr sc_keynodes[1];
static int using_fs_repo;

static
void sc_keynodes_init()
{
	if (using_fs_repo) {
		sc_segment *seg = root->map_segment("info");
		if (!seg) {
			fprintf(stderr, "Error: not found segment /info\n");
			SC_ABORT();
		}
		sc_addr_ll ll = seg->find_by_idtf("dirent");
		if (!ll) {
			fprintf(stderr, "Error: not found /info/dirent\n");
			SC_ABORT();
		}
		sc_addr addr = seg->get_element_addr(ll);
		if (!addr)
			SC_ABORT();
		sc_keynodes[0] = addr;
		info_seg = seg;
		system_segset->set(seg->index);
	} else {
		root->create_segment("info");
		sc_segment *seg = root->map_segment("info");
		sc_addr_ll ll = seg->gen_el(SC_NODE|SC_CONST);
		sc_addr addr = new sc_global_addr(seg,ll);
		seg->set_element_addr(ll,addr);
		sc_keynodes[0] = addr;
		seg->set_idtf(ll,"dirent");
		info_seg = seg;
		system_segset->set(seg->index);
	}
}

#ifdef _WIN32
static const sc_string DIR_SEPARATOR = "\\";
#else
static const sc_string DIR_SEPARATOR = "/";
#endif

sc_string fs_repo_loc;

#include "numbers_segment_p.h"

sc_session *libsc_init(int use_fs_repo)
{
	if (fs_repo_loc.empty()) {
		if (char* _s = getenv("LIBSC_REPO")) {
			fs_repo_loc = _s;
		} else {
			fs_repo_loc = "fs_repo";
		}
	}
	using_fs_repo = use_fs_repo;
	if (use_fs_repo) {
		root = create_fs_repo("/",fs_repo_loc+DIR_SEPARATOR);
	} else {
		root = create_tmp_repo("/");
	}

	if (libsc_inited)
		return system_session;

	sc_constraint_init();
	__init_std_constraints();

	sc_system_session *s = new sc_system_session;
	system_session = s;

	__postinit_std_constraints(s);
	sc_keynodes_init();

	sc_segment *rmeta = s->create_segment("/META");
	if (!s->find_by_idtf("ROOT",rmeta)) {
		sc_addr root = s->create_el(rmeta,SC_N_CONST);
		if (!root)
			SC_ABORT();
		if (s->gen3_f_a_f(DIRENT,0,rmeta,SC_A_CONST|SC_POS,root))
			SC_ABORT();
		if (s->set_idtf(root,"ROOT"))
			SC_ABORT();
	}

	libsc_inited = true;
	tmp = create_tmp_repo("/tmp/");
	proc = create_tmp_repo("/proc/");

	s->mkdir("/tmp");
	s->mkdir("/proc");

	s->create_segment("/tmp/META");
	s->create_segment("/proc/META");

	//
	// Create "/proc/numattrs" segment for keynodes like 1_, 2_ ...
	//
	sc_segment_base *numattrs_seg = new numbers_segment("", "_");
	proc->create_segment("numattrs", numattrs_seg);
	s->create_sign("/proc/numattrs");
	s->open_segment("/proc/numattrs");

	sc_atoms_init();

	return s;
}

void libsc_flush()
{
	root->flush();
}

void libsc_deinit()
{
	sc_constraint_done();
	root->flush();
	
	delete tmp;
	tmp = 0;

	delete proc;
	proc = 0;

	if (!using_fs_repo) {
		delete root;
		root = 0;
	}

	delete system_session;
	system_session = 0;

	libsc_inited = false;
}

sc_session *libsc_login()
{
	return new normal_session;
}

// we must erase all incident arcs in other segments
// BUG. this code have possibility to remove arc in 
void __release_erase(sc_addr addr)
{
	// remove all input and output arcs in other segments
	sc_link_addr *link = addr->links;
	addr->links = 0;
	while (link) {
		sc_addr_ll ll = link->impl;
		sc_segment *seg = link->seg;
		sc_iterator *iter;
		iter = seg->create_output_arcs_iterator(ll);
		for (;!iter->is_over();iter->next())
			system_session->__erase_el(iter->value(0));
		delete iter;
		iter = seg->create_input_arcs_iterator(ll);
		for (;!iter->is_over();iter->next())
			system_session->__erase_el(iter->value(0));
		delete iter;
		seg->erase_el(ll);
		sc_link_addr *next = link->next;
		delete link;
		link = next;
	}
	// now all known incident arcs are dead
}

// We cannot remove any elements inside seg. Representation task
//  is to handle erase flag properly
// we don't delete elements of given segment, but incident arcs
//  in other segments are deleted if erase is true
void release_segment_addreses(sc_segment *seg, bool erase)
{
	// stage 1. remove all link addreses FROM given segment
	sc_segment::iterator *iter = seg->create_link_iterator();
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();
		sc_addr addr = seg->get_element_addr(ll);
		if (!addr)
			continue;
		seg->set_element_addr(ll,0);
		assert(addr->seg != seg);
		sc_link_addr *link = addr->find_link(seg);
		assert(link);
		addr->remove_link(link);
		delete link;
	}
	delete iter;
	// stage 2. now only non-link addresses are left
	iter = seg->create_iterator_on_elements();
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();
		sc_addr addr = seg->get_element_addr(ll);
		// dead links may cause this
		if (!addr)
			continue;
		assert(addr->seg == seg);
		// somebody may still hold reference on this address
		//assert(addr->refcnt == SC_ADDR_REFCNT_BARRIER);
		if (erase)
			addr->activate(SC_WAIT_DIE,&SC_ADDR_AS_PARAM(addr),1);
		if (erase)
			// erase incident arcs in other segments
			__release_erase(addr);
		else {
			// clean all links, to not erase them
			sc_link_addr *link = addr->links;
			while (link) {
				sc_link_addr *next = link->next;
				link->seg->set_element_addr(link->impl,0);
				delete link;
				link = next;
			}
		}
		addr->zombie = true;
		addr->die();
	}
	delete iter;
}

/// Return target's #sc_addr for link @p link_ll. Return #SCADDR_NIL if target not found.
/// @note @p source_seg in parameters for logging purpose.
static sc_addr resolve_link_target_addr(sc_segment *source_seg, sc_addr_ll link_ll)
{
	const sc_string &target_uri = source_seg->get_link_target(link_ll);
	int pos;
	sc_repo *repo = find_repo(target_uri, pos);
	sc_string rep_uri = target_uri.substr(pos);

	sc_segment *target_seg = repo->map_segment(dirname(rep_uri));
	if (!target_seg) {
		if (allow_unresolved_links && diag_output)
			fprintf(stderr, "[SC] Error: Referenced from segment \"%s\" segment of link target \"%s\" wasn't found.\n",
				SC_URIc(source_seg), target_uri.c_str());
		return 0;
	}

	system_segset->set(target_seg->index);

	sc_addr_ll ll = target_seg->find_by_idtf(basename(rep_uri));
	if (!ll) {
		if (allow_unresolved_links && diag_output)
			fprintf(stderr, "[SC] Error: Referenced from segment \"%s\" link target \"%s\" wasn't found.\n",
				SC_URIc(source_seg), target_uri.c_str());
		return 0;
	}

	return target_seg->get_element_addr(ll);
}

bool allow_unresolved_links = true;

bool init_segment(sc_segment *seg)
{
	//
	// This segment may have link to element in segment
	// which has link to element in this segment (cycle), then
	// init at first non-link elements for this segment.
	//
	{
		boost::scoped_ptr<sc_segment::iterator> iter(seg->create_iterator_on_elements());
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();
			if (!seg->is_link(ll)) {
				sc_addr addr = new sc_global_addr(seg, ll);
				seg->set_element_addr(ll, addr);
			}
		}
	}

	//
	// Try to set target addrs for links in segment.
	//

	bool success = true;

	{
		boost::scoped_ptr<sc_segment::iterator> iter(seg->create_link_iterator());
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();

			sc_addr addr = resolve_link_target_addr(seg, ll);
			seg->set_element_addr(ll, addr);

			if (addr) {
				sc_link_addr *link = new sc_link_addr(seg, ll);
				addr->add_link(link);
			} else {
				if (!allow_unresolved_links)
					success = false;
			}
		}
	}

	return success;
}


sc_iterator *create_input_iterator(sc_session *s,sc_addr e)
{
	return new meta_input_iterator(e,s);
}

sc_iterator *create_output_iterator(sc_session *s,sc_addr e)
{
	return new meta_output_iterator(e,s);
}

bool check_prm_access(sc_session *s,sc_param prm[],int len)
{
	for (int i=len-1;len>=0;len--)
		if (!s->is_segment_opened(prm[i].addr->seg))
			return false;
	return true;
}

#include "idtf_gen.h"
sc_repo::sc_repo()
{
	current_tmp_prefix = generate_uid();
}

sc_repo::~sc_repo()
{
}

#ifdef USE_FIXALLOCER
fixallocer<meta_input_iterator> meta_input_iterator_allocer;
void * meta_input_iterator::operator new(size_t __dummy) throw(std::bad_alloc) \
{
	return ::meta_input_iterator_allocer.alloc();
}
void meta_input_iterator::operator delete(void *p)
{
	::meta_input_iterator_allocer.free((meta_input_iterator *)p);
}

fixallocer<meta_output_iterator> meta_output_iterator_allocer;
void * meta_output_iterator::operator new(size_t __dummy) throw(std::bad_alloc) \
{
	return ::meta_output_iterator_allocer.alloc();
}
void meta_output_iterator::operator delete(void *p)
{
	::meta_output_iterator_allocer.free((meta_output_iterator *)p);
}

#endif

sc_addr sc_plain_set_new(sc_session *s, sc_addr a_set)
{
	sc_plain_set impl = (sc_plain_set)malloc(sizeof(sc_addr *)*(s->get_out_qnt(a_set)+1));
	sc_plain_set p = impl;
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a,
								 a_set,
								 SC_A_CONST,
								 0
							   ),true);
	for (;!iter->is_over();iter->next())
		*p++ = iter->value(2);
	delete iter;
	*p++ = 0;
	// compact memory (e.g. get_out_qnt returns count of all arcs)
	// this may be safely commented out
	impl = (sc_plain_set)realloc(impl, sizeof(sc_addr *)*(p-impl));
	return new sc_global_addr(SC_SET_SEG, reinterpret_cast<sc_addr_ll>(impl));
}

void sc_plain_set_destroy(sc_addr a_sc_set)
{
	a_sc_set->die();
}



sc_activity::sc_activity() : ctx(0), on_main_stack(false), sched_added(false)
{
}

sc_activity::~sc_activity()
{
}
sc_wait::sc_wait() : element(0), session(0)
{
}
sc_wait::~sc_wait()
{
}

/// @}
