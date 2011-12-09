/*
 *  SCPx processing module project.
 *
 *  tgfin.c - functions to read TGF
 *
 *  Created at Mon 07 May 2001 01:19:18 AM EEST by ALK
 *  New format, interface generalization and speedup
 *    at Thu 08 Nov 2001 02:55:15 AM EET by ALK
 *
 *  Deflate (de)compression support
 *    at Thu Jun 12 21:59:18 2003 by ALK
 *
 *  Note: this code will need some corrections in case of more real CRC
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tgfin.h"

#include "bytesex_p.h"
#include "tgfmap.h"
#include "tgfdebug_p.h"
#include "tgfutil_p.h"
#include <assert.h>

#define	TGF_READ_BUFSIZE	16384

#ifdef USE_ZLIB
#include <zlib.h>

typedef struct _tgf_z_read {
	tgf_io_fn read_fn;
	void *fd;
	char *buffer;
	char *text,*text_end;
	int eof;
	z_stream z;
} tgf_z_read;
#endif

struct _tgf_stream_in {
	tgf_io_fn read_fn;
	void *fd;
	char *buffer;
	char *text,*text_end;

	tgf_uint16 (*conv16)(tgf_uint16);
	tgf_uint32 (*conv32)(tgf_uint32);

	tgf_allocator *alloc;

	int crc;
	tgf_uint32 cmd_index;
	tgf_command_header cmd;
	tgf_header_user tgf_hdr;

	int last_err;

	tgf_map_in *hash;
};

#ifdef USE_ZLIB

static
int	z_reader(tgf_z_read *fd,void *buf,int len)
{
	int res;
	if (fd->eof)
		return 0;
	do {
		if (!(fd->text_end - fd->text)) {
			int res;
			if (!fd->read_fn)
				return -1;
			res = fd->read_fn(fd->fd,fd->buffer,TGF_READ_BUFSIZE);
			if (res <= 0)
				return -1;
			fd->text = fd->buffer;
			fd->text_end = fd->text + res;
		}
		fd->z.next_in = (Bytef *)fd->text;
		fd->z.avail_in = fd->text_end - fd->text;
		fd->z.next_out = buf;
		fd->z.avail_out = len;
		res = inflate(&fd->z,Z_NO_FLUSH);
		if (res == Z_STREAM_END) {
			fd->eof = 1;
			break;
		}
		if (res != Z_OK)
			return -1;
		fd->text = (char *)fd->z.next_in;
	} while (fd->z.avail_out == len);
	return len-fd->z.avail_out;
}

/* z_read part is much simpler than z_write part */
static
int	setup_read_compression(tgf_stream_in *s)
{
	tgf_z_read *fd = malloc(sizeof(tgf_z_read));
	if (!fd)
		return E_TGF_NOMEM;
	memcpy(fd,s,(char *)&fd->z - (char*)fd);
	fd->eof = 0;
	memset(&fd->z,0,sizeof(z_stream));

	s->buffer = malloc(TGF_READ_BUFSIZE);
	if (!s->buffer) {
		free(fd);
		return E_TGF_NOMEM;
	}
	s->text_end = s->text = s->buffer + TGF_READ_BUFSIZE;
	s->fd = fd;
	s->read_fn = (tgf_io_fn)z_reader;
	inflateInit(&fd->z);
	return 0;
}
#endif /* USE_ZLIB */

int	tgf_stream_in_get_error(tgf_stream_in *s)
{
	return s->last_err;
}

/* If we handle only little and big endian than this code is OK */
static
tgf_uint16	convert16(tgf_uint16 s)
{
	return tgf_ntohs(s);
}
static
tgf_uint32	convert32(tgf_uint32 s)
{
	return tgf_ntohl(s);
}

/*#ifndef HAVE_INLINE
static
tgf_uint16	noconvert16(tgf_uint16 s)
{
	return s;
}
static
tgf_uint32	noconvert32(tgf_uint32 s)
{
	return s;
}
#else*/
/* using this is faster (?) (if inline is supported) */
static INLINE
tgf_uint16 conv16(tgf_stream_in *s,tgf_uint16 i)
{
	if (!s->conv16)
		return i;
	return s->conv16(i);
}
static INLINE
tgf_uint32 conv32(tgf_stream_in *s,tgf_uint32 i)
{
	if (!s->conv32)
		return i;
	return s->conv32(i);
}
/*#endif*/

