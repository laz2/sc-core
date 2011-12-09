
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

static inline
int sc_to_tgf_type(sc_type type) 
{
	return sc2tgf_type_table[type & SC_POOR_MASK];
}

static
int tgf_to_sc_type(tgf_sc_type tgf_type)
{
	int type = tgf2sc_type_table[tgf_type];
	if (type<0)
		SC_ABORT();
	return type;
}

Content pm_in_get_content(tgf_argument *arg)
{
	Cont c;
	TCont ct;
	switch (arg->type) {
	case TGF_INT32:
		c.i = arg->arg.a_int32;
		ct = TCINT;
		break;
	case TGF_INT16:
		c.i = arg->arg.a_int16;
		ct = TCINT;
		break;
	case TGF_DATA:
		c.d.ptr = arg->arg.a_data;
		c.d.size = arg->data_len;
		ct = TCDATA;
		break;
	case TGF_STRING:
		c.d.ptr = arg->arg.a_string;
		c.d.size = strlen(arg->arg.a_string)+1;
		ct = TCSTRING;
		break;
	case TGF_FLOAT:
		c.r = arg->arg.a_float;
		ct = TCREAL;
		break;
	case TGF_LAZY_DATA:
		c.d.ptr = arg->arg.a_string;
		c.d.size = strlen(arg->arg.a_string)+1;
		ct = TCLAZY;
	default:
		SC_ABORT();
	}
	return Content(c,ct);
}

#include <typeinfo>
#include <string.h>

static
sc_retval pm_in_merge_el(sc_session *s,sc_addr addr,sc_type type,const Content &cont)
{
	sc_type eltype = s->get_type(addr);
	bool set_type = false;

	if (eltype == type)
		goto __types_ok;

	if (((eltype ^ type) & (SC_CONSTANCY_MASK /*| SC_PERMANENCY*/)) != SC_EMPTY)
		return RV_ERR_GEN;
	if (sc_type _t = (eltype ^ type) & (SC_NODE|SC_ARC_ACCESSORY)) {
		if (!(_t ^ (SC_NODE|SC_ARC_ACCESSORY)))
			return RV_ERR_GEN;
	}
		
	if ((eltype & SC_SYNTACTIC_MASK) == SC_UNDF)
		set_type = true;
	/*if (((eltype ^ type) & (SC_ACTUALITY)) != SC_EMPTY)
		set_type = true;*/

__types_ok:
	// TODO: revise for lazy data content support
	Content elcont = s->get_content(addr);
	if (elcont.type() != TCEMPTY && cont.type() != TCEMPTY && elcont != cont)
		if (s->set_content(addr,cont))
			return RV_ERR_GEN;
	if (elcont.type() == TCEMPTY && cont.type() != TCEMPTY)
		if (s->set_content(addr,cont))
			return RV_ERR_GEN;
	if (set_type)
		if (s->change_type(addr,type))
			return RV_ERR_GEN;
	return RV_OK;
}

struct extra_data {
	sc_session *session;
	sc_segment *segment;
	sc_segment *free_segment;
	sc_addr set;
};

static
int	pm_reader_genel(tgf_command *cmd,void **cl_id,extra_data *extra)
{
	sc_type type;
	sc_addr addr;
	sc_string str;
	Content cont;

	str = cmd->arguments[0].arg.a_string;
	// special hack for malformed types in fs_repo's TGFs
	if (cmd->arguments[1].arg.a_sctype == 0x30) {
		addr = extra->session->find_by_idtf(str,extra->segment);
		if (!addr) {
			addr = extra->session->create_el(extra->segment,SC_U_CONST);
			if (!addr)
				SC_ABORT();
			if (extra->session->set_idtf(addr,str))
				SC_ABORT();
		}
		goto addr_done;
	}
	type = tgf_to_sc_type(cmd->arguments[1].arg.a_sctype);
	if (cmd->arg_cnt > 2)
		cont = pm_in_get_content(cmd->arguments+2);
	addr = extra->session->find_by_idtf(str,extra->segment);
	if (addr) {
		if (pm_in_merge_el(extra->session,addr,type,cont)) {
			fprintf(stderr,"Cannot merge element received and found\n");
			SC_ABORT();
		}
	} else {
		addr = extra->session->create_el(extra->segment,type);
		if (!addr || extra->session->get_error())
			SC_ABORT();
		if (str.size())
			extra->session->set_idtf(addr,str);
		if (cont.type() != TCEMPTY && extra->session->set_content(addr,cont))
			SC_ABORT();
	}
addr_done:
	if (extra->set) {
		sc_retval rv;
		rv = system_session->gen3_f_a_f(extra->set,0,extra->set->seg,SC_A_CONST|SC_POS,addr);
		if (rv)
			SC_ABORT();
	}
	*cl_id = addr;
	return 0;
}

