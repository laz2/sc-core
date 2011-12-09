
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
 * @file advanced_activity.h
 * @brief Расширение для класса sc_activity. 
 * @ingroup libsc
 */

#ifndef __ADVANCED_ACTIVITY_H__
#define __ADVANCED_ACTIVITY_H__

#include "libsc.h"

/**
 * @brief Базовый класс расширенной sc-активности.
 */
class LIBSC_API basic_advanced_activity : public sc_activity 
{
protected:
	sc_addr sign;

public:
	basic_advanced_activity();
	~basic_advanced_activity();

	sc_retval init(sc_addr _this);
	void done();
	
	/**
	 * @return знак расширенной sc-активности.
	 */
	sc_addr get_sign() 
	{
		return sign;
	}
};

/**
 * @brief Сегмент по умолчанию, где создаються знаки расширенных активностей.
 */
extern LIBSC_API 
sc_segment *libsc_default_creator_segment_var;

/**
 * @brief Шаблонная структура-примесь, которая дает возможность создать 
 * саму активность и ее знак в нужном сегменте.
 * @tparam T класс активности.
 * @tparam var сегмент по умолчанию, в котором будет создаваться знак активности.
 */
template <class T, sc_segment **var = &libsc_default_creator_segment_var> 
struct advanced_activity_creator 
{
	/**
	 * Статический метод для создания активности и ее знака в нужном сегменте.
	 * @param seg сегмент, в котором будет создан знак активности. Если 0, тогда 
	 * активность будет создана в сегменте по умолчанию.
	 */
	static T *create(sc_segment *seg = 0) 
	{
		T *rv;
		if (!seg) {
			seg =  *var;
		}

		if (!seg) {
			return 0;
		}

		sc_addr sign = system_session->create_el(seg, SC_N_CONST);
		if (!sign) {
			return 0;
		}

		try {
			rv = new T();
		} catch (std::exception &exc) {
			system_session->erase_el(sign);
			throw(exc);
		}

		if (system_session->reimplement(sign, rv)) {
			system_session->erase_el(sign);
			delete rv;
			return 0;
		}
		return rv;
	}

	/**
	 * Статический метод для получения объекта активности по ее знаку.
	 */
	static T *get(sc_addr sign) 
	{
		return (T*)(sign->activity);
	}
};

/**
 * @brief Шаблонный класс расширенной активности. Позволяет создать активность и ее знак в нужном сегменте.
 * @note #RAA_DEFINE_DISPATCH и #RAA_DISPATCH позволяют устанавливать обработчики для событий.
 *
 * Пример определения класса расширенной активности, которая обрабатывает 2 события:
 * @code
 * DECLARE_SC_ATOM(event1);
 * DECLARE_SC_ATOM(event2); 
 *
 * class MyActivity : public advanced_activity<MyActivity> 
 * {
 * public:
 *    MyActivity();
 *    ~MyActivity();
 *    sc_retval init(sc_addr _this);
 *    void done();
 *    RAA_DEFINE_DISPATCH() {
 *       RAA_DISPATCH(PROCESS_ERROR_EVENT, catchEvent1);
 *       RAA_DISPATCH(PROCESS_STEP_EVENT, catchEvent2);
 *    }
 * private:
 *    sc_retval catchEvent1(sc_addr p1, sc_addr p2);
 *    sc_retval catchEvent2(sc_addr p1, sc_addr p2);
 * };
 * 
 * @endcode
 *
 *
 * @tparam T класс активности.
 * @tparam var сегмент по умолчанию, в котором будет создаваться знак активности.
 */
template <class T, sc_segment **var = &libsc_default_creator_segment_var> 
struct advanced_activity : public basic_advanced_activity, public advanced_activity_creator<T, var>  
{
	sc_retval activate(sc_session *_s, sc_addr _this, sc_addr param, sc_addr __p2, sc_addr __p3) 
	{
		int handled = 0;
		sc_retval rv = RV_OK;
		T::__dispatcher(static_cast<T *> (this), param, &rv, &handled, __p2, __p3);
		
		if (handled) {
			return rv;
		}

		return default_message(param, __p2, __p3);
	}

	virtual sc_retval default_message(sc_addr p1, sc_addr p2, sc_addr p3) 
	{
		SC_CRITICAL_RETURN("Unknown message sent to activity!\n", RV_ERR_GEN);
	}

	typedef T __dispatcher_klass;

/**
 * @brief Макрос, который начинает блок объявления обработчиков событий.
 */
#define RAA_DEFINE_DISPATCH() \
	static void __dispatcher(__dispatcher_klass *_this, sc_addr __p1, sc_retval *rv, int *handled, sc_addr p1, sc_addr p2)
	
/**
 * @brief Макрос определяет, что для обработки события @p kn будет использован метод \@ method.
 * @param kn sc-адрес события.
 * @param method Метод-обработчик, должен иметь два параметра типа #sc_addr и возвращать #sc_retval.
 */
#define RAA_DISPATCH(kn, method) \
	do { \
		if (__p1 == (kn)) { \
			*rv = (_this->method)(p1, p2); \
			*handled = 1; \
		} \
	} while(0)
};

#endif // __ADVANCED_ACTIVITY_H__