tgf_stream_in *tgf_stream_in_new(void)
{
	tgf_stream_in *p;

	p = malloc(sizeof(tgf_stream_in));
	if (!p)
		return p;
	p->hash = tgf_map_in_new();
	if (!p->hash) {
		free(p);
		return 0;
	}
	p->alloc = tgf_allocator_new(0);
	if (!p->alloc) {
		tgf_map_in_destroy(p->hash);
		free(p);
		return 0;
	}
	p->fd = (void *)-1;
	p->buffer = 0;
	p->crc = 0;
	p->text = p->text_end = 0;
	p->cmd_index = 0;
	p->tgf_hdr.tgf_id = 0;
	p->last_err = 0;
	p->conv16 = 0;
	p->conv32 = 0;
	return p;
}

void	tgf_stream_in_destroy(tgf_stream_in *stream)
{
	if (!stream)
		return;
	if (stream->buffer)
		free(stream->buffer);
	if (stream->hash)
		tgf_map_in_destroy(stream->hash);
	if (stream->alloc)
		tgf_allocator_delete(stream->alloc);
	if (stream->tgf_hdr.tgf_id)
		free(stream->tgf_hdr.tgf_id);
#	ifdef USE_ZLIB
		if (stream->tgf_hdr.compression) {
			tgf_z_read *fd = stream->fd;
			if (fd->buffer)
				free(fd->buffer);
			inflateEnd(&fd->z);
			free(fd);
		}
#	endif
	free(stream);
}

int	tgf_stream_in_buf(tgf_stream_in *stream,char *start,int size)
{
/*	if (size&3)
		return stream->last_err = E_TGF_INVAL;*/
	stream->read_fn = __tgf_read_fn;
	if (stream->fd != (void *)-1) {
		if (stream->buffer) {
			free(stream->buffer);
			stream->buffer = 0;
		}
		stream->fd = (void *)-1;
	}
	stream->text = start;
	stream->text_end = start+size;
	return stream->last_err = 0;
}

void	tgf_stream_in_fd(tgf_stream_in *stream,int fd)
{
	stream->read_fn = __tgf_read_fn;
	stream->buffer = malloc(TGF_READ_BUFSIZE);
	if (stream->buffer)
		stream->fd = (void *)fd;
	stream->text = stream->text_end = NULL;
}

void	tgf_stream_in_custom(tgf_stream_in *s,tgf_io_fn fn,void *cust)
{
	s->read_fn = fn;
	s->buffer = malloc(TGF_READ_BUFSIZE);
	if (s->buffer)
		s->fd = cust;
	s->text = s->text_end = NULL;
}


static
int	tgf_read(tgf_stream_in *s,void *buf,int size)
{
	int raiden=0, /* actually, `readen' ;<) */
		ts,t;
	if (!size)
		return 0;
	do {
		if ((ts=s->text_end-s->text)>0) {
			t = MIN(ts,size);
			memcpy(buf,s->text,t);
			tgf_update_crc(&(s->crc),buf,t);
			s->text += t;
			buf = (char *)buf + t;
			size -= t;
			raiden += t;
			if (!size)
				return raiden;
		}
		t = 0;
		if (s->fd != (void *)-1) {
			unsigned allign = ((long unsigned)(s->text-(char *)0))&3;
			t = s->read_fn(s->fd,s->buffer+allign,TGF_READ_BUFSIZE-allign);
			if (t<0)
				return E_TGF_IO;
			if (!t)
				s->fd = (void *)-1;
			else {
				s->text = s->buffer+allign;
				s->text_end = s->text + t;
			}
		}
	} while (t);
	return E_TGF_EOF;
}

/* NOTE: it doesn't guarantee that whole buffer will be filled */
/*  but it must read at least 1 byte */
/* rest cannot be more than 3 */
/*
   This function may be called only in functions that require it
 */
/*
   This function must maintain following condition:
     stream offset of s->buffer must be alligned by 4
     and it must preserve allignment of s->text
 */
/*
   must make real text size at least 4 bytes
 */