static
int	pm_reader_declare_seg(tgf_command *cmd,void **cl_id,extra_data *data)
{
	sc_string path = cmd->arguments[0].arg.a_string;
	sc_segment *seg;
	if (!data->session->_stat(path)) {
		seg = data->session->open_segment(path);
		assert(seg);
	} else {
		seg = create_segment_full_path(data->session,path);
		if (!seg) {
			fprintf(stderr,"Cannot create path passed: %s\n", path.c_str());
			return -1;
		}
	}
	data->segment = seg;
	*cl_id = seg;
	return 0;
}

static
int	pm_reader_callback(tgf_command *cmd,void **cl_id,void *extra)
{
	extra_data *data = (extra_data *)extra;
	sc_addr a1,a2;
	*cl_id = 0;
	switch (cmd->type) {
	case TGF_NOP:
		break;
	case TGF_GENEL:
		return pm_reader_genel(cmd,cl_id,data);
	case TGF_DECLARE_SEGMENT:
		return pm_reader_declare_seg(cmd,cl_id,data);
	case TGF_SWITCH_TO_SEGMENT:
		if (cmd->arguments[0].arg.a_userid == (void *)-1) {
			data->segment = data->free_segment;
			break;
		}
		data->segment = reinterpret_cast <sc_segment *>(cmd->arguments[0].arg.a_userid);
		data->session->error = RV_OK;
		break;
	case TGF_SETBEG:
		a1 = sc_addr(cmd->arguments[0].arg.a_userid);
		a2 = sc_addr(cmd->arguments[1].arg.a_userid);
		data->session->set_beg(a1,a2);
		break;
	case TGF_SETEND:
		a1 = sc_addr(cmd->arguments[0].arg.a_userid);
		a2 = sc_addr(cmd->arguments[1].arg.a_userid);
		data->session->set_end(a1,a2);
		break;
	case TGF_ENDOFSTREAM:
		return 0;
	default:
		fprintf(stderr,"Unknown or unsupported TGF command\n");
		SC_ABORT();
	}
	return data->session->get_error();
}

sc_retval read_tgf_to(tgf_stream_in *st, sc_session *s, sc_segment *seg, sc_addr collect_set)
{
	extra_data data;

	data.session = s;
	data.segment = seg;
	data.free_segment = seg;
	data.set = collect_set;

	if (tgf_stream_in_process_all(st,pm_reader_callback,&data)<0) {
		fprintf(stderr,"tgf_error: %s\n",tgf_error(tgf_stream_in_get_error(st)));
		return RV_ERR_GEN;
	}

	return RV_OK;
}

void fill_cont_argument(const Content &cont,tgf_argument *arg, int lazy_limit, const char *idtf)
{
	switch(cont.type()) {
	case TCSTRING:
		arg->type = TGF_STRING;
		arg->arg.a_string = Cont(cont).d.ptr;
		break;
	case TCINT:
		arg->type = TGF_INT32;
		arg->arg.a_int32 = Cont(cont).i;
		break;
	case TCREAL:
		arg->type = TGF_FLOAT;
		arg->arg.a_float = Cont(cont).r;
		break;
	case TCDATA:
		if (lazy_limit<0 || (arg->data_len = Cont(cont).d.size) < lazy_limit) {
			arg->type = TGF_DATA;
			arg->arg.a_data = Cont(cont).d.ptr;
			arg->data_len = Cont(cont).d.size;
			break;
		}
	case TCLAZY:
		arg->type = TGF_LAZY_DATA;
		arg->arg.a_string = const_cast<char *>(idtf);
		break;
	default:
		SC_ABORT();
	}
}

