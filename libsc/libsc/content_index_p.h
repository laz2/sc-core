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
 * @file content_index_p.h
 * @brief Functional for indexing content.
 * @ingroup libsc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBSC_CONTENT_INDEX_H_INCLUDED__
#define __LIBSC_CONTENT_INDEX_H_INCLUDED__

/// @addtogroup libsc
/// @{

#include <vector>

#include "sc_content.h"
#include "sc_utils.h"

extern "C" {
	#include <md5/md5.h>
};

/// Class for indexing content. Calculate MD5 checksum.
/// Support only contents of type #TCSTRING and #TCDATA.
class content_index
{
public:
	content_index()
	{
	}

	~content_index();

	/// Update index for content of passed #sc_addr_ll. Return signature for new content.
	void* update(sc_addr_ll ll, void *cur_signature, const Content &new_content);

	/// Clear index for passed #sc_addr_ll.
	void clear(sc_addr_ll ll, void *cur_signature);

	/// Find elements with passed @p content.
	const ll_list& find(const Content &content);

private:
	static
	bool is_indexed_type(TCont t) { return t != TCEMPTY; }

	static
	void calc_signature(const Content &content, void *signature);

private:
	class signature_less : std::binary_function<char *, char *, bool>
	{
	public:
		result_type operator() (first_argument_type a, second_argument_type b) const
		{
			return ::memcmp(a, b, MD5_SIZE) < 0;
		}
	};

	typedef std::map<char *, ll_list, signature_less> checksum_map;
	checksum_map index;

	static const ll_list empty_ll_list;
};

/// @}

#endif // __LIBSC_CONTENT_INDEX_H_INCLUDED__