static INLINE
int	tgf_fill_buffer(tgf_stream_in *s)
{
	int t=0;
	int rv = 0;
	unsigned rest = s->text_end - s->text;
	unsigned allign = ((unsigned long)(s->text))&3;
	char *buf = s->buffer + allign;
	if (s->fd == (void *)-1)
		return E_TGF_EOF;
	/* unrolled loop. I hope it's faster than memcpy */
	/* I think probability of rest == 0 is very high */
	assert(rest<4);
	if (rest) {
		char *text = s->text;
		switch (rest) {
		default:
			*buf++ = *text++;
		case 2:
			*buf++ = *text++;
		case 1:
			*buf++ = *text++;
		}
	}
	/* assert(s->buf - s->buffer == rest+allign); */
	t += rv = s->read_fn(s->fd,buf,TGF_READ_BUFSIZE-rest-allign);
	if (rv<0)
		return E_TGF_IO;
	if (rv == 0)
		return E_TGF_EOF;
	while (rest+t<4) {
		buf += rv;
		t += rv = s->read_fn(s->fd,buf,TGF_READ_BUFSIZE-rest-allign-t);
		if (rv<0)
			return E_TGF_IO;
		if (rv == 0)
			break;
	}
	s->text = s->buffer + allign;
	s->text_end = s->text + t + rest;
	return t+rest;
}

/* reads data alligned to 4 by offset and size */
/* `count' is number of tgf_uint32 to read, NOT bytes */
/* will read all or will return error */
static
int	tgf_read_align4(tgf_stream_in *s,void *_buf,int _count)
{
	int t;
	char *buf;
	int count;
	if (!_count)
		return 0;
	count = _count;
	/* alignment */
	do {
		/* this may be necessary on some 64-bit systems.
		   Not sure */
		/* unsigned to make it work on some very ancient machines */
		unsigned long _t = (s->text-(char *)0);
		/* tricky code */
		/* GCC is so smart that it will make ((-_t)&3) */
		buf = s->text + ((~(_t-1))&3);
		if (buf<s->text_end) {
			if (s->text_end - buf>3)
				break;
			else
				s->text = buf;
		}
		/* TODO: make tgf_fill_buffer_no_rest_copy */
		if ((t = tgf_fill_buffer(s))<0)
			return t;
		/* _t is the same as several lines before
			but this form gives gcc information
			that _t and s->text may share register,
			and _t need not be preserved across tgf_fill_buffer */
		_t = (s->text-(char *)0);
		s->text += ((~(_t-1))&3);
	} while (1);
	s->text = buf;
	buf = _buf;
	do {
		t = ((s->text_end - s->text)>>2);
		if (t>=count) {
			__memcpy_move4(buf,s->text,count);
			tgf_update_crc(&s->crc,_buf,_count*4);
			return _count;
		}
		__memcpy_move4(buf,s->text,t);
		count -= t;
		if ((t = tgf_fill_buffer(s))<0)
			return t;
	} while(1);
	return 0;
}

int	tgf_stream_in_start(tgf_stream_in *s,tgf_header_user *hdr)
{
	int t = tgf_read(s,&(s->tgf_hdr),TGF_HEADER_SIZE);
	if (t<0)
		return t;
	s->conv16 = 0;
	s->conv32 = 0;
	if (s->tgf_hdr.signature[0] != 'T' ||
			s->tgf_hdr.signature[1] != 'G' ||
			s->tgf_hdr.signature[2] != 'F')
		return E_TGF_INVHDR;
	if (s->tgf_hdr.major_ver != TGF_MAJOR_VER ||
			(s->tgf_hdr.minor_ver != TGF_MINOR_VER
					/* backward compatibility with 3.0 */
					&& s->tgf_hdr.minor_ver != 0))
		return E_TGF_VER;
	if (s->tgf_hdr.tgf_id)
		free(s->tgf_hdr.tgf_id);
	s->tgf_hdr.tgf_id = malloc(s->tgf_hdr.id_size+1);
	if (!s->tgf_hdr.tgf_id)
		return E_TGF_NOMEM;
	t = tgf_read(s,s->tgf_hdr.tgf_id,s->tgf_hdr.id_size);
	if (t<0) {
		free(s->tgf_hdr.tgf_id);
		return t;
	}
	s->tgf_hdr.tgf_id[s->tgf_hdr.id_size]=0;
	if (s->tgf_hdr.endianness != TGF_HOST_ENDIANNESS) {
		s->conv16 = convert16;
		s->conv32 = convert32;
	}
	s->cmd_index = 0;
	if (hdr)
		memcpy(hdr,&(s->tgf_hdr),sizeof(tgf_header));
	s->last_err = 0;
#	ifdef USE_ZLIB
		if (s->tgf_hdr.compression)
			s->last_err = setup_read_compression(s);
#	else
		if (s->tgf_hdr.compression)
			s->last_err = E_TGF_FEATURE;
#	endif
	return s->last_err;
}