char *	sc_string_strdup(const sc_string &str)
{
	int len;
	char *res = (char *) malloc((len = str.size()) + 1);
	if (!res) {
		fprintf(stderr,"Memory exhausted !\n");
		SC_ABORT();
	}
	memcpy(res,str.data(),len);
	res[len] = 0;
	return res;
}

static
sc_retval write_declare_segment(tgf_stream_out *str,sc_segment *s)
{
	tgf_command cmd;
	tgf_argument arg;
	cmd.arguments = &arg;
	cmd.type = TGF_DECLARE_SEGMENT;
	cmd.arg_cnt = 1;
	arg.type = TGF_STRING;
	sc_string tmp = s->get_full_uri();
	arg.arg.a_string = (char *)(tmp.c_str());
	if (tgf_write_command(str,&cmd,s)<0) {
		SC_ABORT();
		return RV_ERR_GEN;
	}
	return RV_OK;
}

static
sc_retval write_switch_to_segment(tgf_stream_out *str,sc_segment *s)
{
	tgf_command cmd;
	tgf_argument arg;
	cmd.arguments = &arg;
	cmd.type = TGF_SWITCH_TO_SEGMENT;
	cmd.arg_cnt = 1;
	arg.type = TGF_USERID;
	arg.arg.a_userid = s;
	if (tgf_write_command(str,&cmd,0)<0) {
		SC_ABORT();
		return RV_ERR_GEN;
	}
	return RV_OK;
}

sc_retval pm_writer::go_to_segment(sc_segment *s)
{
	if (s == free_segment)
		return write_switch_to_segment(stream,(sc_segment *)-1);
	if (!tgf_stream_out_is_written(stream,s))
		return write_declare_segment(stream,s);
	return write_switch_to_segment(stream,s);
}

sc_retval pm_writer::write(sc_addr element)
{
	sc_type type;
	tgf_command cmd;
	tgf_argument args[3];
	Content cont;
	char *idtf;
	int rv;
	if (tgf_stream_out_is_written(stream,element))
		return RV_OK;
	cmd.arguments = args;
	cmd.type = TGF_GENEL;
	type = sess->get_type(element);
	cont = sess->get_content(element);
	if (sess->get_error())
		return sess->get_error();

	if (type & SC_CONNECTOR_MASK) {
		arc_map::iterator iter = arcs.find(element);
		sc_addr beg,end;
		if (iter != arcs.end()) {
			SC_ABORT();
			return RV_ELSE_GEN;
		}
		beg = sess->get_beg(element);
		end = sess->get_end(element);
		if (beg)
			write(beg);
		if (end)
			write(end);
		arcs.insert(std::pair<sc_addr, addr2addr_pair >(element, addr2addr_pair(beg,end)));
	}

	if (cur_segment != element->seg)
		if (go_to_segment(element->seg)) {
			SC_ABORT();
			return RV_ERR_GEN;
		} else
			cur_segment = element->seg;
	args[1].type = TGF_SCTYPE;
	if (sc_to_tgf_type(type) < 0) {
		SC_ABORT();
		return RV_ERR_GEN;
	}
	args[1].arg.a_sctype = tgf_sc_type(sc_to_tgf_type(type));
	args[0].type = TGF_STRING;
	idtf = args[0].arg.a_string = sc_string_strdup(sess->get_idtf(element));
	cmd.arg_cnt = 2;
	if (cont.type() != TCEMPTY) {
		cmd.arg_cnt = 3;
		fill_cont_argument(cont,args+2,lazy_limit,idtf);
	}
	rv = tgf_write_command(stream,&cmd,element);
	if (idtf)
		free(idtf);
	if (rv<0) {
		SC_ABORT();
		return RV_ERR_GEN;
	}
	return RV_OK;
}

