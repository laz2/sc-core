
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libtgf/tgfin.h>
#include <libtgf/tgfout.h>

#include "tmp_segment_p.h"

#ifndef WIN32
#include <unistd.h>
#include <dirent.h>

#define O_BINARY 0
#else
#include <direct.h>
#include <io.h>

#define S_ISDIR(x) (_S_IFDIR & (x))
#define S_ISREG(x) (_S_IFREG & (x))

static inline int mkdir(const char* _d, int mode) {return _mkdir(_d);}

#endif



typedef std::map<sc_string, tmp_segment *> mapped_t;

// fs_place must have trailing '/'
class unix_fs_repo : public sc_repo
{
sc_string base_uri;
sc_string fs_place;
mapped_t mapped;
public:
unix_fs_repo(const sc_string &_base_uri,const sc_string &_fs_place);
sc_retval _stat(const sc_string &dir);
sc_dir_iterator* search_dir(const sc_string &dir);

sc_retval create_segment(const sc_string &uri);
sc_retval unlink(const sc_string &uri);
sc_retval rename(const sc_string &old,const sc_string &);
sc_retval mkdir(const sc_string &uri);

sc_string get_uri() {return base_uri;}
void set_uri(const sc_string &s) {base_uri = s;}

void flush();

sc_segment* map_segment(const sc_string &uri);
sc_retval unmap_segment(sc_segment *seg);

~unix_fs_repo();
};


unix_fs_repo::unix_fs_repo(const sc_string &_base_uri,const sc_string &_fs_place)
	: base_uri(_base_uri), fs_place(_fs_place)
{
	struct stat buf;
	int rv;

#ifdef WIN32
	int __l = fs_place.length() - 1;
	if (fs_place[__l] == '\\')	{
		rv = stat(fs_place.substr(0, __l).c_str(),&buf);
	} else rv = stat(fs_place.c_str(),&buf);
#else
	rv = stat(fs_place.c_str(),&buf);
#endif
	if (rv<0 || !S_ISDIR(buf.st_mode)) {
		fprintf(stderr,"fs_storage: fs_place cannot be stat'ed\n");
		exit(1972);
	}
}

sc_retval unix_fs_repo::_stat(const sc_string &dir)
{
	struct stat buf;
	int rv;

#ifdef WIN32
	sc_string _buffer = fs_place + dir;
	int __l = _buffer.length() - 1;
	if (_buffer[__l] == '\\')	{
		rv = stat(_buffer.substr(0, __l).c_str(),&buf);
	} else rv = stat(_buffer.c_str(),&buf);
#else
	rv = stat((fs_place + dir).c_str(),&buf);
#endif

	if (rv < 0)  return RV_ERR_GEN;

	if (S_ISDIR(buf.st_mode))
		return RV_ELSE_GEN;
	if (S_ISREG(buf.st_mode))
		return RV_OK;
	return RV_ERR_GEN;
}

struct unixfs_openfail_exc {};

// this iterator safety is under the question.
// Of course it can not cause core dump, but what about other requirements?
#ifndef WIN32
class unix_fs_dir_iter : public sc_dir_iterator {
DIR *handle;
bool over;
sc_string nextname;
public:
unix_fs_dir_iter(const sc_string &where)
{
	handle = opendir(where.c_str());
	if (!handle)
		throw unixfs_openfail_exc();
	over = false;
	next();
}
const sc_string value()
{
	if (over)
		return sc_string();
	return nextname;
}
bool next()
{
	if (over)
		return false;
again:
	struct dirent *p = readdir(handle);
	if (!p) {
		over = true;
		return false;
	}
	if (!strcmp(p->d_name,".") || !strcmp(p->d_name,".."))
		goto again;
	nextname = p->d_name;
	return true;
}
bool is_over() {return over;}
~unix_fs_dir_iter()
{
	closedir(handle);
}
};
#else

class win32_fs_dir_iter : public sc_dir_iterator {
int handle;
bool over;
sc_string nextname;
_finddata_t fdata;
public:
win32_fs_dir_iter(const sc_string &_where)
{
	sc_string where = _where;
	where = where + "/*";
	handle = _findfirst(where.c_str(),&fdata);
	if (handle < 0)
		throw unixfs_openfail_exc();
	over = false;
	nextname = fdata.name;
	if (nextname == "." || nextname == "..")
		next();
}
const sc_string value()
{
	if (over)
		return sc_string();
	return nextname;
}
bool next()
{
	if (over)
		return false;
	if (_findnext(handle,&fdata)<0) {
		over = true;
		return false;
	}
	nextname = fdata.name;
	if (nextname == "." || nextname == "..")
		next();
	return true;
}
bool is_over() {return over;}
~win32_fs_dir_iter()
{
	_findclose(handle);
}
};

