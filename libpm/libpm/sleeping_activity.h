
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


#ifndef SLEEPING_ACTIVITY_H
#define SLEEPING_ACTIVITY_H
#include <libsc/advanced_activity.h>
#include "pm_process.h"

class basic_sleep_marshaller {
public:
	virtual void marshall(sc_addr p1, sc_addr p2) = 0;
	virtual ~basic_sleep_marshaller();
};

template <class T>
class sleeping_activity_marshaller : public basic_sleep_marshaller {
	class __sleeping_activity_stub_class {};
	typedef void (T::*message_method)(sc_addr p1, sc_addr p2);
	T *obj;
	message_method method;
public:
	sleeping_activity_marshaller(T *_obj, message_method _method)
		: obj(_obj), method(_method) {}
	void marshall(sc_addr p1, sc_addr p2)
	{
		(obj->*method)(p1,p2);
	}
};

class sleeping_caller : public pm_process {
	unsigned magic;
	basic_sleep_marshaller *marshaller;
	sc_addr p1;
	sc_addr p2;
	sc_retval &rv;
	sc_addr caller;
	void wake_caller();
public:
	sleeping_caller(basic_sleep_marshaller *_m, sc_addr _p1, sc_addr _p2, sc_retval &_rv);
	~sleeping_caller();
	void run();

	static void do_return(sc_retval retval, bool do_not_wake = false);
	static sc_retval call(basic_sleep_marshaller *m, sc_addr p1, sc_addr p2);
	template <class T>
	static sc_retval call(T *obj, void (T::*method)(sc_addr, sc_addr), sc_addr p1, sc_addr p2)
	{
		sleeping_activity_marshaller<T> marshaller(obj, method);
		return sleeping_caller::call(&marshaller, p1, p2);
	}
	static sleeping_caller *current();
};

class __sleeping_advanced_activity {
public:
	void sleeping_return(sc_retval rv);
};

template<class T, sc_segment **var = &libsc_default_creator_segment_var>
class sleeping_advanced_activity
	: public advanced_activity<T, var>
	, public __sleeping_advanced_activity 
{
public:
#define RAA_SLEEP_DISPATCH(kn,method) \
		do { \
			if (__p1 == (kn)) { \
				*rv = sleeping_caller::call(_this, &__dispatcher_klass::method, p1, p2); \
				*handled = 1; \
			} \
		} while(0)
};

#endif
