
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


#ifndef __SAFE_LIST_H__
#define __SAFE_LIST_H__
//#include <iostream>

// inherit very carefully
template <class T>
class safe_list {
public:

class safe_iterator;


struct safe_item_base {
public:
	safe_item_base *next,*prev;
	safe_item_base()
	{
		next = prev = this;
	}
	void remove()
	{
		next->prev = prev;
		prev->next = next;
		next = prev = this;
	}
	void append(safe_item_base *item)
	{
		item->next = next;
		item->prev = this;
		next->prev = item;
		next = item;
	}
	void prepend(safe_item_base *item)
	{
		item->prev = prev;
		item->next = this;
		prev->next = item;
		prev = item;
	}
};

class safe_item: public safe_item_base {
public:
	int refcnt;
	bool dead;

	safe_item *_prev()
	{
		return static_cast<safe_item *>(safe_item_base::prev);
	}
	safe_item *_next()
	{
		return static_cast<safe_item *>(safe_item_base::next);
	}

public:
//	friend class safe_list<T>::safe_iterator;

	T data;

	safe_item() : refcnt(1), dead(false) {}
	safe_item(T _data) : refcnt(1), dead(false), data(_data) {}
	// should not be deleted directly by clients or by iterators
	~safe_item()
	{
		//std::cout << "safe_item(" << data << ") is dying\n";
	}
	bool is_dead() const
	{
		return dead;
	}
	void ref()
	{
		refcnt++;
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
		dead = true;
		unref();
	}
	safe_item *get_next() const
	{
		safe_item *item = _next();
		while (item != this && item->is_dead())
			item=item->_next();
		return item;
	}

	safe_item *get_prev() const
	{
		safe_item *item = _prev();
		while (item != this && item->is_dead())
			item=item->_prev();
		return item;
	}
	void remove()
	{
		safe_item_base::next->prev = safe_item_base::prev;
		safe_item_base::prev->next = safe_item_base::next;
		safe_item_base::next = safe_item_base::prev = this;
	}
	void append(safe_item_base *item)
	{
		item->next = safe_item_base::next;
		item->prev = this;
		safe_item_base::next->prev = item;
		safe_item_base::next = item;
	}
	void prepend(safe_item_base *item)
	{
		item->prev = safe_item_base::prev;
		item->next = this;
		safe_item_base::prev->next = item;
		safe_item_base::prev = item;
	}
};

// this class is model (?) of BidirecionalIterator concept of STL
//  plus additional safety propreries (invalidation)
// TODO: optimize heavily
class	safe_iterator {
public:
	safe_item *ptr;
	safe_list *list;

void	unref_ptr()
{
	if (ptr != &list->head)
		ptr->unref();
}
void	ref_ptr()
{
	if (ptr != &list->head)
		ptr->ref();
}
void	goto_next()
{
	safe_item *_ptr = ptr;
	ptr = ptr->_next();
	while (ptr != &list->head && ptr->is_dead())
		ptr = ptr->_next();
	ref_ptr();
	if (_ptr != &list->head)
		_ptr->unref();
}
void	goto_prev()
{
	safe_item *_ptr = ptr;
	ptr = ptr->_prev();
	while (ptr != &list->head && ptr->is_dead())
		ptr = ptr->_prev();
	ref_ptr();
	if (_ptr != &list->head)
		_ptr->unref();
}
public:

friend class safe_list;

	safe_iterator(safe_list *_list,safe_item_base *_ptr)
	{
		list = _list;
		ptr = static_cast<safe_item *>(_ptr);
		ref_ptr();
	}
	safe_iterator(safe_list *_list) : list(_list)
	{
		list->refcnt++;
		ptr = static_cast <safe_item *>(list->head.next);
		while (ptr != &list->head && ptr->is_dead())
			ptr = ptr->_next();
		ref_ptr();
	}
	safe_iterator(const safe_iterator &iter)
	{
		*this = iter;
	}
	safe_iterator &operator =(const safe_iterator &iter)
	{
		ptr = iter.ptr;
		list = iter.list;
		if (ptr != &list->head)
			ptr->ref();
		list->refcnt++;
		return *this;
	}
	~safe_iterator()
	{
		list->refcnt--;
		unref_ptr();
	}
	safe_iterator operator ++(int)
	{
		safe_iterator cpy = *this;
		goto_next();
		return cpy;
	}
	safe_iterator &operator ++()
	{
		goto_next();
		return *this;
	}
	safe_iterator operator --(int)
	{
		safe_iterator cpy = *this;
		goto_prev();
		return cpy;
	}
	safe_iterator &operator --()
	{
		goto_prev();
		return *this;
	}
	T &operator *() const
	{
		return ptr->data;
	}
	T *operator ->() const
	{
		return &ptr->data;
	}
	bool operator ==(const safe_iterator &iter)
	{
		return list == iter.list && ptr == iter.ptr;
	}
	bool operator !=(const safe_iterator &iter)
	{
		return !(*this == iter);
	}

	//plus deletion
	void remove()
	{
		if (ptr != &list->head)
			ptr->die();
	}
};

friend class safe_iterator;
private:
safe_item_base head;
int	refcnt;

public:

typedef safe_iterator iterator;
typedef const safe_iterator const_iterator;

safe_list() : refcnt(1) {}

safe_iterator begin()
{
	return safe_iterator(this);
}
safe_iterator end()
{
	return safe_iterator(this,&head);
}

// all live iterators will become wrong
~safe_list()
{
	safe_item *ptr = static_cast<safe_item *>(head.next);
	while (ptr != &head) {
		safe_item *next = static_cast<safe_item *>(ptr->next);
		delete ptr;
		ptr = next;
	}
}

// for dynamically allocated list
void die()
{
	safe_iterator iter = begin();
	while (iter != end())
		(iter++).remove();
	if (!--refcnt)
		delete this;
}

void append(safe_item *item)
{
	head.prepend(item);
}
void prepend(safe_item *item)
{
	head.append(item);
}
void remove(safe_item *item)
{
	item->die();
}
void insert(safe_item *before,safe_item *item)
{
	before->prepend(item);
}
void insert_after(safe_item *after,safe_item *item)
{
	after->append(item);
}

void insert(const safe_iterator &iter,T data)
{
	safe_item *ptr = new safe_item(data);
	iter.ptr->prepend(ptr);
}

};

#endif //__SAFE_LIST_H__