sc_retval pm_writer::arc_sync()
{
	arc_map::iterator iter = arcs.begin();
	tgf_command cmd;
	tgf_argument args[2];

	cmd.arguments = args;
	cmd.arg_cnt = 2;
	for (;iter != arcs.end();iter++) {
		sc_addr arc,beg,end;

		arc = (*iter).first;
		beg = (*iter).second.first;
		end = (*iter).second.second;
		if (beg && tgf_stream_out_is_written(stream,beg)) {
			args[0].type = TGF_USERID;
			args[0].arg.a_userid = arc;
			args[1].type = TGF_USERID;
			args[1].arg.a_userid = beg;
			cmd.type = TGF_SETBEG;
			if (tgf_write_command(stream,&cmd,0)<0)
				return RV_ERR_GEN;
		}
		if (end && tgf_stream_out_is_written(stream,end)) {
			args[0].type = TGF_USERID;
			args[0].arg.a_userid = arc;
			args[1].type = TGF_USERID;
			args[1].arg.a_userid = end;
			cmd.type = TGF_SETEND;
			if (tgf_write_command(stream,&cmd,0)<0)
				return RV_ERR_GEN;
		}
	}
	arcs.clear();
	return RV_OK;
}

pm_writer::~pm_writer()
{
	arc_sync();
}

class sc_tgf_ostream
{
public:
	sc_tgf_ostream(tgf_stream_out *_out) : out(_out)
	{
	}

	void declare(sc_segment *seg)
	{
		tgf_command cmd;
		tgf_argument arg;
		cmd.arguments = &arg;

		cmd.type = TGF_DECLARE_SEGMENT;
		cmd.arg_cnt = 1;

		arg.type = TGF_STRING;
		sc_string uri = seg->get_full_uri().c_str();
		arg.arg.a_string = const_cast <char *>(uri.c_str());

		if (tgf_write_command(out, &cmd, seg) < 0) {
			fprintf(stderr,"tgf_error: %s\n",tgf_error(tgf_stream_out_get_error(out)));
		}
	}

	void gen_el(sc_segment *seg, sc_addr_ll ll, bool write_content=true)
	{
		tgf_command cmd;
		tgf_argument args[3];

		cmd.arguments = args;

		cmd.type = TGF_GENEL;

		// Type of element
		sc_type type = seg->get_type(ll);
		args[1].type = TGF_SCTYPE;
		if (sc_to_tgf_type(type) < 0) {
			fprintf(stderr,"tgf_error: cannot convert %s to tgf type\n", get_type_info(type).c_str());
			return;
		}
		args[1].arg.a_sctype = tgf_sc_type(sc_to_tgf_type(type));

		// Idtf of element
		args[0].type = TGF_STRING;
		sc_string idtf = seg->get_idtf(ll);
		boost::scoped_ptr<char> idtf_copy(sc_string_strdup(seg->get_idtf(ll)));
		// Write real idtf only if it isn't system idtf.
		// Expect system idtf starts with '@@'.
		if (idtf.length() <= 2 || idtf.compare(0, 2, "@@")) {
			args[0].arg.a_string = idtf_copy.get();
		} else {
			args[0].arg.a_string = "";
		}
		cmd.arg_cnt = 2;

		if (write_content) {
			// If content present then add content to arguments
			const Content *cont = seg->get_content_const(ll);
			if (cont->type() != TCEMPTY) {
				++cmd.arg_cnt;
				fill_cont_argument(*cont, args + 2, -1, idtf_copy.get());
			}
		}

		if (tgf_write_command(out, &cmd, seg->get_element_addr(ll)) < 0) {
			fprintf(stderr,"tgf_error: %s\n",tgf_error(tgf_stream_out_get_error(out)));
			return;
		}
	}

