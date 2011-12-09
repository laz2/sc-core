
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
 * @file sc_list.h
 * @brief Functions and classes for working with two-linked list in sc-memory.
 * @ingroup libsc
 * @author Alexey Kondratenko, Dmitry Lazurkin
 */

#ifndef __LIBSC_SC_LIST_H_INCLUDED__
#define __LIBSC_SC_LIST_H_INCLUDED__

/// @addtogroup libsc
/// @{

#include "libsc.h"

#include <iterator>

/// @brief Representation of two-linked list in sc-memory.
/// @note <code>/proc/keynode/list</code>
/// @note <code>/proc/keynode/list_next_</code>
/// @note <code>/proc/keynode/list_value_</code>
class LIBSC_API sc_list
{
public:
	/**
	 * @brief Create sc-list element with value @p val and set for it as next @p next.
	 *
	 * Example sc-list creation:
	 * @code
	 * {
	 *     sc_addr head = sc_list::create(s, seg, val, 0), tail = head;
	 *     while (..) {
	 *         sc_addr curr = sc_list::create(s, seg, val, 0);
	 *         sc_list::set_next(s, seg, tail, curr);
	 *         tail = curr;
	 *     }
	 * }
	 * @endcode
	 *
	 * Example iteration over list:
	 * @code
	 * {
	 *     sc_addr curr = head;
	 *     while (curr) {
	 *         sc_addr val = sc_list::get_value(s,curr);
	 *         //< do something on value here >
	 *         curr = sc_list::get_next(s, curr);
	 *     }
	 * }
	 * @endcode
	 */
	static sc_addr create(sc_session *s, sc_segment *seg, sc_addr val, sc_addr next=0);

	/// Erase @p list and all next elements.
	/// @note Not erase values of list elements.
	static void erase(sc_session *s, sc_addr list);

	/// Set for @p list1 next element as @p list2.
	static sc_retval set_next(sc_session *s, sc_segment *seg, sc_addr list1, sc_addr list2);

	/// As #sc_list.set_next, but generate in segment of @p l1.
	static inline sc_retval set_next(sc_session *s, sc_addr l1, sc_addr l2)
	{
		return set_next(s, l1->seg, l1, l2);
	}

	/// Set value @p val for list element @p list. If @p val is #SCADDR_NIL, then set for @p list empty value.
	static sc_retval set_value(sc_session *s, sc_segment *seg, sc_addr list, sc_addr val);

	/// As #sc_list.set_value, but generate in segment of @p l.
	static inline sc_retval set_value(sc_session *s, sc_addr l, sc_addr v)
	{
		return set_value(s, l->seg, l, v);
	}

	/// Return next element for @p list. Return 0 if @p list is last element.
	static sc_addr get_next(sc_session *s,sc_addr list);

	/// Return prev element for @p list. Return 0 if @p list is begin element.
	static sc_addr get_prev(sc_session *s, sc_addr list);

	/// Return value of list element @p list. Return 0 if @p list hasn't value.
	static sc_addr get_value(sc_session *s, sc_addr list);

	/// Template STL forward iterator for iterating over sc-list elements. 
	/// @details Use @p GetNext for retrieve next element for current element.
	/// @note Default constructor constructs "past-the-end" iterator.
	/// @see #sc_list.get_prev
	/// @see #sc_list.get_next
	template<sc_addr (*GetNext)(sc_session *, sc_addr)>
	class sc_list_iterator
	{
	public:
		/// Proxy class for create #sc_list_iterator assignable.
		class proxy_value
		{
		public:
			proxy_value(const sc_list_iterator &it) : it_(it) {}

			proxy_value& operator=(sc_addr v)
			{
				assert(it_.list_ && "Iterator is past-the-end");
				if (it_.seg) {
					sc_list::set_value(it_.s_, it_.seg, it_.list_, v);
				} else {
					sc_list::set_value(it_.s_, it_.list_->seg, it_.list_, v);
				}
				return *this;
			}

			operator sc_addr () const { return sc_list::get_value(it_.s_, it_.list_); }

		private:
			const sc_list_iterator &it_;
		};

		typedef std::forward_iterator_tag iterator_category;
		typedef sc_addr value_type;
		typedef ptrdiff_t difference_type;
		typedef ptrdiff_t distance_type;
		typedef sc_addr pointer;
		typedef proxy_value reference;

		/// Construct "past-the-end" iterator.
		sc_list_iterator() : list_(SCADDR_NIL) {}

		/// Use segment of @p list for generating.
		sc_list_iterator(sc_session *s, sc_addr list) : s_(s), seg(0), list_(list)
		{
			assert(s_);
			assert(list_);
		}

		/// Use segment @p seg for generating.
		sc_list_iterator(sc_session *s, sc_segment *seg, sc_addr list) : s_(s), seg(seg), list_(list)
		{
			assert(s_);
			assert(seg);
			assert(list_);
		}

		bool operator==(const sc_list_iterator &o) const
		{
			return list_ == o.list_;
		}

		bool operator!=(const sc_list_iterator &o) const 
		{
			return list_ != o.list_;
		}

		reference operator*() const
		{
			assert(list_ && "Iterator is past-the-end");
			return proxy_value(*this);
		}

		pointer operator->() const
		{
			assert(list_ && "Iterator is past-the-end");
			return sc_list::get_value(s_, list_);
		}

		sc_list_iterator& operator++()
		{
			assert(list_ && "Iterator is past-the-end");
			list_ = GetNext(s_, list_);
			return *this;
		}

		sc_list_iterator operator++(int)
		{
			assert(list_ && "Iterator is past-the-end");
			iterator tmp_ = *this;
			list_ = GetNext(s_, list_);
			return tmp_;
		}
	private:
		sc_session *s_;
		sc_segment *seg;
		sc_addr list_; ///< Current list element, if SCADDR_NIL then iterator is "past-the-end".
	};

	/// Forward iterator from passed #sc_list element.
	typedef sc_list_iterator<&get_next> iterator;
	/// Reverse forward iterator from passed #sc_list element.
	typedef sc_list_iterator<&get_prev> reverse_iterator;
};

/// @}

#endif // __LIBSC_SC_LIST_H_INCLUDED__
