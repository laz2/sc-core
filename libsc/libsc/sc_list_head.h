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


#ifndef __LIBSC_SC_LIST_HEAD_H_INCLUDED__
#define __LIBSC_SC_LIST_HEAD_H_INCLUDED__

// Implementation of nice linked list. This is hack, but should work
// Be vary careful using it with multiple inheritance
class LIBSC_API __sc_listhead {
public:
	__sc_listhead *next,*prev;
	__sc_listhead() {next = prev = this;}
	__sc_listhead(__sc_listhead *_next,__sc_listhead *_prev)
	{
		next = _next; prev = _prev;
	}
	void remove()
	{
		prev->next = next;
		next->prev = prev;
		next = prev = this;
	}
	void append(__sc_listhead *_new)
	{
		assert(_new->prev == _new && _new->next == _new);
		_new->prev = this;
		next->prev = _new;
		_new->next = next;
		next = _new;
	}
	void prepend(__sc_listhead *_new)
	{
		assert(_new->prev == _new && _new->next == _new);
		_new->next = this;
		prev->next = _new;
		_new->prev = prev;
		prev = _new;
	}
	~__sc_listhead() {remove();}
};

// this is subtle. Should be used for childs with virtual methods
class LIBSC_API __sc_listhead_virtual {
public:
	__sc_listhead_virtual *next,*prev;
	__sc_listhead_virtual() {next = prev = this;}
	__sc_listhead_virtual(__sc_listhead_virtual *_next,__sc_listhead_virtual *_prev)
	{
		next = _next; prev = _prev;
	}
	void remove()
	{
		prev->next = next;
		next->prev = prev;
		next = prev = this;
	}
	void append(__sc_listhead_virtual *_new)
	{
		if (_new->prev != _new)
			remove();
		_new->prev = this;
		next->prev = _new;
		_new->next = next;
		next = _new;
	}
	void prepend(__sc_listhead_virtual *_new)
	{
		assert(_new->prev == _new && _new->next == _new);
		_new->next = this;
		prev->next = _new;
		_new->prev = prev;
		prev = _new;
	}
	virtual ~__sc_listhead_virtual() {remove();}
};

#define SCLISTHEAD_DEFINE_CHILD_OPS(child_class) \
	child_class *get_prev() {return (child_class *)prev;} \
	child_class *get_next() {return (child_class *)next;}

#define DECLARE_LIST_HEAD(struct_or_class,child_class) \
	struct_or_class child_class; \
struct LIBSC_API child_class##_listhead : public __sc_listhead { \
	SCLISTHEAD_DEFINE_CHILD_OPS(child_class) \
};

// for childs with virtual methods
#define DECLARE_LIST_HEAD_VIRTUAL(struct_or_class,child_class) \
	struct_or_class child_class; \
struct LIBSC_API child_class##_listhead : public __sc_listhead_virtual { \
	SCLISTHEAD_DEFINE_CHILD_OPS(child_class) \
};

#endif // __LIBSC_SC_LIST_HEAD_H_INCLUDED__
