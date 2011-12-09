
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

#include "tmp_segment_p.h"

#if 0
	#define PREFETCH(p) do { __asm__ __volatile__ ("prefetchnta (%0)" : : "r"(p)); } while (0)
#else 
	#define PREFETCH(p) do { } while (0)
#endif 

inline element *ELEMENT_FROM_ADDR(sc_addr_ll addr) 
{
	return reinterpret_cast<element*>(addr);
}

struct element_link : public element_link_listhead 
{
	DECLARE_FIXALLOCER(element_link)
	
	element *value;
	int refcnt;
	int dead;

	element_link(element *val) : element_link_listhead() 
	{
		value = val;
		refcnt = 1;
		dead = 0;
	} 
	
	element_link *ref() 
	{
		refcnt++;
		return this;
	}

	void unref() 
	{
		if (!--refcnt) {
			remove();
			delete this;
		}
	}

	void die() 
	{
		if (!--refcnt) {
			remove();
			delete this;
		} else {
			dead = 1;
		} 
	}
};

DEFINE_FIXALLOCER(element_link)

typedef element_link *sc_hint;

#define TMP_LINK         0x01
#define TMP_MISSING_IDTF 0x02

/**
 * @brief Structure for all element types.
 * @note This will save us reallocations on change_type.
 */ 
struct element : public element_listhead
{
	DECLARE_FIXALLOCER(element)
	
	int in_qnt;                   ///< count of input arcs
	int out_qnt;                  ///< count of output arcs
	sc_addr addr;                 ///< sc-address of this element
	sc_type type;                 ///< type of this element
	element_link_listhead input;  ///< head of input arc links list
	element_link_listhead output; ///< head of output arc links list

	// comment // -- end of first cacheline on i386
	
	element *beg; ///< only for arc: begin of arc
	element *end; ///< only for arc: end of arc
	
	/// only for arc: link in output arcs links list of begin element
	/// @note O(1) arc removing
	sc_hint beg_hint;
	/// only for arc: link in input arcs links list of end element
	/// @note O(1) arc removing
	sc_hint end_hint;

	Content cont;         ///< content of element
	void *cont_signature; ///< signature for indexing of element's content

	sc_string idtf; ///< idtf of element or URI as link target

	sc_uint8 flags;

	element(sc_type _type, sc_uint8 _flags)
	{
		beg_hint = end_hint = 0;
		beg = end = 0;

		type = _type;
		flags = _flags;

		addr = 0;
		out_qnt = in_qnt = 0;
		cont_signature = 0;
	}

	inline sc_uint8 get_flags() const
	{
		return flags;
	}

	inline void set_flags(sc_uint8 flags)
	{
		this->flags |= flags;
	}

	inline void unset_flags(sc_uint8 flags)
	{
		this->flags &= ~flags;
	}

	inline bool has_idtf() const
	{
		return (flags & TMP_MISSING_IDTF) != TMP_MISSING_IDTF;
	}

	inline void set_idtf(const sc_string &str)
	{
		idtf = str;
		unset_flags(TMP_MISSING_IDTF);
	}

	inline bool is_link() const
	{
		return (flags & TMP_LINK) == TMP_LINK;
	}
};

/// Clear or update signature for elements content.
void update_cindex(content_index &cindex, element *el)
{
	if (el->cont.type() == TCEMPTY) {
		cindex.clear(sc_addr_ll(el), el->cont_signature);
		el->cont_signature = 0;
	} else {
		el->cont_signature = cindex.update(sc_addr_ll(el), el->cont_signature, el->cont);
	}
}

DEFINE_FIXALLOCER(element)

element *tmp_segment::create_element(sc_type type)
{
	element *e = new element(type, TMP_MISSING_IDTF);
	list_head.prepend(e);
	return e;
}

void tmp_segment::attach_input_arc(sc_addr_ll el, sc_addr_ll arc)
{
	element *e = ELEMENT_FROM_ADDR(el);
	e->in_qnt++;
	element_link *link = new element_link(ELEMENT_FROM_ADDR(arc));
	ELEMENT_FROM_ADDR(arc)->end_hint = link;
	e->input.prepend(link);
}

void tmp_segment::attach_output_arc(sc_addr_ll el, sc_addr_ll arc)
{
	element *e = ELEMENT_FROM_ADDR(el);
	e->out_qnt++;
	element_link *link = new element_link(ELEMENT_FROM_ADDR(arc));
	ELEMENT_FROM_ADDR(arc)->beg_hint = link;
	e->output.prepend(link);
}