static
int	tgf_read_byte(tgf_stream_in *s)
{
	int ch;
	if (!(s->text_end-s->text)) {
		if ((ch = tgf_fill_buffer(s))<=0)
			return s->last_err = ch;
	}
	ch = (unsigned char )(*(s->text)++);
	tgf_update_crc_byte(&s->crc,ch);
	return ch;
}

/* used only by read_argument. So int32 is alligned on 4 */
static
int	tgf_read_int32(tgf_stream_in *s,int *buf)
{
	int rv;
	while ((s->text_end-s->text)<4) {
		if ((rv = tgf_fill_buffer(s))<0)
			return s->last_err = rv;
	}
	rv = *CAST_LV(s->text,tgf_uint32 *)++;
	tgf_update_crc_uint32(&s->crc,rv);
	*buf = conv32(s,rv);
	return 0;
}

/* used only by read_argument. So int16 is alligned by 4 */
static
int	tgf_read_int16(tgf_stream_in *s,int *buf)
{
	tgf_sint16 rv;
	while ((s->text_end-s->text)<2) {
		if ((rv = tgf_fill_buffer(s))<=2)
			return s->last_err = rv;
	}
	rv = *CAST_LV(s->text,tgf_sint16 *)++;
	tgf_update_crc_byte(&s->crc,((char *)&rv)[0]);
	tgf_update_crc_byte(&s->crc,((char *)&rv)[1]);
	*buf = conv16(s,rv);
	return 0;
}

static
int	tgf_read_sctype(tgf_stream_in *s,tgf_sc_type *buf)
{
	int rv=tgf_read_byte(s);
	if (rv<0)
		return rv;
	*buf = (char)rv;
	return 0;
}

static
int	tgf_read_data(tgf_stream_in *s,char **buf,int *len)
{
	int datasize;
	int rv = tgf_read_int32(s,&datasize);
	if (rv < 0)
		return rv;
	if (!(*buf = tgf_allocator_alloc(s->alloc,datasize)))
		return E_TGF_NOMEM;
	*len = datasize;
	return tgf_read(s,*buf,datasize);
}

static
int	tgf_read_string(tgf_stream_in *s,char **buf)
{
	int datasize;
	int rv = tgf_read_int32(s,&datasize);
	if (rv < 0)
		return rv;
	if (!(*buf = tgf_allocator_alloc(s->alloc,datasize+1)))
		return E_TGF_NOMEM;
	rv = tgf_read(s,*buf,datasize);
	(*buf)[(int)datasize]=0;
	return rv;
}

static
int	tgf_read_command_hdr(tgf_stream_in *s,tgf_command_header *cmd)
{
	int rv;
	s->crc = 0;
	rv = tgf_read_align4(s,(&s->cmd),1);
	if (rv<0)
		return rv;
	s->cmd.cmd_type = conv16(s,s->cmd.cmd_type);
	s->cmd.arg_cnt = conv16(s,s->cmd.arg_cnt);
	if ((unsigned)s->cmd.cmd_type>TGF_CMD_LAST)
		return E_TGF_OPNOTSUPP;
	if (cmd)
		memcpy(cmd,&(s->cmd),sizeof(*cmd));
	return 0;
}

/*
 * I assume that sizeof(double) == 8
 *  this is true for all 32-bit and 64-bit platforms
 */
/* just flip halves and conv them */
static
void	convert_float(double *_flt)
{
	unsigned *flt = (unsigned *)_flt;
	tgf_uint32 tmp;
	tmp = tgf_ntohl(flt[0]);
	flt[0] = tgf_ntohl(flt[1]);
	flt[1] = tmp;
}

