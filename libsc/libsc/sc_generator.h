
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
 * @file sc_generator.h
 * @brief Класс генератора конструкций в sc-памяти.
 * @ingroup libsc
 */

/**
 * @addtogroup libsc
 * @{
 */

#ifndef __SC_GENERATOR_H__
#define __SC_GENERATOR_H__

#include "libsc.h"

class LIBSC_API sc_generator_proxy
{
protected:
	sc_session *session;

public:
	sc_generator_proxy(sc_session *_session) : session(_session) {}

	virtual ~sc_generator_proxy();

	sc_session *get_session()
	{
		return session;
	}

	virtual sc_addr create_el(sc_segment *seg, sc_type) = 0;
	virtual sc_retval erase_el(sc_addr el) = 0;
	virtual sc_retval set_beg(sc_addr arc, sc_addr beg) = 0;
	virtual sc_retval set_end(sc_addr arc, sc_addr end) = 0;
	virtual sc_retval set_idtf(sc_addr arc, sc_string idtf) = 0;
	virtual sc_retval set_content(sc_addr arc, const Content &c) = 0;
	virtual sc_retval change_type(sc_addr arc, sc_type type) = 0;
};

class sc_generator_session_proxy : public sc_generator_proxy
{
public:
	sc_generator_session_proxy(sc_session *session);
	~sc_generator_session_proxy();

	sc_addr create_el(sc_segment *seg, sc_type type);
	sc_retval erase_el(sc_addr el);
	sc_retval set_beg(sc_addr arc, sc_addr beg);
	sc_retval set_end(sc_addr arc, sc_addr end);
	sc_retval set_idtf(sc_addr arc, sc_string idtf);
	sc_retval set_content(sc_addr arc, const Content &c);
	sc_retval change_type(sc_addr arc, sc_type type);
};

// TODO: rework sc_generator to work through generator proxy
struct sc_generator_state;

/**
 * @brief Класс для генерации элементов в sc-памяти.
 */
class LIBSC_API sc_generator
{
	sc_segment *active_segment;
	sc_session *session;
	// lets hide generator state
	// minus is additional malloc
	struct sc_generator_state *state;
	sc_addr generate_internal(const char *idtf, sc_type type, sc_segment *seg);
	sc_addr generate_find_internal(const char *idtf, sc_type type, sc_segment *seg);
	void push_state(bool);
	sc_addr internal_gen_arc(sc_addr from, sc_addr to, sc_type arc_type, sc_segment *seg);
	sc_addr __element(sc_addr);
	sc_generator_proxy *proxy;
	bool own_proxy;
	sc_retval gen3_f_a_f(sc_addr beg, sc_addr *arc, sc_segment *seg, sc_type type, sc_addr end);

public:
	sc_type arc_type; /**< Тип генерируемой дуги. */
	sc_type element_type; /**< Тип генерируемого элемента. */
	sc_type set_arc_type; /**< Тип генерируемой дуги, которая определяет принадлежность к множеству. */
	sc_type system_arc_type; /**< Тип генерируемой дуги, которая определяет вхождение в контур. */
	sc_type attr_arc_type; /**< Тип генерируемой дуги атрибута. */

	/**
	 * @brief Конструктор, который настривает генератор на работу с сессией @p _session
	 * и для генерации в сегменте @p segment.
	 */
	sc_generator(sc_session *_session, sc_segment *segment);

	/**
	 * @brief Конструктор, который настривает генератор на работу через @p proxy
	 * и для генерации в сегменте @p segment.
	 */
	sc_generator(sc_generator_proxy *proxy, sc_segment *segment);

	~sc_generator();

	/**
	 * @brief Возвращает сессию, с использованием которой работает генератор.
	 */
	sc_session *get_session()
	{
		return session;
	}

	/**
	 * @brief Устанавливает сегмент @p seg в качестве сегмента для генерации.
	 * @return Сегмент, в котором происходила генерация до изменения.
	 */
	sc_segment *set_active_segment(sc_segment *seg)
	{
		sc_segment *prev = active_segment;
		active_segment = seg;
		return prev;
	}

	/**
	 * @brief Устанавливает содержимое @p cont для @p el.
	 */
	sc_retval set_content(sc_addr el, const Content &cont);

	/**
	 * @brief Устанавливает идентификатор @p str для @p el.
	 */
	sc_retval set_idtf(sc_addr el, sc_string str);

	/**
	 * @brief Изменяет тип @p el на @p type.
	 */
	sc_retval change_type(sc_addr el, sc_type type);

	/**
	 * @brief Возвращает сегмент, в котором происходит генерация.
	 */
	sc_segment *get_active_segment()
	{
		return active_segment;
	}

	/**
	 * @brief Переводит генератор в состояние генерации множества, знаком которого является @p set.
	 * @return Знак множества.
	 */
	sc_addr enter_set(sc_addr set);

	/**
	 * @brief Переводит генератор в состояние генерации множества и генерирует знак множества.
	 * @param idtf Индентификатор. Если нет идентификатора, передать "".
	 * @param type Тип. Если не указан используется sc_generator#element_type.
	 * @param segment Сегмент для генерации. Если не указан используется sc_generator#get_active_segment.
	 * @return Знак множества.
	 */
	sc_addr enter_set(const char *idtf, sc_type type = 0, sc_segment *segment = 0);

	/**
	 * @brief Завершает генерацию текущего множества и передводит генератор в прешествующее состояние.
	 */
	sc_addr leave_set();