void tmp_segment::detach_input_arc(sc_addr_ll el, sc_addr_ll arc)
{
	assert(ELEMENT_FROM_ADDR(arc)->end == ELEMENT_FROM_ADDR(el));
	element *e = ELEMENT_FROM_ADDR(el);
	e->in_qnt--;
	element_link *link = ELEMENT_FROM_ADDR(arc)->end_hint;
	link->die();
}

void tmp_segment::detach_output_arc(sc_addr_ll el, sc_addr_ll arc)
{
	assert(ELEMENT_FROM_ADDR(arc)->beg == ELEMENT_FROM_ADDR(el));
	element *e = ELEMENT_FROM_ADDR(el);
	e->out_qnt--;
	element_link *link = ELEMENT_FROM_ADDR(arc)->beg_hint;
	link->die();
}

bool tmp_segment::register_idtf(const sc_string &idtf, sc_addr_ll el)
{
	return idtf_map.insert(my_pair(idtf, el)).second;
}

bool tmp_segment::deregister_idtf(const sc_string &idtf)
{
	return idtf_map.erase(idtf) > 0;
}

void tmp_segment::clear_links(element_link_listhead *head)
{
	element_link *link = head->get_next();
	while (link != head) {
		element_link *next = link->get_next();
		delete link;
		link = next;
	}
}

tmp_segment::tmp_segment(const sc_string &_uri, sc_repo *r) : sc_segment_base(_uri, r) {}

tmp_segment::~tmp_segment()
{
	CUR_SEGMENT_LOCAL_LOCK;
	idtf_counter = 0;
	release_segment_addreses(this, dead);
	element *e = list_head.get_next();
	while (e !=  &list_head) {
		element *next = e->get_next();
		clear_links(&e->input);
		clear_links(&e->output);
		delete e;
		e = next;
	}
}

tmp_segment::tmp_segment_iterator::tmp_segment_iterator(tmp_segment *_seg) : seg(_seg)
{
	SEGMENT_LOCAL_LOCK(seg);
	current = seg->list_head.get_next();
}

sc_addr_ll tmp_segment::tmp_segment_iterator::next()
{
	SEGMENT_LOCAL_LOCK(seg);

	if (current == &seg->list_head)
		return 0;

	element *old = current;
	current = current->get_next();

	return sc_addr_ll(old);
}

bool tmp_segment::tmp_segment_iterator::is_over()
{
	SEGMENT_LOCAL_LOCK(seg);
	return current == &seg->list_head;
}

tmp_segment::tmp_links_iterator::tmp_links_iterator(tmp_segment *seg) : tmp_segment_iterator(seg)
{
	SEGMENT_LOCAL_LOCK(seg);
	if (!seg->is_link(sc_addr_ll(current)))
		current = static_cast<element *>(&seg->list_head);
}

sc_addr_ll tmp_segment::tmp_links_iterator::next()
{
	SEGMENT_LOCAL_LOCK(seg);

	sc_addr_ll rv = tmp_segment_iterator::next();

	if (!rv)
		return rv;

	if (!seg->is_link(sc_addr_ll(current)))
		current = static_cast<element *>(&seg->list_head);

	return rv;
}

int tmp_segment::get_in_qnt(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;
	return ELEMENT_FROM_ADDR(addr)->in_qnt;
}

int tmp_segment::get_out_qnt(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;
	return ELEMENT_FROM_ADDR(addr)->out_qnt;
}

sc_type tmp_segment::get_type(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to get_type for link.");

	return el->type;
}

sc_retval tmp_segment::change_type(sc_addr_ll addr, sc_type type)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to change_type for link.");

	el->type = type;

	return RV_OK;
}

bool tmp_segment::is_link(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;
	return ELEMENT_FROM_ADDR(addr)->is_link();
}

sc_addr_ll tmp_segment::create_link(const sc_string &target_uri)
{
	element *e = new element(SC_EMPTY, TMP_LINK);

	CUR_SEGMENT_LOCK_SECTION {
		list_head.append(e);
		e->idtf = target_uri;
	}

	return sc_addr_ll(e);
}

sc_string tmp_segment::get_link_target(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);

	SC_ASSERTS(el->is_link(), "Try to get_link_target for non-link element.");

	return el->idtf;
}

void tmp_segment::set_link_target(sc_addr_ll addr, const sc_string &new_target)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);

	SC_ASSERTS(el->is_link(), "Try to set_link_target for non-link element.");

	el->idtf = new_target;
}