#endif

sc_dir_iterator* unix_fs_repo::search_dir(const sc_string &dir)
{
	sc_dir_iterator *iter;
	try {
#ifndef WIN32
		iter = new unix_fs_dir_iter(fs_place+dir);
#else
		iter = new win32_fs_dir_iter(fs_place+dir);
#endif
	}
	catch (unixfs_openfail_exc /*e*/) {
		return 0;
	}
	return iter;
}

sc_retval unix_fs_repo::create_segment(const sc_string &uri)
{
	if (_stat(uri) == RV_OK)
		return RV_OK;
	int fd = open((fs_place+uri).c_str(),O_CREAT | O_EXCL | O_WRONLY,0644);
	if (fd<0)
		return RV_ERR_GEN;
	close(fd);
	return RV_OK;
}
sc_retval unix_fs_repo::unlink(const sc_string &uri)
{
	dprintf("Unlink fs segment %s\n", uri.c_str());
	mapped_t::iterator iter = mapped.find(uri);
	bool flag = false;
	if (iter != mapped.end()) {
		iter->second->die();
		flag = true;
		mapped.erase(iter);
	}
	if (::remove((fs_place+uri).c_str())<0)
		if (flag) {
				SC_ABORT();
		} else
			return RV_ERR_GEN;
	return RV_OK;
}
sc_retval unix_fs_repo::rename(const sc_string &old,const sc_string &newname)
{
	// our rename has different semantic than fs standart has
	if (!RV_IS_ERR(_stat(newname)))
		return RV_ERR_GEN;
	mapped_t::iterator iter = mapped.find(old);
	bool flag = false;
	if (iter != mapped.end())
		iter->second->set_uri(newname), flag = true;
	if (rename((fs_place+old).c_str(),(fs_place+newname).c_str())<0)
		if (flag)
			SC_ABORT();
		else
			return RV_ERR_GEN;
	return RV_OK;
}
sc_retval unix_fs_repo::mkdir(const sc_string &uri)
{
	if (_stat(uri) == RV_ELSE_GEN)
		return RV_OK;
	if (::mkdir((fs_place + uri).c_str(),0755)<0)
		return RV_ERR_GEN;
	return RV_OK;
}

/// Fill from tgf passed segment.
/// @note Use segment uri as tgf client id of external segment.
class tgf_segment_loader
{
private:
	bool in_freeseg;
	sc_segment *seg;

	std::list<char *> seg_uris;
	char             *cur_seg_uri;

public:
	tgf_segment_loader(sc_segment *_seg)
		: seg(_seg), in_freeseg(true)
	{
	}

	~tgf_segment_loader()
	{
		std::for_each(seg_uris.begin(), seg_uris.end(),
			boost::checked_deleter<char>());
	}

	static
	int callback(tgf_command *cmd, void **cl_id, void *extra)
	{
		return static_cast<tgf_segment_loader *>(extra)->load_segment_cb(cmd, cl_id);
	}

private:
	int load_segment_cb(tgf_command *cmd, void **cl_id)
	{
		tgf_argument *args = cmd->arguments;
		*cl_id = 0;

		switch (cmd->type) {
		case TGF_GENEL:
			if (!in_freeseg) {
				sc_addr_ll ll = seg->create_link(sc_string(cur_seg_uri) + '/' + args[0].arg.a_string);
				if (!ll)
					return -1;

				*cl_id = ll;
			} else {
				if (tgf2sc_type_table[args[1].arg.a_sctype] < 0)
					SC_ABORT();
				sc_type type = tgf2sc_type_table[args[1].arg.a_sctype];

				sc_addr_ll ll = seg->gen_el(type);
				sc_string idtf = args[0].arg.a_string;

				if (seg->set_idtf(ll, idtf)) {
					dprintf("[SC::tgf_segment_loader] Warning: double '%s' definition. "
						"Using type and content from freeseg\n", args[0].arg.a_string);

					seg->erase_el(ll);

					*cl_id = seg->find_by_idtf(args[0].arg.a_string);
					SC_ASSERT(*cl_id && "Not found idtf in freeseg.");
				} else {
					if (cmd->arg_cnt > 2 && seg->set_content(ll, pm_in_get_content(args + 2)))
						SC_ABORT();

					*cl_id = ll;
				}
			}
			break;
		case TGF_DECLARE_SEGMENT:
			cur_seg_uri = strdup(args[0].arg.a_string);
			seg_uris.push_back(cur_seg_uri);
			*cl_id = cur_seg_uri;

			in_freeseg = seg->get_full_uri() == cur_seg_uri;
			break;
		case TGF_SWITCH_TO_SEGMENT:
			in_freeseg = args[0].arg.a_userid == reinterpret_cast<void *>(-1);
			if (!in_freeseg) {
				cur_seg_uri = static_cast<char *>(args[0].arg.a_userid);
				in_freeseg = seg->get_full_uri() == cur_seg_uri;
			}
			break;
		case TGF_FINDBYIDTF:
			SC_ABORT();
		case TGF_SETBEG:
			if (seg->set_beg(sc_addr_ll(args[0].arg.a_userid), sc_addr_ll(args[1].arg.a_userid)))
				SC_ABORT();
			break;
		case TGF_SETEND:
			if (seg->set_end(sc_addr_ll(args[0].arg.a_userid), sc_addr_ll(args[1].arg.a_userid)))
				SC_ABORT();
		default:
			break;
		};

		return 0;
	}
};