	void set_ends(sc_segment *seg, sc_addr_ll ll)
	{
		tgf_command cmd;
		tgf_argument args[2];
		cmd.arguments = args;

		sc_addr    addr   = seg->get_element_addr(ll);

		sc_addr_ll beg_ll = seg->get_beg(ll);
		sc_addr_ll end_ll = seg->get_end(ll);
		sc_addr    beg    = 0;
		sc_addr    end    = 0;

		if (beg_ll)
			beg = seg->get_element_addr(beg_ll);

		if (end_ll)
			end  = seg->get_element_addr(end_ll);

		cmd.arg_cnt = 2;
		if (beg) {
			args[0].type = TGF_USERID;
			args[1].type = TGF_USERID;
			args[0].arg.a_userid = addr;
			args[1].arg.a_userid = beg;
			cmd.type = TGF_SETBEG;

			if (tgf_write_command(out, &cmd, 0) < 0) {
				fprintf(stderr,"tgf_error: %s\n",tgf_error(tgf_stream_out_get_error(out)));
				return;
			}
		}

		if (end) {
			args[0].type = TGF_USERID;
			args[1].type = TGF_USERID;
			args[0].arg.a_userid = addr;
			args[1].arg.a_userid = end;
			cmd.type = TGF_SETEND;

			if (tgf_write_command(out, &cmd, 0) < 0) {
				fprintf(stderr,"tgf_error: %s\n",tgf_error(tgf_stream_out_get_error(out)));
				return;
			}
		}
	}

	void switch_to_free()
	{
		switch_to(reinterpret_cast<sc_segment *>(-1));
	}

	void switch_to(sc_segment *seg)
	{
		tgf_command cmd;
		tgf_argument arg;
		cmd.arguments = &arg;

		cmd.type = TGF_SWITCH_TO_SEGMENT;
		cmd.arg_cnt = 1;

		arg.type = TGF_USERID;
		arg.arg.a_userid = seg;

		if (tgf_write_command(out, &cmd, 0) < 0) {
			fprintf(stderr,"tgf_error: %s\n",tgf_error(tgf_stream_out_get_error(out)));
			return;
		}
	}

private:
	tgf_stream_out *out;
};

/// This code does not support lazy data, but it is not used in non-obsolete code.
/// Pass 1. write elements, but not links.
static
sc_retval write_segment1(sc_segment *seg, tgf_stream_out *str, bool need_freeseg)
{
	sc_tgf_ostream out(str);

	if (!need_freeseg) {
		//
		// Declare in TGF segment @c seg if not needed free segment.
		//
		out.declare(seg);
	}

	//
	// Write elements, not links.
	//
	{
		boost::scoped_ptr<sc_segment::iterator> iter(seg->create_iterator_on_elements());
		while (!iter->is_over()) {
			sc_addr_ll ll = iter->next();

			if (seg->is_link(ll))
				continue;

			sc_addr addr = seg->get_element_addr(ll);
			if (!addr || addr->is_dead())
				continue;

			out.gen_el(seg, ll);
		}
	}
	return RV_OK;
}

/// Pass 2. write links
static
sc_retval write_segment2(sc_session *s, sc_segment *seg, tgf_stream_out *str)
{
	sc_tgf_ostream out(str);

	sc_segment *cur_seg = 0;

	boost::scoped_ptr<sc_segment::iterator> iter(seg->create_link_iterator());
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();

		if (!seg->is_link(ll))
			continue;

		sc_addr addr = seg->get_element_addr(ll);
		if (!addr || tgf_stream_out_is_written(str, addr))
			continue;

		sc_segment *target_seg = s->open_segment(dirname(seg->get_link_target(ll)));
		if (!target_seg)
			continue;

		//
		// If @c target_seg isn't current target segment
		// then declare or switch to new target segment.
		//
		if (cur_seg != target_seg) {
			if (!tgf_stream_out_is_written(str, target_seg)) {
				out.declare(target_seg);
			} else {
				out.switch_to(target_seg);
			}

			cur_seg = target_seg;
		}

		out.gen_el(addr->seg, addr->impl, false);
	}

	return RV_OK;
}