sc_segment::iterator *tmp_segment::create_link_iterator()
{
	return new tmp_links_iterator(this);
}

sc_addr_ll tmp_segment::gen_el(sc_type type)
{
	CUR_SEGMENT_LOCAL_LOCK;
	return sc_addr_ll(create_element(type));
}

void tmp_segment::erase_el(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	
	if (!(el->get_flags() & (TMP_LINK|TMP_MISSING_IDTF)))
		deregister_idtf(el->idtf);

	cindex.clear(addr, el->cont_signature);

	delete el;
}

sc_addr_ll tmp_segment::get_beg(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);

	SC_ASSERTF(el->type & SC_CONNECTOR_MASK,
		"Try to get_beg for non-connector of type '%s'.", SC_TYPEc(el->type));

	return sc_addr_ll(el->beg);
}

sc_addr_ll tmp_segment::get_end(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);

	SC_ASSERTF(el->type & SC_CONNECTOR_MASK,
		"Try to get_end for non-connector of type '%s'.", SC_TYPEc(el->type));

	return sc_addr_ll(el->end);
}

sc_retval tmp_segment::set_beg(sc_addr_ll addr, sc_addr_ll beg)
{
	CUR_SEGMENT_LOCAL_LOCK;
	element *a = ELEMENT_FROM_ADDR(addr);

	if (a->beg)
		detach_output_arc(sc_addr_ll(a->beg), addr);

	a->beg = 0;
	a->beg_hint = 0;

	if (beg) {
		attach_output_arc(beg, addr);
		a->beg = ELEMENT_FROM_ADDR(beg);
	}

	return RV_OK;
}

sc_retval tmp_segment::set_end(sc_addr_ll addr, sc_addr_ll end)
{
	CUR_SEGMENT_LOCAL_LOCK;
	element *a = ELEMENT_FROM_ADDR(addr);

	if (a->end)
		detach_input_arc(sc_addr_ll(a->end), addr);

	a->end = 0;
	a->end_hint = 0;

	if (end) {
		attach_input_arc(end, addr);
		a->end = ELEMENT_FROM_ADDR(end);
	}

	return RV_OK;
}

sc_addr tmp_segment::get_element_addr(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;
	return ELEMENT_FROM_ADDR(addr)->addr;
}

void tmp_segment::set_element_addr(sc_addr_ll ll, sc_addr addr)
{
	CUR_SEGMENT_LOCAL_LOCK;
	ELEMENT_FROM_ADDR(ll)->addr = addr;
}

sc_segment::iterator *tmp_segment::create_iterator_on_elements() 
{
	return new tmp_segment_iterator(this);
}

Content tmp_segment::get_content(sc_addr_ll addr) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to get_content for link.");

	return el->cont;
}

const Content *tmp_segment::get_content_const(sc_addr_ll addr) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to get_content_const for link.");

	return &(el->cont);
}

sc_retval tmp_segment::set_content(sc_addr_ll addr, const Content &c) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to set_content for link.");

	el->cont = c;
	update_cindex(cindex, el);

	return RV_OK;
}

sc_retval tmp_segment::erase_content(sc_addr_ll addr) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to set_content for link.");

	el->cont.eraseCont();
	update_cindex(cindex, el);

	return RV_OK;
}

bool tmp_segment::find_by_content(const Content &content, ll_list &result)
{
	const ll_list& l = cindex.find(content);
	if (l.empty()) {
		return false;
	} else {
		std::copy(l.begin(), l.end(), std::back_inserter(result));
		return true;
	}
}

sc_string tmp_segment::get_idtf(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);

	SC_ASSERTS(!el->is_link(), "Try to get_idtf for link.");

	if (!el->has_idtf())
		generate_idtf(addr);

	return el->idtf;
}

#ifdef WIN32
	#define snprintf _snprintf
#endif 

/**
 * Устанавливает для элемента уникальный идентификатор. 
 */
sc_retval tmp_segment::generate_idtf(sc_addr_ll addr) 
{
	static unsigned counter = 0;
	static char buf[25]; // 20 - is maximal length of string representation
	// of 64bit int (decimal)
	sc_string str = "@@";
	str.reserve(3+37+9);
	str += repo->current_tmp_prefix;
	str += "@";
	snprintf(buf, sizeof(buf), "%x", counter++);
	str += buf;
	if (!register_idtf(str, addr)) {
		SC_ABORT();
	} 

	ELEMENT_FROM_ADDR(addr)->set_idtf(str);

	return RV_OK;
}

