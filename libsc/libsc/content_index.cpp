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

#include "precompiled_p.h"

#include "content_index_p.h"

const ll_list content_index::empty_ll_list;

content_index::~content_index()
{
	checksum_map::iterator it = index.begin();
	for (; it != index.end(); ++it)
		delete[] it->first;
}

void* content_index::update(sc_addr_ll ll, void *cur_signature, const Content &new_content)
{
	clear(ll, cur_signature);

	if (is_indexed_type(new_content.type())) {
		// Calculate new signature
		char new_signature[MD5_SIZE];

		calc_signature(new_content, new_signature);

		// Create index for new content
		checksum_map::iterator it = index.find(new_signature);
		if (it != index.end()) {
			it->second.push_back(ll);
			return it->first;
		} else {
			// Alloc dynamic memory for new signature
			char *signature = new char[MD5_SIZE];
			memcpy(signature, new_signature, MD5_SIZE);

			index[signature].push_back(ll);

			return signature;
		}
	} else {
		return 0;
	}
}

void content_index::clear(sc_addr_ll ll, void *cur_signature)
{
	if (cur_signature) {
		checksum_map::iterator it = index.find(static_cast<char *>(cur_signature));

		SC_ASSERT(it != index.end() && "Content index doesn't contain passed signature.");

		it->second.remove(ll);

		if (it->second.empty()) {
			index.erase(it->first);
			delete[] cur_signature;
		}
	}
}

const ll_list& content_index::find(const Content &content)
{
	if (is_indexed_type(content.type())) {
		if (!index.empty()) {
			char signature[MD5_SIZE];
			calc_signature(content, signature);

			checksum_map::iterator it = index.find(signature);
			if (it != index.end())
				return it->second;
		}
	}

	return empty_ll_list;
}

void content_index::calc_signature(const Content &content, void *signature)
{
	const char *buf = 0;
	size_t buf_len = 0;

	const Cont c(content);
	switch (content.type()) {
	case TCSTRING:
	case TCDATA:
		buf = c.d.ptr;
		buf_len = c.d.size;
		break;
	case TCINT:
		buf = reinterpret_cast<const char *>(&c.i);
		buf_len = sizeof(cint);
		break;
	case TCREAL:
		buf = reinterpret_cast<const char *>(&c.r);
		buf_len = sizeof(creal);
		break;
	default:
		SC_ABORT_MESSAGE("Unknown content type.");
	}

	md5_buffer(buf, buf_len, signature);
}
