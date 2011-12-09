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

///
/// @file stl_utils.h
/// @brief Utils for working with STL-containers, .
/// @ingroup libsc
///

#ifndef __LIBSC_STL_UTILS_H_INCLUDED__
#define __LIBSC_STL_UTILS_H_INCLUDED__

/// @addtogroup libsc
/// @{
///

/// From map container @p m for @p key return pointer value.
template<typename K, typename V>
inline V* get_pvalue(const std::map<K, V*> &m, const K &key)
{
	typename std::map<K, V*>::const_iterator it = m.find(key);
	if (it != m.end()) {
		return it->second;
	} else {
		return 0;
	}
}

/// Test if container @p c contains element for which predicate @p pred is successful.
template<typename C, class P>
inline bool contains_if(const C &c, P pred)
{
	typename C::const_iterator it = std::find_if(c.begin(), c.end(), pred);
	return it != c.end();
}

/// Test if container @p c contains element @p v.
template<typename C, typename V>
inline bool contains(const C &c, const V &v)
{
	typename C::const_iterator it = std::find(c.begin(), c.end(), v);
	return it != c.end();
}

/// Test if set container @p s contains element @p v.
template<typename V>
inline bool contains(const std::set<V> &s, const V &v)
{
	typename std::set<V>::const_iterator it = s.find(v);
	return it != s.end();
}

/// Test if map container @p m contains key @p k.
template<typename K, typename V>
inline bool contains(const std::map<K, V> &m, const K &k)
{
	typename std::map<K, V>::const_iterator it = m.find(k);
	return it != m.end();
}

namespace std
{
	template <>
	class less<const char*> : public std::binary_function<const char *, const char *, bool>
	{
	public:
		result_type operator() (first_argument_type a, second_argument_type b) const
		{
			return ::strcmp(a, b) < 0;
		}
	};

	template <>
	class less<char*> : public std::binary_function<char *, char *, bool>
	{
	public:
		result_type operator() (first_argument_type a, second_argument_type b) const
		{
			return ::strcmp(a, b) < 0;
		}
	};
} // namespace std

///
/// @}

#endif // __LIBSC_STL_UTILS_H_INCLUDED__