static
int	tgf_read_argument(tgf_stream_in *s,tgf_argument *arg)
{
	int rv;
	int type;
	if (!arg)
		return E_TGF_INVAL;
	rv = tgf_read_align4(s,&type,1);
	if (rv<0)
		return rv;
	type = conv32(s,type);
	arg->type = type;
	switch (type) {
	case TGF_INT32:
		return tgf_read_int32(s,&(arg->arg.a_int32));
	case TGF_INT64:
		return E_TGF_OPNOTSUPP;
	case TGF_FLOAT:
		/* assuming IEEE754 floating point number */
		rv = tgf_read(s,&(arg->arg.a_float),sizeof(double));
		if (s->conv32 && rv >= 0)
			convert_float(&arg->arg.a_float);
		return rv;
	case TGF_STRING:
		return tgf_read_string(s,&(arg->arg.a_string));
	case TGF_DATA:
		return tgf_read_data(s,
				&(arg->arg.a_data),
				&arg->data_len);
	case TGF_SCTYPE:
		return tgf_read_sctype(s,&(arg->arg.a_sctype));
	case TGF_INT16:
		return tgf_read_int16(s,&(arg->arg.a_int16));
	case TGF_LAZY_DATA:
		return tgf_read_string(s,&arg->arg.a_string);
	default:
		return E_TGF_INVTYPE;
	}
}


/* returns number of arguments readen */
static INLINE
int	__tgf_read_command(tgf_stream_in *s,tgf_command *cmd)
{
	int i,rv;
	s->last_err = 0;
	for (i=0;i<cmd->arg_cnt;i++) {
		rv = tgf_read_argument(s,cmd->arguments+i);
		if (rv<0)
			break;
	}
	if (s->tgf_hdr.checksum)
		tgf_read_byte(s);
	s->cmd_index++;
	if (s->last_err<0)
		return s->last_err;
	if (s->crc && s->tgf_hdr.checksum)
		return s->last_err = E_TGF_INVCRC;
	return i;
}

int	tgf_read_command(tgf_stream_in *s,tgf_command **_cmd)
{
	int rv;
	tgf_command *cmd;
	tgf_argument *args;

	rv = tgf_read_command_hdr(s,0);
	if (rv<0)
		return rv;
	cmd = tgf_allocator_alloc(s->alloc,sizeof(tgf_command));
	if (!cmd)
		return E_TGF_NOMEM;
	args = tgf_allocator_alloc(s->alloc,sizeof(tgf_argument)*s->cmd.arg_cnt);
	if (!args) {
		tgf_allocator_free_all(s->alloc);
		return E_TGF_NOMEM;
	}
	cmd->type = s->cmd.cmd_type;
	cmd->arg_cnt = s->cmd.arg_cnt;
	cmd->arguments = args;
	__tgf_read_command(s,cmd);
	rv = s->last_err;
	if (rv<0 || (rv = tgf_command_check(cmd))<0) {
		tgf_allocator_free_all(s->alloc);
		return rv;
	}
	*_cmd = cmd;
	return 0;
}

/* returns 1 if reached end of stream */
int	tgf_stream_in_process_command(tgf_stream_in *s,tgf_in_cb cb,void *extra)
{
	tgf_command *cmd;
	int rv;
	void *cl_id;

	rv = tgf_read_command(s,&cmd);

	if (rv<0 || (rv = tgf_command_translate_in(cmd,s->hash))<0)
		goto out;

	if (cb(cmd,&cl_id,extra))
		return s->last_err = E_TGF_CLNT;
	if (cl_id && !tgf_map_in_add(s->hash,s->cmd_index-1,cl_id))
		return s->last_err = E_TGF_CLNT;
	rv = 0;
	if (cmd->type == TGF_ENDOFSTREAM)
		rv = 1;

out:
	tgf_allocator_free_all(s->alloc);
	return s->last_err = rv;
}

int	tgf_stream_in_process_all(tgf_stream_in *s,tgf_in_cb cb,void *extra)
{
	int rv;
	while (!(rv = tgf_stream_in_process_command(s,cb,extra)));
	return rv;
}
