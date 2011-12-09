
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
 * @file segment_utils.h
 * @brief Файл определений функций, которые помогают в работе с sc-сегментами.
 * @ingroup libsc
 */

#ifndef __SEGMENT_UTILS_H__
#define __SEGMENT_UTILS_H__

#include "libsc.h"

/**
 * @brief Функция для создания уникального сегмента.
 * @param s sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param base базовый uri, на основе которого будет происходить создание сегмента. Например, "/tmp/myseg".
 * @return sc-сегмент, если создание прошло успешно.
 * @return 0, если возникла ошибка.
 */
LIBSC_API sc_segment *create_unique_segment(sc_session *s,const sc_string &base);

/**
 * @brief Функция для создания уникальной директории.
 * @param s sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param base базовый uri, на основе которого будет происходить создание директории. Например, "/tmp/mydir".
 * @return имя созданной директории, если создание прошло успешно.
 * @return пустую строку, если возникла ошибка.
 */
LIBSC_API sc_string create_unique_dir(sc_session *s,const sc_string &base);

/**
 * @brief Функция проверяет, являеться ли @p sign знаком sc-сегмента.
 * @param s sc-сессия, при помощи которой будет происходить работа с sc-памятью.
 * @param sign проверяемый знак sc-сегмента.
 * @return Если @p sign - знак sc-сегмента, тогда sc-сегмент, иначе 0.
 */
LIBSC_API sc_segment *is_segment(sc_session *s,sc_addr sign);

/**
 * @brief Функция получает знак для sc-сегмента @p seg при помощи sc-сессии @p s.
 * @return знак sc-сегмента @p seg.
 */
LIBSC_API sc_addr seg2sign(sc_session *s,sc_segment *seg);

/**
 * @brief Функция получает для знака @p addr sc-сегмент, который он обозначает, при помощи sc-сессии @p s.
 * @return Если знак @p addr обозначает sc-сегмента, тогда возвращается этот sc-сегмент, иначе 0.
 */
LIBSC_API sc_segment *sign2seg(sc_session *s,sc_addr addr);

/**
 * @brief Функция при помощи sc-сессии @p s создает sc-сегмент по полному URI @p uri c созданием всех промежуточных директорий.
 * @return Если создание прошло успешно, то sc-сегмент, иначе 0.
 */
LIBSC_API sc_segment *create_segment_full_path(sc_session *s,const sc_string &uri);

/**
 * @brief Функция копирует при помощи sc-сессии @p s sc-сегмент @p orig по пути @p uri.
 * @note Использует для создания нового sc-сегмента #create_unique_segment.
 * @note Не копирует содержимое.
 * @return Если копирование прошло успешно, то созданную копию, иначе 0.
 */
LIBSC_API sc_segment *copy_segment(sc_session *s, sc_segment *orig, const sc_string &uri);

/**
 * @brief Функция копирует при помощи sc-сессии @p s sc-сегмент @p orig по пути @p uri. Так же копирует содержимое sc-элементов.
 * @note Использует для создания нового sc-сегмента #create_unique_segment.
 * @note Копирует содержимое.
 * @return Если копирование прошло успешно, то созданную копию, иначе 0.
 */
LIBSC_API sc_segment *copy_segment_content(sc_session *s,sc_segment *orig,const sc_string &pat);

/// Open segments for session @p s by its sign's from set @p segset.
/// If @p segs isn't 0 then add in it pointers to opened segments.
LIBSC_API
inline sc_retval open_segset(sc_session *s,sc_addr segset, std::set< sc_segment* >* segs=0);

/// Open segments for session @p s by its sign's from set @p segset.
/// Uses session @p root for accessing @p segset.
/// If @p segs isn't 0 then add in it pointers to opened segments.
LIBSC_API
sc_retval open_segset(sc_session *s, sc_addr segset, sc_session *root, std::set< sc_segment* >* segs=0);

LIBSC_API void mkdir_by_full_uri(sc_session *s, const sc_string &uri);

#endif // __SEGMENT_UTILS_H__
