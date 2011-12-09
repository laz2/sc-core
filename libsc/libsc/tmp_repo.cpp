
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

#include "precompiled_p.h"

#include "tmp_segment_p.h"

// field element::type for links

class tmp_dir;
class tmp_segment;
class tmp_iterator;
class tmp_dir_iterator;

struct tmp_dirent {
	sc_string name;
	bool dir;
	union {
		tmp_dir *dir;
		sc_segment_base *seg;
	} u;
};

typedef safe_list<tmp_dirent> dirent_list;

class tmp_dir
{
	dirent_list contents;
	int refcnt;
public:
	friend class tmp_dir_iterator;

	tmp_dir() : refcnt(1) {}
	
	tmp_dir *ref()
	{
		refcnt++;
		return this;
	}
	
	void unref()
	{
		if (!--refcnt)
			delete this;
	}
	void die() {unref();}
	
	sc_dir_iterator *search();
	
	tmp_dirent *find(const sc_string &str);
	
	void add(const tmp_dirent &dir);
	
	bool remove(const sc_string &str);
	
	bool remove(const tmp_dirent *dirent);
};

class tmp_dir_iterator: public sc_dir_iterator
{
	dirent_list::iterator iter;
	dirent_list *list;
	tmp_dir *dir;
public:
	tmp_dir_iterator (dirent_list *_list,tmp_dir *d)
		: iter(_list), list(_list), dir(d->ref()) {}
	~tmp_dir_iterator()
	{
		dir->unref();
	}
	const sc_string value()
	{
		if (iter == list->end())
			SC_ABORT();
		return (*iter).name;
	}
	bool is_over()
	{
		return iter == list->end();
	}
	bool next()
	{
		return (++iter) == list->end();
	}
};

sc_dir_iterator * tmp_dir::search()
{
	return new tmp_dir_iterator(&contents,this);
}

tmp_dirent * tmp_dir::find(const sc_string &str)
{
	dirent_list::iterator iter = contents.begin();
	for (;iter != contents.end();++iter) {
		if (iter->name == str)
			return &(*iter);
	}
	return 0;
}

void tmp_dir::add(const tmp_dirent &dir)
{
	contents.insert(contents.end(),dir);
}

bool tmp_dir::remove(const sc_string &str)
{
	dirent_list::iterator iter = contents.begin();
	for (;iter != contents.end();++iter)
		if (iter->name == str) {
			iter.remove();
			return true;
		}
	return false;
}

bool tmp_dir::remove(const tmp_dirent *dirent)
{
	dirent_list::iterator iter = contents.begin();
	for (;iter != contents.end();++iter)
		if (&(*iter) == dirent) {
			iter.remove();
			return true;
		}
	return false;
}

class tmp_repo : public sc_custom_repo
{
	tmp_dir root;
	sc_string repo_loc;

	tmp_dirent *find_dirent(const sc_string &name)
	{
		int i,size = name.size();
		int start = 0;
		tmp_dir *cdir = &root;
		tmp_dirent *dirent;

		assert(name[0] != '/');
		for (;;) {
			for (i = start;i<size;i++)
				if (name[i] == '/')
					break;
			dirent = cdir->find(name.substr(start,i-start));
			if (!dirent)
				return 0;
			if (!dirent->dir) {
				if (i<size)
					return 0;
				return dirent;
			}
			if (i==size)
				return dirent;
			cdir = dirent->u.dir;
			while (name[++i] == '/');
			start = i;
		}
	}
	// finds direcory where item s reside and returns place where its name begins
	// dir means allowed to have trailing /
	tmp_dir* find_directory(const sc_string &s,int &i,bool _dir)
	{
		i = (s.size())-1;
		if (i<0)
			return 0;
		assert(i>0 || s[0] != '/');

		if (_dir) { // get rid of trailing /
			while (s[i] == '/')
				i--;
		} else if (s[i] == '/')
			return 0;

		while (i>0)
			if (s[--i] == '/')
				break;
		tmp_dir *dir = &root;
		if (i>0) {
			int k = i;
			while (s[--k] == '/');
			tmp_dirent *dent = find_dirent(s.substr(0,k+1));
			if (!dent || !dent->dir)
				return 0;
			dir = dent->u.dir;
			i++;
		}
		return dir;
	}

