
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
 * @file sc_event_multi.h
 * @brief Механизм для подписывания на несколько событий.
 * @ingroup libsc
 */

#ifndef __SC_EVENT_MULTI__
#define __SC_EVENT_MULTI__
	
#include "libsc.h"    
#include <list>

/**
 * @brief Базовый класс для классов, 
 * которым необходимо подписывание на несколько событий одновременно.
 */
class LIBSC_API sc_event_multi 
{
	class sc_event_wait: public sc_wait 
	{
		sc_event_multi *parent;
	public:
		sc_event_wait(sc_event_multi *_parent) 
		{
			parent = _parent;
		}
		bool activate(sc_wait_type type, sc_param *prm, int len) 
		{
			return parent->activate(type, prm, len);
		}
		~sc_event_wait(){}
	};

	typedef std::list <sc_event_wait *> my_list_type;
	my_list_type list;
	// will work only once
public:
	/**
	 * @brief Метод отмены подписи на все события.
	 */
	void detach_all();

	/**
	 * @brief Метод подписи на событие.
	 * @param s сессия, при помощи которой будет происходить подпись.
	 * @param type тип события.
	 * @param prm указатель на массив параметров для подписи на событие.
	 * @param len количество параметров.
	 */
	sc_retval attach_to(sc_session *s, sc_wait_type type, sc_param prm[], int len);

	virtual ~sc_event_multi() 
	{
		detach_all();
	}

	/**
	 * @brief Метод вызывается, когда происходит событие, на которое есть подпись.
	 * @param type тип события.
	 * @param prm указатель на массив переданных параметров.
	 * @param len количество переданных параметров.
	 * @return true - событие было обработано.
	 * @return false - событие не было обработано.
	 */
	virtual bool activate(sc_wait_type type, sc_param *prm, int len) = 0;
};

#endif // __SC_EVENT_MULTI__