static
sc_retval load_segment(sc_segment *seg, const sc_string &path)
{
	int fd;

#ifdef WIN32
	int __l = path.length() - 1;
	if (path[__l] == '\\')	{
		fd = open(path.substr(0, __l).c_str(),O_RDONLY | O_BINARY);
	} else fd = open(path.c_str(),O_RDONLY | O_BINARY);
#else
	fd = open(path.c_str(),O_RDONLY | O_BINARY);
#endif

	if (fd<0)
		return RV_ERR_GEN;
	tgf_stream_in *str = tgf_stream_in_new();
	if (!str)
		return RV_ERR_GEN;
	tgf_stream_in_fd(str,fd);
	if (tgf_stream_in_start(str,0)<0) {
		tgf_stream_in_destroy(str);
		return RV_ERR_GEN;
	}

	tgf_segment_loader loader(seg);
	int rv = tgf_stream_in_process_all(str, tgf_segment_loader::callback,&loader);
	if (rv < 0)
		SC_ABORT_MESSAGE(tgf_error(tgf_stream_in_get_error(str)));

	tgf_stream_in_destroy(str);
	close(fd);
	return RV_OK;
}

sc_segment *	unix_fs_repo::map_segment(const sc_string &uri)
{
	mapped_t::iterator iter = mapped.find(uri);
	if (iter != mapped.end())
		return iter->second;
	struct stat buf;
	int rv;

#ifdef WIN32
	sc_string _buffer = fs_place + uri;
	int __l = _buffer.length() - 1;
	if (_buffer[__l] == '\\')	{
		rv = stat(_buffer.substr(0, __l).c_str(),&buf);
	} else rv = stat(_buffer.c_str(),&buf);
#else
	rv = stat((fs_place + uri).c_str(),&buf);
#endif
	if (rv < 0  ||  !S_ISREG(buf.st_mode)) {
		return 0;
	}

	dprintf("[SC] Loading fs segment '%s'\n",uri.c_str());
	
	tmp_segment *seg = new tmp_segment(uri,this);
	if (buf.st_size == 0) {
		if (!mapped.insert(std::pair<sc_string,tmp_segment *>(uri,seg)).second)
			SC_ABORT();
		return seg;
	}
	if (load_segment(seg,(fs_place + uri).c_str())) {
		delete seg;
		return 0;
	}
	if (!mapped.insert(std::pair<sc_string,tmp_segment *>(uri,seg)).second)
		SC_ABORT();

	if (init_segment(seg)) {
		return seg;
	} else {
		delete seg;
		return 0;
	}
}

sc_retval	unix_fs_repo::unmap_segment(sc_segment *seg)
{
	return RV_OK;
}