	bool create_dirent(const sc_string &s, bool _dir, sc_segment_base *custom_seg=0)
	{
		tmp_dirent dirent;
		tmp_dir *dir;
		int i;

		if (!(dir = find_directory(s,i,_dir)))
			return false;

		if (_dir) {
			tmp_dir *newdir = new tmp_dir;
			dirent.dir = true;
			dirent.u.dir = newdir;
		} else {
			dirent.dir = false;

			if (custom_seg) {
				custom_seg->set_uri(s);
				custom_seg->set_repo(this);
				dirent.u.seg = custom_seg;
			} else {
				dirent.u.seg = new tmp_segment(s, this);
			}
		}

		dirent.name = s.substr(i);
		dir->add(dirent);

		return true;
	}

	void purge_dir(tmp_dir *dir)
	{
		sc_dir_iterator *it = dir->search();
		for (;!it->is_over();it->next()) {
			tmp_dirent *dirent = dir->find(it->value());
			if (dirent->dir) {
				purge_dir(dirent->u.dir);
			}
			unlink(dirent->name);
		}
		delete it;
	}

public:
	tmp_repo(const sc_string &loc) : repo_loc(loc)
	{
	}
	
	// we dont care here about history of alloc'ed memory, yet.
	~tmp_repo()
	{
		purge_dir(&root);
	}

	sc_retval _stat(const sc_string &s)
	{
		if (!s.size())
			return RV_ELSE_GEN;
		tmp_dirent *dirent = find_dirent(s);
		if (!dirent)
			return RV_ERR_GEN;
		return dirent->dir?RV_ELSE_GEN:RV_OK;
	}

	sc_dir_iterator *search_dir(const sc_string &s)
	{
		if (!s.size()) {
			return root.search();
		}
		tmp_dirent *dirent = find_dirent(s);
		if (!dirent || !dirent->dir)
			return 0;
		return dirent->u.dir->search();
	}

	sc_retval create_segment(const sc_string &s)
	{
		sc_segment *seg = map_segment(s);
		if (seg)
			return RV_OK;
		return create_dirent(s,false)?RV_OK:RV_ERR_GEN;
	}

	sc_retval create_segment(const sc_string &s, sc_segment_base *custom_seg)
	{
		sc_segment *seg = map_segment(s);
		if (seg)
			return RV_OK;
		return create_dirent(s, false, custom_seg) ? RV_OK : RV_ERR_GEN;
	}

	sc_retval unlink(const sc_string &s)
	{
		tmp_dir *dir;
		int pos;
		if (!(dir = find_directory(s,pos,false)))
			return RV_ERR_GEN;

		sc_string str = s.substr(pos);
		tmp_dirent *dirent = dir->find(str);
		if (!dirent)
			return RV_ERR_GEN;
		if (!dirent->dir) {
			dirent->u.seg->die(); // when last session will
						//close this segment it will die
			dir->remove(str);
			return RV_OK;
		}
		sc_dir_iterator *iter = dirent->u.dir->search();
		if (!iter->is_over()) {
			delete iter;
			return RV_ERR_GEN;
		}
		delete iter;
		dir->remove(dirent);
		dirent->u.dir->die();
		return RV_OK;
	}

	sc_retval rename(const sc_string &oldname, const sc_string &newname)
	{
		tmp_dir *olddir,*newdir;
		tmp_dirent *dirent;
		int pos,pos2;

		if (!(olddir = find_directory(oldname,pos,false)))
			return RV_ERR_GEN;
		dirent = olddir->find(oldname.substr(pos));
		if (!(newdir = find_directory(newname,pos2,false)))
			return RV_ERR_GEN;
		if (!dirent->dir)
			dirent->u.seg->set_uri(newname);
		olddir->remove(dirent);
		dirent->name = oldname.substr(pos);
		newdir->add(*dirent);
		delete dirent;
		return RV_OK;
	}

	sc_retval mkdir(const sc_string &s)
	{
		return create_dirent(s,true)?RV_OK:RV_ERR_GEN;
	}

	sc_segment* map_segment(const sc_string &s)
	{
		tmp_dirent *dirent = find_dirent(s);
		if (!dirent || dirent->dir)
			return 0;
		return dirent->u.seg;
	}

	sc_retval unmap_segment(sc_segment *seg)
	{
		return RV_OK;
	}

	sc_string get_uri()
	{
		return repo_loc;
	}
	void set_uri(const sc_string& str)
	{
		repo_loc = str;
	}

}; // end of tmp_repo definition

sc_custom_repo* create_tmp_repo(const sc_string &loc)
{
	return new tmp_repo(loc);
}
