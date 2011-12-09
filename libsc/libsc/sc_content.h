
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
 * @file sc_content.h
 * @brief Описание классов представления содержимого sc-элементов.
 * @ingroup libsc
 * @author IVP
 */

#ifndef __CONTENT_H__
#define __CONTENT_H__

#include "config.h"

#include "sc_types.h"

#include <ostream>
#include <string.h>
#include <stdlib.h>

typedef long int cint; /**< Тип для представления целочисленного содержимого. */
typedef double creal; /**< Тип для представления вещественного содержимого. */

/**
 * @brief Структура для представления строкового и бинарного содержимого.
 */
struct cdata 
{
	/**
	 * @brief размер хранимых данных.
	 * @note Если используеться для строкового содержимого, 
	 * тогда должно включать реальный размер строки, т.е. с завершающим 0. 
	 */
	unsigned int size;
	char *ptr; /**< @brief указатель на хранимые данные. */
};

class Content;

/**
 * @brief Объединение, которое представляет хранимое содержимое.
 */
union Cont 
{
#ifdef __ICL
	Cont(const Content &c);
	
	Cont() {};
	
	Cont(const Cont &c) 
	{
		 *this = c;
	}
#endif
	/** @brief Целочисленное содержимое. */
	cint i;
	/** @brief Вещественное содержимое. */
	creal r;
	/** @brief Бинарное, строковое или "ленивое" содержимое. */
	cdata d; 

	/**
	 * @brief Метод позволяет позволяет создать объект #Cont на основе целочисленных данных.
	 */
	static Cont integer(cint i) 
	{
		Cont c;
		c.i = i;
		return c;
	}
	
	/**
	 * @brief Метод позволяет позволяет создать объект #Cont на основе вещественных данных.
	 */
	static Cont real(creal r) 
	{
		Cont c;
		c.r = r;
		return c;
	}

	/**
	 * @brief Метод позволяет позволяет создать объект #Cont на основе бинарных данных.
	 */
	static Cont data(unsigned size, void *ptr) 
	{
		Cont c;
		c.d.size = size;
		c.d.ptr = (char*)ptr;
		return c;
	}
	
	/**
	 * @brief Метод позволяет позволяет создать объект #Cont на основе строковых данных.
	 */
	static Cont string(const char *str) 
	{
		Cont c;
		c.d.size = (unsigned int)strlen(str) + 1;
		c.d.ptr = (char*)str;
		return c;
	}
	
	/**
	 * @brief Метод позволяет позволяет создать объект #Cont на основе незагруженных данных.
	 * @param full_uri URI данных на файловой системе, в сети или в любом другом месте.
	 */
	static Cont lazy_data(const char *full_uri) 
	{
		Cont c;
		c.d.size = (unsigned int)strlen(full_uri) + 1;
		c.d.ptr = (char*)full_uri;
		return c;
	}
};

/**
 * @brief Идентифицирует тип содержимого sc-элемента.
 */
enum TCont 
{
	TCSTRING = 1, /**< строковое содержимое. */
	TCINT, /**< целочисленное содержимое. */
	TCREAL, /**< вещественное содержимое. */
	TCDATA, /**< бинарное содержимое. */
	TCLAZY, /**< содержимое, которое еще не загружено. */
	TCEMPTY = 0 /**< не имеет содержимого. */
};

/**
 * @brief Класс для работы с содержимым sc-элементов.
 */
class LIBSC_API Content 
{
	Cont cont;
	TCont c_type;

public:
#ifdef __ICL
	friend union Cont;
#else 
	
	/**
	 * @brief Оператор явного приведения типа к структуре Cont.
	 */
	operator const Cont &()const 
	{
		return cont;
	}

#endif 
	
	/**
	 * @brief Конструктор по умолчанию.
	 */
	Content() 
	{
		set_type(TCEMPTY);
	}
	
	/**
	 * @brief Конструктор копирования.
	 */
	Content(const Content &c) 
	{
		c_type = TCEMPTY;
		copy_cont(c.cont, c.c_type);
	}
	
	/**
	 * @param c содержимое.
	 * @param t тип содержимого.
	 */
	Content(const Cont &c, TCont t) 
	{
		c_type = TCEMPTY;
		copy_cont(c, t);
	}
	
	/**
	 * @brief Конструктор создающий объект на основании строки.
	 * @note Строка будет скопирована.
	 */
	Content(const sc_string &str) 
	{
		c_type = TCSTRING;
		cont.d.size = (unsigned int)str.size() + 1;
		cont.d.ptr = new char[cont.d.size];
		memcpy(cont.d.ptr, str.data(), cont.d.size - 1);
		cont.d.ptr[cont.d.size - 1] = 0;
	}
	
	~Content() 
	{
		eraseCont();
	}
	
	/**
	 * @brief Удаляет реальное содержимое и устанавливает тип содержимого в TCont#TCEMPTY.
	 */
	void eraseCont() 
	{
		if (c_type == TCDATA || c_type == TCSTRING) {
			delete [] cont.d.ptr;
		} 
		c_type = TCEMPTY;
	}

	/**
	 * @brief Оператор явного приведения типа к sc_string для содержимого типа TCont#TCSTRING.
	 */
	operator sc_string()const 
	{
		if (c_type != TCSTRING) {
			return sc_string();
		} 
		
		return sc_string(cont.d.ptr);
	}
	
	/**
	 * @brief Оператор явного приведения типа к char* для содержимого типа TCont#TCSTRING или TCont#TCDATA.
	 */
	operator char *()const 
	{
		return (c_type != TCDATA && c_type != TCSTRING) ? 0: cont.d.ptr;
	}

	Content &operator = (const Content &c) 
	{
		copy_cont(c.cont, c.c_type);
		return  *this;
	}
	
	bool operator == (const Content &c) const;

	bool operator > (const Content &c) const;

	bool operator < (const Content &c) const;

	bool operator != (const Content &c) const
	{
		return !(*this == c);
	}

	/**
	 * @return Тип хранимых данных.
	 */
	TCont type()const 
	{
		return c_type;
	}
	
	/**
	 * @brief Установить тип хранимых данных.
	 */
	void set_type(TCont t) 
	{
		c_type = t;
	}

	/**
	 * @brief Метод позволяет позволяет создать объект #Content на основе целочисленных данных.
	 */
	static Content integer(cint i) 
	{
		return Content(Cont::integer(i), TCINT);
	}

	/**
	 * @brief Метод позволяет позволяет создать объект #Content на основе вещественных данных.
	 */
	static Content real(creal r) 
	{
		return Content(Cont::real(r), TCREAL);
	}
	
	/**
	 * @brief Метод позволяет позволяет создать объект #Content на основе строковых данных.
	 */
	static Content string(const char *ptr) 
	{
		return Content(Cont::string(ptr), TCSTRING);
	}
	
	/**
	 * @brief Метод позволяет позволяет создать объект #Content на основе бинарных данных.
	 */
	static Content data(unsigned size, void *data) 
	{
		return Content(Cont::data(size, data), TCDATA);
	}

private:
	int copy_cont(const Cont &c, TCont t);
};

LIBSC_API std::ostream& operator<<(std::ostream &out, const Content &content);

#ifdef __ICL
inline Cont::Cont(const Content &c) 
{
	*this = c.cont;
}
#endif 

#endif // __CONTENT_H__
