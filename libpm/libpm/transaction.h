
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



#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <libsc/sc_transaction.h>
#include <list>
#include <memory>

class pm_transaction : public sc_transaction
{
public:
	pm_transaction(sc_session *_host) : sc_transaction(_host) { }

	virtual ~pm_transaction() { rollback(); }

	void rollback()
	{
		if (need_rollback) {
			trans_pair_list::reverse_iterator piter = orig_beg.rbegin();
			while (piter != orig_beg.rend())	{
				host->set_beg(piter->first,piter->second);
				piter++;
			}

			piter = orig_end.rbegin();
			while (piter != orig_end.rend()) {
				host->set_end(piter->first,piter->second);
				piter++;
			}

			trans_list::reverse_iterator iter = gened_elems.rbegin();
			while (iter != gened_elems.rend())
				host->erase_el(*iter++);
			
			segment_list::iterator created_seg_iter = created_segments.begin();
			while (created_seg_iter != created_segments.end()) {
				host->unlink((*created_seg_iter)->get_full_uri());
				created_seg_iter++;
			}

			need_rollback = false;
		}
	}

	void commit()
	{
		if (need_rollback) {
			trans_list::reverse_iterator iter = erased_elems.rbegin();
			while (iter != erased_elems.rend()) {
				if (!(*iter)->is_dead())
					host->erase_el(*iter);
				(*iter++)->unref();
			}

			need_rollback = false;
		}
	}
};

/**
 * Создает указатель транзакции и оборачивает переданную сессию. Транзакция оборачивается в auto_ptr.
 * @param trans имя указателя на транзакцию.
 * @param session указатель на сессию, которая будет оборачиватся в транзакцию.
 */
#define START_TRANSACTION(trans, session) \
	pm_transaction *trans = new pm_transaction(session); \
	const std::auto_ptr<pm_transaction> __auto##trans(s);

#endif // __TRANSACTION_H__
