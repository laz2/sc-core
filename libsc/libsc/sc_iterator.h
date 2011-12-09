/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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
 * @file sc_iterator.h
 * @brief Interface of sc-iterator and
 *        utils for working with objects of this class.
 * @ingroup libsc
 */

#ifndef __LIBSC_SC_ITERATOR_INCLUDED__
#define __LIBSC_SC_ITERATOR_INCLUDED__

/// @addtogroup libsc
/// @{

#include "sc_types.h"

/**
 * @brief ������� ����� ��������� �� ��������� ����� sc-�����������.
 */
class LIBSC_API sc_iterator
{
public:
	#ifdef COUNT_ELEMENTS
		static int iter_cnt;

		sc_iterator()
		{
			iter_cnt++;
		}
	#endif

	/// Switch iterator to next founded result.
	virtual sc_retval next() = 0;

	/**
	 * @return true, ���� ��� sc-�����������, � ������� ����� �������.
	 * @return false - � ��������� ������.
	 */
	virtual bool is_over() = 0;

	/**
	 * @return ����� sc-�������� � ���������� ������� @p num � ��������� sc-�����������.
	 */
	virtual sc_addr value(sc_uint num) = 0;

	virtual ~sc_iterator()
	{
		#ifdef COUNT_ELEMENTS
			iter_cnt--;
		#endif
	}
};

/// Typedef std::auto_ptr + sc_iterator
typedef std::auto_ptr<sc_iterator> auto_sc_iterator_ptr;

/// Define local #auto_sc_iterator_ptr for deleting #sc_iterator @p iter
#define SC_ITERATOR_PTR_DELETER(iter) auto_sc_iterator_ptr auto_ ## iter (iter)

/// Iterate with @p iter and delete it at the end.
#define sc_for_each(iter) for (SC_ITERATOR_PTR_DELETER(iter); !iter->is_over(); iter->next())


class over_iterator : public sc_iterator
{
public:
	~over_iterator() {}

	sc_retval next() { return RV_ELSE_GEN; }

	sc_addr value(sc_uint num) { return SCADDR_NIL; }

	bool is_over() { return true; }
};

/// @}

#endif // __LIBSC_SC_ITERATOR_INCLUDED__
