
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
 * @file string_util.h
 * @brief Файл определений функций, которые помогают в работе с URI.
 * @ingroup libsc
 */

#ifndef __SC_STRING_UTILS_H__
#define __SC_STRING_UTILS_H__

#include "sc_types.h"
#include <assert.h>

/**
 * @brief Функция поиска в @p uri последнего символа '/', начиная с позиции @p start.
 * @note По умолчанию поиска начинается с конца @p uri.
 * @return Если '/' был найден, то его индекс в @p uri, иначе число меньше нуля.
 */
inline
int find_last_slash(const sc_string &uri,int start = -1)
{
	int i = start<0?(int)uri.size()-1:start;
	for (;i>0 && uri[i]!='/';i--);
	return i;
}
#ifdef __GNUG__
__attribute__((const))
#endif

/**
 * @brief Функция поиска в @p uri первого символа '/', начиная с позиции @p start.
 * @note По умолчанию поиска начинается с начала @p uri.
 * @return Если '/' был найден, то его индекс в @p uri, иначе число большее длины @p uri.
 */
inline
int find_first_slash(const sc_string &uri,int start = 0)
{
	assert(start>=0);
	int i = start<0?0:start;
	int len = (int)uri.size();
	while (i<len && uri[i]!='/')
		i++;
	return i;
}
#ifdef __GNUG__
__attribute__((const))
#endif

/**
 * @brief Функция поиска в @p uri базового имени. Например, для "/tmp/myseg" базой будет "myseg".
 * @return базовое имя.
 */
inline
sc_string basename(const sc_string &uri)
{
	int len = (int)uri.size();
	while (uri[len-1]=='/')
		len--;
	int i = find_last_slash(uri,len-1)+1;
	return uri.substr(i,len-i);
}

/**
 * @brief Функция поиска в @p uri пути к родительской директории. Например, для "/tmp/mydir/myseg" таким путем будет "/tmp/mydir".
 * @return пути к родительской директории.
 */
inline
sc_string dirname(const sc_string &uri)
{
	int len = (int)uri.size();
	while (uri[len-1]=='/')
		len--;
	int i = find_last_slash(uri,len-1);
	while (i>0 && uri[i] == '/') i--;
	return uri.substr(0,i+1);
}

#endif // __SC_STRING_UTILS_H__
