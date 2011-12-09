
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
 * @file sc_queue.h
 * @brief Functions and classes for working with queue in sc-memory.
 * @ingroup libsc
 */

#ifndef __SC_QUEUE_H_INCLUDED__
#define __SC_QUEUE_H_INCLUDED__

/// @addtogroup libsc
/// @{

#include "libsc.h"

/**
 * @brief Функция для создания sc-очереди.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param[out] queue sc-адрес созданной sc-очереди.
 * @param seg sc-сегмент, в котором будет происходить создание необходимых структур.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_new(sc_session *session, sc_addr &queue, sc_segment *seg = 0);

/**
 * @brief Функция для удаления sc-очереди.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param queue sc-адрес sc-очереди.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_delete(sc_session *session, sc_addr queue);

/**
 * @brief Функция для добавления @p element в sc-очередь @p queue.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param queue sc-адрес sc-очереди.
 * @param element sc-адрес элемента, который будет добавлен в sc-очередь.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_append(sc_session *session, sc_addr queue, sc_addr element);

/**
 * @brief Функция для просмотра головного элемент sc-очереди.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param queue sc-адрес sc-очереди.
 * @param[out] element sc-адрес головного элемента sc-очереди.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_peek(sc_session *session, sc_addr queue, sc_addr &element);

/**
 * @brief Функция для извлечения головного элемент из sc-очереди.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param queue sc-адрес sc-очереди.
 * @param[out] element sc-адрес головного элемента sc-очереди.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_get(sc_session *session, sc_addr queue, sc_addr &element);

/**
 * @brief Функция для установки переданного элемента перед головой sc-очереди.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param queue sc-адрес sc-очереди.
 * @param element sc-адрес элемента, который будет установлен перед головой.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_prepend(sc_session *session, sc_addr queue, sc_addr element);

/**
 * @brief Функция для удаления элемента из sc-очереди.
 * @param session sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param queue sc-адрес sc-очереди.
 * @param element sc-адрес элемента, который будет удален.
 * @return sc-код возврата.
 */
LIBSC_API sc_retval sc_queue_remove(sc_session *session, sc_addr queue, sc_addr element);

/// @}

#endif // __SC_QUEUE_H_INCLUDED__