	/**
	 * @brief Переводит генератор в состояние генерации контура, знаком которого является @p sys_sign.
	 * @return Знак контура.
	 */
	sc_addr enter_system(sc_addr sys_sign);

	/**
	 * @brief Переводит генератор в состояние генерации контура и генерирует знак контура.
	 * @param idtf Индентификатор. Если нет идентификатора, передать "".
	 * @param type Тип. Если не указан используется sc_generator#element_type.
	 * @param segment Сегмент для генерации. Если не указан используется sc_generator#get_active_segment.
	 * @return Знак контура.
	 */
	sc_addr enter_system(const char *idtf, sc_type type = 0, sc_segment *segment = 0);

	/**
	 * @brief Завершает генерацию текущего контура и переводит генератор в прешествующее состояние.
	 */
	sc_addr leave_system();

	/**
	 * @brief Указывает, что необходима генерация дуги, которая будет входить в следующий генерируемый элемент,
	 * а выходить из текущего элемента.
	 */
	void arc();

	/**
	 * @brief Указывает атрибут, которым должна уточняться генерируемая дуга.
	 * @return атрибут.
	 */
	sc_addr attr(sc_addr attr);

	/**
	 * @brief Создает атрибут, которым должна уточняться генерируемая дуга.
	 * @param idtf Индентификатор. Если нет идентификатора, передать "".
	 * @param type Тип. Если не указан используется sc_generator#element_type.
	 * @param segment Сегмент для генерации. Если не указан используется sc_generator#get_active_segment.
	 * @return атрибут.
	 */
	sc_addr attr(const char *idtf, sc_type type = 0, sc_segment *segment = 0);

	/**
	 * @brief Устанавливает текущим элементов генератора @p el.
	 * @return Текущий элемент генератора.
	 */
	sc_addr element(sc_addr el);

	/**
	 * @see sc_generator#element.
	 */
	sc_generator &operator << (sc_addr el)
	{
		element(el);
		return  *this;
	}

	/**
	 * @brief Устанавливает текущим элементов генератора @p el.
	 * @return Текущий элемент генератора.
	 */
	sc_addr element(const char *idtf, sc_type type = 0, sc_segment *segment = 0);

	/**
	 * @brief Очищает текущее состояние.
	 */
	void finish();

	/**
	 * @brief Генерирует дугу или изменяет начало и конце существующей дуги с идентификатором @p idtf.
	 * @param from Начало дуги.
	 * @param to Конец дуги.
	 * @param idtf Индентификатор. Если нет идентификатора, передать "".
	 * @param type Тип. Если не указан используется sc_generator#arc_type.
	 * @param segment Сегмент для генерации. Если не указан используется sc_generator#get_active_segment.
	 * @return дуга.
	 */
	sc_addr gen_arc(sc_addr from, sc_addr to, const char *idtf, sc_type type = 0, sc_segment *segment = 0);

	/**
	 * @brief Завершает генерацию всех множеств и контуров.
	 */
	void leave_all();
};

/**
 * @brief Объявляет необходимые структуры для работы макросов, которые используют генератор.
 */
#define GEN_PREPARE() bool ____flag

/**
 * @brief Определяет блок генерации контура c использование генератора.
 * @param gen генератор, при помощи которого создается контур.
 * @param prm первый параметр для sc_generator#enter_system.
 *
 * @code
 * GEN_PREPARE();
 * sc_generator gen(session, seg);
 * GEN_SYSTEM(gen, "") {
 *  // Генерация элементов, которые будут входить в контур.
 * }
 * @endcode
 */
#define GEN_SYSTEM(gen,prm) \
	for (____flag = (gen.enter_system(prm), true);	\
								____flag;			\
								gen.leave_system(), ____flag=false)

/**
 * @brief Определяет блок генерации контура c использование генератора.
 * @param gen генератор, при помощи которого создается контур.
 * @param prm первый параметр для sc_generator#enter_system.
 * @param var имя переменной, которая получит в качестве значения узел, обозначающий генерируемого контура.
 */
#define GEN_SYSTEM_VAR(var,gen,prm) \
	for (____flag = ((var = gen.enter_system(prm)), true);	\
										____flag;			\
										gen.leave_system(), ____flag=false)

/**
 * @brief Определяет блок генерации множетсва c использование генератора.
 * @param gen генератор, при помощи которого создается множество.
 * @param prm первый параметр для sc_generator#enter_set.
 *
 * @code
 * GEN_PREPARE();
 * sc_generator gen(session, seg);
 * GEN_SET(gen, "") {
 *  // Генерация элементов, которые будут входить во множество.
 * }
 * @endcode
 */
#define GEN_SET(gen,prm) \
	for (____flag = (gen.enter_set(prm), true);	\
							____flag;			\
							gen.leave_set(), ____flag=false)

/**
 * @brief Определяет блок генерации множества c использование генератора.
 * @param gen генератор, при помощи которого создается множество.
 * @param prm первый параметр для sc_generator#enter_set.
 * @param var имя переменной, которая получит в качестве значения узел, обозначающий генерируемого множества.
 */
#define GEN_SET_VAR(var,gen,prm) \
	for (____flag = ((var = gen.enter_set(prm)), true);	\
									____flag;			\
									gen.leave_set(), ____flag=false)

#endif //__SC_GENERATOR_H__

/**@}*/