// pass 3. write arc ends
static
sc_retval write_segment3(sc_segment *seg, tgf_stream_out *str)
{
	sc_tgf_ostream out(str);

	boost::scoped_ptr<sc_segment::iterator> iter(seg->create_iterator_on_elements());
	while (!iter->is_over()) {
		sc_addr_ll ll = iter->next();

		if (seg->is_link(ll) || !(seg->get_type(ll) & SC_CONNECTOR_MASK))
			continue;

		sc_addr addr = seg->get_element_addr(ll);
		if (!addr && addr->is_dead())
			continue;

		out.set_ends(seg, ll);
	}

	return RV_OK;
}

void write_to(sc_session *s, sc_segment *seg, tgf_stream_out *stream, bool need_freeseg)
{
	write_segment1(seg, stream, need_freeseg);
	write_segment2(s, seg, stream);
	write_segment3(seg, stream);
}

typedef std::list<sc_segment *> seglist;

static
sc_retval dirhier_collect(sc_session *s,const sc_string &uri,seglist &list)
{
	sc_segment *meta = s->open_segment(uri+"/META");
	sc_retval rv;
	if (!meta)
		return RV_ERR_GEN;
	sc_iterator *iter = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, meta, false), true);
	for (;!iter->is_over();iter->next()) {
		sc_addr addr = iter->value(0);
		if (s->get_type(addr) & (SC_CONNECTOR_MASK|SC_UNDF))
			continue;
		sc_string tmp = s->get_idtf(addr);
		if (tmp == "META")
			continue;
		sc_string curi = uri+"/"+tmp;
		rv = s->_stat(curi);
		if (RV_IS_ERR(rv)) {
			printf("Warning: wrong sign of segment(%s) detected\n",curi.c_str());
			continue;
		}
		if (rv) {
			if (dirhier_collect(s,curi,list))
				SC_ABORT();
			continue;
		}
		sc_segment *cseg = s->open_segment(curi);
		if (!cseg)
			SC_ABORT();
		list.push_back(cseg);
	}
	delete iter;
	return RV_OK;
}

sc_retval pm_write_dirhier(sc_session *s,const sc_string &uri,tgf_stream_out *stream)
{
	seglist list;
	if (dirhier_collect(s,uri,list))
		return RV_ERR_GEN;
	seglist::iterator iter = list.begin();
	for (;iter != list.end();iter++)
		if (write_segment1(*iter, stream, false))
			return RV_ERR_GEN;
	iter = list.begin();
	for (;iter != list.end();iter++)
		if (write_segment2(s,*iter,stream))
			return RV_ERR_GEN;
	iter = list.begin();
	for (;iter != list.end();iter++)
		if (write_segment3(*iter,stream))
			return RV_ERR_GEN;
	return RV_OK;
}

// TODO: implement it more efficiently eventually
static
int tgfout_write_cust (void *fd, char *buf, int size)
{
	char * temp;
	struct tgf_dump * mem = (struct tgf_dump *) fd;

	if (mem->data)
		temp = (char *)realloc(mem->data,mem->size+size);
	else
		temp = (char *)malloc(size);

	if (!temp)
		SC_ABORT_MESSAGE("Memory exausted\n");

	memcpy (temp+mem->size, buf, size);
	mem->size += size;
	mem->data = temp;
	return size;
}

tgf_dump * tgf2mem_dumper_new()
{
	tgf_stream_out *stream = tgf_stream_out_new();
	if (!stream)
		SC_ABORT();
	tgf_dump *dump = new tgf_dump;
	tgf_stream_out_custom(stream,tgfout_write_cust,dump);
	tgf_stream_out_start(stream,"from_PM!");
	dump->stream = stream;
	return dump;
}

void	tgf2mem_dumper_delete(tgf_dump *d)
{
	tgf_stream_out_destroy(d->stream);
	delete d;
}