// three pass saving
static
sc_retval __save_segment(sc_segment *seg, int fd)
{
	tgf_stream_out *str = tgf_stream_out_new();
	if (!str) {
		return RV_ERR_GEN;
	}
	tgf_stream_out_fd(str,fd);
	if (tgf_stream_out_start(str,"fs_storage")<0) {
		tgf_stream_out_destroy(str);
		return RV_ERR_GEN;
	}
	tgf_command cmd;
	tgf_argument args[3];
	cmd.arguments = args;

	//pass 1. write elements
	sc_segment::iterator *iter = seg->create_iterator_on_elements();
	cmd.type = TGF_GENEL;
	args[0].type = TGF_STRING;
	args[1].type = TGF_SCTYPE;
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();
		if (seg->is_link(ll))
			continue;
		{
//			sc_addr addr = seg->get_element_addr(ll);
/*			if (addr && addr->dead)
			{
				volatile int i=0;
			}*/
			//assert(!addr || !addr->dead);
		}
		sc_string idtf = seg->get_idtf(ll);
		Content cont = seg->get_content(ll);
		sc_type type = seg->get_type(ll);
		args[0].arg.a_string = const_cast<char *>(idtf.c_str());

		if (sc2tgf_type_table[type & SC_POOR_MASK]<0) {
			printf("[SC] Error: Cannot save element \"%s\" with type %s\n",
				(seg->get_full_uri() + "/" + idtf).c_str(), get_type_info(type).c_str());
			SC_ABORT();
		}
		args[1].arg.a_sctype = sc2tgf_type_table[type & SC_POOR_MASK];

		if (cont.type() != TCEMPTY) {
			cmd.arg_cnt = 3;
			fill_cont_argument(cont,args+2,-1,const_cast<char *>(idtf.c_str()));
		} else
			cmd.arg_cnt = 2;
		if (tgf_write_command(str,&cmd,ll)<0)
			SC_ABORT();
	}
	delete iter;
	//stage 2. write links
	//TODO: write this better
	//this may produce bad TGFs depending on our refinements of TGF stream
	iter = seg->create_link_iterator();
	sc_string last_seg;
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();
		sc_string target = seg->get_link_target(ll);
		cmd.type = TGF_DECLARE_SEGMENT;
		cmd.arg_cnt = 1;
		sc_string name = dirname(target);
		if (name != last_seg) {
			args[0].arg.a_string = const_cast<char *>(name.c_str());
			if (tgf_write_command(str,&cmd,0)<0)
				SC_ABORT();
			last_seg = name;
		}
		name = basename(target);
		args[0].arg.a_string = const_cast<char *>(name.c_str());
		args[1].arg.a_sctype = TGF_UNDF;
		cmd.arg_cnt = 2;
		cmd.type = TGF_GENEL;
		if (tgf_write_command(str,&cmd,ll)<0)
			SC_ABORT();
	}
	delete iter;
	// stage 3. write beg/end
	iter = seg->create_iterator_on_elements();
	cmd.arg_cnt = 2;
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();
		if (seg->is_link(ll))
			continue;
		if (!(seg->get_type(ll) & SC_CONNECTOR_MASK))
			continue;
		sc_addr_ll end = seg->get_beg(ll);
		if (!end)
			goto __write_end;
		cmd.type = TGF_SETBEG;
		args[0].arg.a_userid = ll;
		args[1].arg.a_userid = end;
		args[0].type = TGF_USERID;
		args[1].type = TGF_USERID;
		if (tgf_write_command(str,&cmd,0)<0)
			SC_ABORT();
	__write_end:
		end = seg->get_end(ll);
		if (!end)
			continue;
		cmd.type = TGF_SETEND;
		args[0].arg.a_userid = ll;
		args[1].arg.a_userid = end;
		args[0].type = TGF_USERID;
		args[1].type = TGF_USERID;
		if (tgf_write_command(str,&cmd,0)<0)
			SC_ABORT();
	}
	delete iter;
	tgf_stream_out_finish(str);
	tgf_stream_out_destroy(str);
	return RV_OK;
}
static
sc_retval save_segment(sc_segment *seg, const sc_string &path)
{
	int fd = open(path.c_str(),O_WRONLY | O_BINARY | O_TRUNC);
	if (fd<0)
		return RV_ERR_GEN;
	if (__save_segment(seg,fd)) {
		close(fd);
		return RV_ERR_GEN;
	}
	close(fd);
	return RV_OK;
}

void	unix_fs_repo::flush()
{
	dprintf("[FSREPO] Unloading all live segments of fs repository\n");
	mapped_t::iterator iter = mapped.begin();
	for (;iter != mapped.end();iter++)
		save_segment(iter->second,(fs_place + iter->first).c_str());
}

unix_fs_repo::~unix_fs_repo()
{
	dprintf("[FSREPO] Unloading all live segments of fs repository\n");
	mapped_t::iterator iter = mapped.begin();
	for (;iter != mapped.end();iter++) {
		save_segment(iter->second,(fs_place + iter->first).c_str());
		dprintf("%s\n",iter->first.c_str());
		delete iter->second;
	}
}

sc_repo *create_fs_repo(const sc_string &loc,const sc_string &fs_place)
{
	return new unix_fs_repo(loc,fs_place);
}