sc_retval tmp_segment::has_erased_idtf(sc_addr_ll addr) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);
	SC_ASSERTS(!el->is_link(), "Try to has_erased_idtf for link.");

	if (!el->has_idtf())
		return RV_OK;
	else
		return RV_ELSE_GEN;
}

sc_retval tmp_segment::set_idtf(sc_addr_ll addr_ll, const sc_string &idtf) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *e = ELEMENT_FROM_ADDR(addr_ll);

	SC_ASSERTS(!e->is_link(), "Not permit set_idtf for link.");

	if (idtf.empty())
		return erase_idtf(addr_ll);

	if (e->has_idtf() && e->idtf == idtf)
		return RV_OK;

	if (register_idtf(idtf, addr_ll)) {
		if (e->has_idtf())
			deregister_idtf(e->idtf);

		e->set_idtf(idtf);

		return RV_OK;
	} else {
		//
		// Some element already has this idtf.
		//
		return RV_ERR_GEN;
	}
}

sc_retval tmp_segment::erase_idtf(sc_addr_ll addr)
{
	CUR_SEGMENT_LOCAL_LOCK;

	element *el = ELEMENT_FROM_ADDR(addr);

	SC_ASSERTS(!el->is_link(), "Try to erase_idtf for link.");

	if (el->has_idtf()) {
		deregister_idtf(el->idtf);
		el->set_flags(TMP_MISSING_IDTF);
	}

	return RV_OK;
}

sc_addr_ll tmp_segment::find_by_idtf(const sc_string &idtf) 
{
	CUR_SEGMENT_LOCAL_LOCK;

	my_map::iterator iter = idtf_map.find(idtf);
	if (iter == idtf_map.end())
		return SCADDR_LL_NIL;
	else
		return sc_addr_ll(iter->second);
}

class tmp_iterator : public sc_iterator 
{
	element_link *curr;
	sc_addr curr_addr;
	element_link_listhead *first;
	tmp_segment *seg;

public:
	DECLARE_FIXALLOCER(tmp_iterator)
	
	tmp_iterator(tmp_segment *_seg, element_link_listhead *_first) : seg(_seg), first(_first) 
	{
		curr = first->get_next();
		if (curr != first) {
			curr_addr = curr->value->addr;
			if (curr_addr) {
				curr_addr->ref();
				curr_addr->seg->ref();
			}

			curr->ref();
			
			if (curr->dead) {
				next();
			} else if (curr_addr) {
				if (curr_addr->is_dead()) {
					next();
				} 
				// else
				// 	curr_addr->seg->ref();
			} else {
				next();
			} 
		}
		PREFETCH(curr->next);
	}

	sc_retval next() 
	{
		if (is_over()) {
			return RV_ELSE_GEN;
		} 
		element_link *next;
		sc_addr next_addr = 0; //just to remove warning.
		// It's okay to left this uninitialized
		next = curr->get_next();
		while (next != first && (next->dead || !(next_addr = next->value->addr))) {
			next = next->get_next();
		} 
		curr->unref();
		curr_addr->seg->unref();
		curr_addr->unref();
		curr = next;
		curr_addr = next_addr;
		if (curr != first) {
			curr->ref();
			curr_addr->ref();
			curr_addr->seg->ref();
			PREFETCH(curr->next);
			return RV_OK;
		}
		return RV_ELSE_GEN;
	}

	sc_addr value(sc_uint num) 
	{
		if (is_over() || num > 0) {
			return SCADDR_NIL;
		} 
		return curr_addr;
	}

	bool is_over() 
	{
		return first == curr;
	}

	~tmp_iterator() 
	{
		//		printf("simple_list_iterator(%p,%p) at %p dies\n",from,first,this);
		if (!is_over()) {
			curr_addr->seg->unref();
			curr_addr->unref();
			curr->unref();
		}
	}
};

DEFINE_FIXALLOCER(tmp_iterator)

sc_iterator *tmp_segment::create_input_arcs_iterator(sc_addr_ll addr) 
{
	return new tmp_iterator(this, &(ELEMENT_FROM_ADDR(addr)->input));
}

sc_iterator *tmp_segment::create_output_arcs_iterator(sc_addr_ll addr) 
{
	return new tmp_iterator(this, &(ELEMENT_FROM_ADDR(addr)->output));
}
