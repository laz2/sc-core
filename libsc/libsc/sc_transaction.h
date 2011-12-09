
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
 * @file sc_transaction.h
 * @brief Определение мини-транзакции над sc-памятью.
 * @ingroup libsc
 */

#ifndef __LIBSC_SC_TRANSACTION_H_INCLUDED__
#define __LIBSC_SC_TRANSACTION_H_INCLUDED__

#include "libsc.h"
#include "sc_session_proxy.h"

/**
 * @brief Класс мини-транзакции над sc-памятью.
 * Это ненастоящая транзакция. Из ACID данный класс реализует только С, т.е. согласованность.
 * Мини-транзация обрабатывает только генерацию и удаление элементов.
 * @warning Мини-транзакция не поддерживает ссылки.
 */
class LIBSC_API sc_transaction : public sc_session_proxy
{
public:
	typedef std::pair<sc_addr,sc_addr> trans_pair;
	typedef std::vector<sc_addr> trans_list;
	typedef std::vector<trans_pair> trans_pair_list;
	typedef std::list<sc_segment*> segment_list;

protected:
	trans_list erased_elems;
	trans_pair_list orig_beg;
	trans_pair_list orig_end;
	trans_list gened_elems;
	segment_list created_segments;
	bool need_rollback;

public:
	
	/**
	 * @param _host сессия, которую необходимо сделать транзакционной.
	 */
	sc_transaction(sc_session *_host) : sc_session_proxy(_host), need_rollback(true)
	{
	}

	~sc_transaction() {}
	
	void commit();

	void rollback();

	void close() {commit();}

	sc_retval set_beg(sc_addr arc,sc_addr beg);

	sc_retval set_end(sc_addr arc, sc_addr end);

	sc_iterator *create_iterator(sc_constraint *constr, bool sink);

	sc_addr create_el(sc_segment *seg, sc_type type);

	sc_retval erase_el(sc_addr addr);
	
	sc_segment *create_segment(const sc_string &uri);

	sc_session *__fork() { SC_ABORT(); }

	bool erase_el_pre(sc_addr a) { SC_ABORT(); }

	sc_retval __erase_el(sc_addr a) { SC_ABORT(); }
};

#endif // __LIBSC_SC_TRANSACTION_H_INCLUDED__
