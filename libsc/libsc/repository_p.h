
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



#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__

#include "libsc.h"
#include "sc_types.h"

/// Repository is a tree of directories
class sc_repo
{
public:
	sc_string current_tmp_prefix;
	sc_repo();
	virtual sc_retval _stat(const sc_string &dir) = 0;
	virtual sc_dir_iterator* search_dir(const sc_string &dir) = 0;

	virtual sc_retval create_segment(const sc_string &uri) = 0;
	virtual sc_retval unlink(const sc_string &uri) = 0;
	virtual sc_retval rename(const sc_string &old,const sc_string &) = 0;
	virtual sc_retval mkdir(const sc_string &uri) = 0;

	virtual sc_segment* map_segment(const sc_string &uri) = 0;
	virtual sc_retval unmap_segment(sc_segment *seg) = 0;

	virtual sc_string get_uri() = 0;
	virtual void set_uri(const sc_string &) = 0;

	virtual void flush() {}

	virtual	~sc_repo();
};

class sc_segment_base;

class sc_custom_repo : public sc_repo
{
public:
	virtual sc_retval create_segment(const sc_string &uri, sc_segment_base *custom_seg) =0;
};

extern sc_custom_repo* create_tmp_repo(const sc_string &loc);
extern sc_repo *create_fs_repo(const sc_string &loc, const sc_string &fs_place);

#endif //__REPOSITORY_H__
