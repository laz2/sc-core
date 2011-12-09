
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
 * @file sc_operation.h
 * @author Lazurkin Dmitriy
 * @brief Интерфейс реализации подсистемы sc-операций.
 */

#ifndef __SC_OPERATION_H__
#define __SC_OPERATION_H__

#include "pm.h"
#include <libsc/advanced_activity.h>
#include <libsc/sc_generator.h>
#include <libsc/sc_event_multi.h>

/**
 * @brief Инициализация подсистемы sc-операций.
 */
LIBPM_API sc_retval init_sc_operations(sc_session *s);

/**
 * @brief URI сегмента, в котором хранится информация о sc-операциях.
 */
const sc_string OPERATIONS_SEGURI = "/proc/operations";

/**
 * @brief Родительский класс для sc-операций.
 */
class LIBPM_API ScOperation : public sc_event_multi
{
protected:
	sc_string name;
	sc_addr operation;
	sc_addr actCond;
	sc_addr actCondParams;
	sc_segment *tmpSegment;

public:
	/**
	 * @param _name имя sc-операции. 
	 */
	ScOperation(sc_string _name) : name(_name), actCond(0), actCondParams(0) {} 

	sc_string getName() { return name; }

	/**
	 * @brief Возвращает условие активации.
	 */
	sc_addr getActCond() { return actCond; }

	/**
	 * @brief Возвращает параметры условия активации.
	 */
	sc_addr getActCondParams() { return actCondParams; }

	/**
	 * @brief Производит регистрацию sc-операции.
	 */
	sc_addr registerOperation();

	/**
	 * @brief Шаблонный метод для обработки активации sc-оперпации.
	 */
	virtual bool activate(sc_wait_type type, sc_param *prm, int len);

protected:
	/**
	 * @brief Создает связь между знаком параметры и знаком значения параметра условия активации.
	 * @param param знак параметра.
	 * @param value знак значения параметра.
	 */
	void bindParam(sc_addr param, sc_addr value);

	/**
	 * @brief Создает временный сегмент для sc-операции.
	 */
	sc_segment *createTmpSegment();

	/**
	 * @brief Метод pеализации генерации условия активации и создания связей между параметрами и их значениями. 
	 * @param &gen генератор для создания условия активации, текущий элемент генератора - знак условия активации. 
	 */
	virtual void genActCond(sc_generator &gen) = 0;

	/**
	 * @brief Реализация логики sc-операции.
	 * @param paramSystem знак параметров, заключенных в контур.
	 * @param tmpSegment временный сегмент для работы.
	 * @return true если отработала успешно, иначе false.
	 */
	virtual bool activateImpl(sc_addr paramSystem, sc_segment *tmpSegment) = 0; 
};

/**
 * @brief Класс sc-операции с условием активации включения элемента в конкретное множество.
 */
class LIBPM_API ScOperationActSetMember : public ScOperation
{
private:
	sc_addr aset;

public:
	/**
	 * @param name имя sc-операции.
	 * @param _aset знак множества, при включении элемента в которое будет происходить активация.
	 */
	ScOperationActSetMember(sc_string name, sc_addr _aset) : ScOperation(name), aset(_aset) {} 

protected:
	void genActCond(sc_generator &gen)
	{
		sc_addr e1 = gen.element("", SC_N_VAR);
		sc_addr e3 = gen.element("", SC_N_VAR);
		gen.gen_arc(e1, e3, "", SC_A_VAR|SC_POS);

		bindParam(e1, aset);

		sc_param prm[1];
		prm[0].addr = aset;
		attach_to(system_session, SC_WAIT_HACK_SET_MEMBER, prm, 1);
	}

};

/**
 * @brief Класс sc-операции с условием активации включения конкретного элемента в любое множество.
 */
class LIBPM_API ScOperationActInSet : public ScOperation
{
private:
	sc_addr el;

public:
	/**
	 * @param name имя sc-операции.
	 * @param _el знак элемента, при включении которого в любое множество будет происходить активация.
	 */
	ScOperationActInSet(sc_string name, sc_addr _el) : ScOperation(name), el(_el) {} 

protected:
	void genActCond(sc_generator &gen)
	{
		sc_addr e1 = gen.element("", SC_N_VAR);
		sc_addr e3 = gen.element("", SC_N_VAR);
		gen.gen_arc(e1, e3, "", SC_A_VAR|SC_POS);

		bindParam(e3, el);

		sc_param prm[1];
		prm[0].addr = el;
		attach_to(system_session, SC_WAIT_HACK_IN_SET, prm, 1);
	}

};


class LIBPM_API ScOperationElChange : public ScOperation
{
protected:
	sc_addr el;
	sc_wait_type event_type;

public:

	ScOperationElChange(sc_string name, sc_addr _el, sc_wait_type _event_type) : ScOperation(name), el(_el), 
		event_type(_event_type) {} 

protected:
	void genActCond(sc_generator &gen)
	{
		sc_addr e1 = gen.element("", SC_N_VAR);
		
		bindParam(e1, el);

		sc_param prm[1];
		prm[0].addr = el;
		attach_to(system_session, event_type, prm, 1);
	}

};


class LIBPM_API ScOperationCont : public ScOperationElChange
{
public:
	ScOperationCont(sc_string name, sc_addr _el) : ScOperationElChange(name, _el, SC_WAIT_CONT) {} 
};


class LIBPM_API ScOperationIdtf : public ScOperationElChange
{
public:
	ScOperationIdtf(sc_string name, sc_addr _el) : ScOperationElChange(name, _el, SC_WAIT_IDTF) {} 
};


class LIBPM_API ScOperationDie : public ScOperationElChange
{
public:
	ScOperationDie(sc_string name, sc_addr _el) : ScOperationElChange(name, _el, SC_WAIT_DIE) {} 
};


class LIBPM_API ScOperationChangeType : public ScOperationElChange
{
public:
	ScOperationChangeType(sc_string name, sc_addr _el) : ScOperationElChange(name, _el, SC_WAIT_CHANGE_TYPE) {} 
};


#endif // __SC_OPERATION_H__
