/*
 *  SCPx processing module project.
 *
 *  tgfout.c - functions to write TGF
 * 
 *  Created at Mon 07 May 2001 10:32:23 PM EEST by ALK
 *
 *  Deflate (de)compression support
 *    at Thu Jun 12 21:59:18 2003 by ALK
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tgfout.h"
#include "tgfmap.h"

#include "bytesex_p.h"
#include "tgfdebug_p.h"

#include "tgfutil_p.h"

#define	TGF_WRITE_BUFSIZE 65536

#ifdef USE_ZLIB
#include <zlib.h>

struct	_tgf_write_stream {
	tgf_io_fn write_fn;
	void *fd;
	char *buffer;
	char *text,*text_end;
};

typedef struct _tgf_z_write_stream {
	struct _tgf_write_stream s;
	z_stream z;
} tgf_z_write_stream;
#endif

/* start of this struct must match _tgf_write_stream */
struct	_tgf_stream_out {
	tgf_io_fn write_fn;
	void *fd;
	char *buffer;
	char *text,*text_end;

	int compression;
	int compression_started;
	int crc;
	int cmd_index;
	tgf_sint32 arg_index;
	tgf_command_header *cmd;

	int last_err;

	tgf_map_out *hash;
};

static
int	tgf_stream_out_flush(tgf_stream_out *s);
/* It assumes more writes will follow */
static
int	stream_flush(tgf_stream_out *s);

#ifdef USE_ZLIB
static
int	z_writer(tgf_z_write_stream *fd,void *buf,int len)
{
	int rv = 0;
	if (!len)
		return 0;
	if (!(fd->s.text_end - fd->s.text))
		rv = stream_flush((tgf_stream_out *)(&fd->s));
	if (rv<0)
		return -1;
	fd->z.next_out = (Bytef *)(fd->s.text);
	fd->z.avail_out = fd->s.text_end - fd->s.text;
	fd->z.next_in = buf;
	fd->z.avail_in = len;
	while ((rv = deflate(&fd->z,Z_NO_FLUSH)) == Z_OK && fd->z.avail_in) {
		fd->s.text = (char *)(fd->z.next_out);
		rv = stream_flush((tgf_stream_out *)(&fd->s));
		if (rv<0)
			return -1;
		fd->z.next_out = (Bytef *)(fd->s.text);
		fd->z.avail_out = fd->s.text_end - fd->s.text;
	}
	fd->s.text = (char *)fd->z.next_out;
	if (rv != Z_OK)
		return -1;
	return len;
}

static
int	tgf_z_finish(tgf_z_write_stream *fd)
{
	int rv = 0;
	/*
         * more writes will follow because of finishing (?)
	 * Too sad. If zlib does not have anything to finish
         *  this may erroneously fail
         */
        /* Present version of zlib will always have something to finish
           because of lazy compression */
	if (!(fd->s.text_end - fd->s.text))
		rv = stream_flush((tgf_stream_out *)(&fd->s));
	if (rv<0)
		return rv;
	fd->z.avail_in = 0;
	fd->z.next_out = (Bytef *)(fd->s.text);
	fd->z.avail_out = fd->s.text_end - fd->s.text;
	while ((rv = deflate(&fd->z,Z_FINISH)) == Z_OK) {
		fd->s.text = (char *)(fd->z.next_out);
		rv = stream_flush((tgf_stream_out *)(&fd->s));
		if (rv<0)
			return rv;
		fd->z.next_out = (Bytef *)(fd->s.text);
		fd->z.avail_out = fd->s.text_end - fd->s.text;
	}
	fd->s.text = (char *)fd->z.next_out;
	if (rv != Z_STREAM_END)
		return E_TGF_IO;
	tgf_stream_out_flush((tgf_stream_out *)(&fd->s));
	return 0;
}
#endif /* USE_ZLIB */

int	tgf_stream_out_get_error(tgf_stream_out *s)
{
	return s->last_err;
}

#if defined(COMPRESS_BY_DEFAULT) && defined(USE_ZLIB)
#define COMPRESSION_SETTING 1
#else
#define COMPRESSION_SETTING 0
#endif

tgf_stream_out *tgf_stream_out_new(void)
{
	tgf_stream_out *p = malloc(sizeof(tgf_stream_out));
	if (!p)
		return p;
	p->fd = (void *)-1;
	p->text = p->text_end = p->buffer = NULL;
	p->compression_started = 0;
	p->compression = COMPRESSION_SETTING;
	
	if (!(p->hash = tgf_map_out_new())) {
		free(p);
		return NULL;
	}
	p->write_fn = __tgf_write_fn;
	p->cmd_index = 0;
	p->last_err = 0;
	return p;
}

void	tgf_stream_out_destroy(tgf_stream_out *s)
{
	if (!s)
		return;
	if (s->hash)
		tgf_map_out_destroy(s->hash);
	if (s->buffer) {
		tgf_stream_out_flush(s);
		free(s->buffer);
	}
#	ifdef USE_ZLIB
		if (s->compression_started) {
			tgf_z_write_stream *fd = s->fd;
			if (fd->s.buffer)
				free(fd->s.buffer);
			deflateEnd(&fd->z);
			free(fd);
		}
#	endif
	free(s);
}

int	tgf_stream_out_buf(tgf_stream_out *s,char *buf,int size)
{
	if (size & 3)
		return s->last_err = E_TGF_INVAL;
	if (s->fd != (void *)-1) {
		if (s->buffer) {
			tgf_stream_out_flush(s);
			free(s->buffer);
			s->buffer = NULL;
		}
		s->fd = (void *)-1;
	}
	s->text = buf;
	s->text_end = buf + size;
	return s->last_err = 0;
}

void	tgf_stream_out_fd(tgf_stream_out *s,int fd)
{
	if (s->fd != (void *)-1) {
		tgf_stream_out_flush(s);
		if (s->buffer)
			free(s->buffer);
	}
	s->fd = (void *)fd;
	if (!(s->buffer = malloc(TGF_WRITE_BUFSIZE)))
		s->fd = (void *)-1;
	else {
		s->text = s->buffer;
		s->text_end = s->buffer + TGF_WRITE_BUFSIZE;
	}
}

void	tgf_stream_out_custom(tgf_stream_out *s,tgf_io_fn fn,void *fd)
{
	if (s->fd != (void *)-1) {
		tgf_stream_out_flush(s);
		if (s->buffer)
			free(s->buffer);
	}
	s->fd = fd;
	s->write_fn = fn;
	if (!(s->buffer = malloc(TGF_WRITE_BUFSIZE)))
		s->fd = (void *)-1;
	else {
		s->text = s->buffer;
		s->text_end = s->buffer + TGF_WRITE_BUFSIZE;
	}
}

/* Guarantee that whole buffer will be written. It's OK for writing I think */
static
int	tgf_stream_out_flush(tgf_stream_out *s)
{
	int rv;
	if (s->fd == (void *)-1)
		return 0;
	rv = s->write_fn(s->fd,s->buffer,s->text-s->buffer);
	if (rv<(s->text-s->buffer))
		rv = E_TGF_NOSPC;
	s->text = s->buffer;
	s->text_end = s->buffer + TGF_WRITE_BUFSIZE;
	return s->last_err = rv;
}

/* It assumes more writes will follow */
static
int	stream_flush(tgf_stream_out *s)
{
	int rv = tgf_stream_out_flush(s);
	if (rv<0)
		return rv;
	if (!rv)
		return E_TGF_NOSPC;
	return 0;
}

#define STREAM_FLUSH(s) \
	do { \
		int rv = stream_flush(s); \
		if (rv<0) \
			return s->last_err = rv; \
	} while(0)

/* implementation of this is very similar to some code of g_scanner in glib */
/*    ( May cause licensing issues (?)) */
static
int	tgf_write(tgf_stream_out *s,void *_buf,int _size)
{
	int written=0,ts,t;
	char *buf = _buf;
	int size = _size;
	if (!size)
		return s->last_err = 0;
	if (!buf)
		return s->last_err = E_TGF_INVAL;
	do {
		if ((ts=s->text_end-s->text)>0) {
			t = MIN(ts,size);
			memcpy(s->text,buf,t);
			s->text += t;
			buf = buf + t;
			size -= t;
			written += t;
			if (!size)
				return written;
		}
		if (s->fd == (void *)-1)
			return s->last_err = E_TGF_NOSPC;
	} while (tgf_stream_out_flush(s)>0);
	return s->last_err = E_TGF_IO;
}

static INLINE
void	tgf_allign4_write(tgf_stream_out *s)
{
	/* see comments why so in tgfin.c */
	unsigned long _t = s->text - (char *)0;
	_t = ((~(_t-1))&3);
	if (_t) {
		char *text = s->text;
		switch (_t) {
		default:
			*text++ = 0;
		case 2:
			*text++ = 0;
		case 1:
			*text++ = 0;
		}
		s->text = text;
	}
	/*assert(s->text<s->text_end); */
}

static
int	tgf_write_allign4(tgf_stream_out *s,void *_buf,int _count)
{
	char *buf=_buf;
	int count = _count;
	int rv;
	tgf_allign4_write(s);
	if (!count)
		return 0;
	do {
		if (!(s->text_end - s->text))
			STREAM_FLUSH(s);
		rv = (s->text_end - s->text)>>2;
		if (rv>count)
			rv = count;
		__memcpy_move4(s->text,buf,rv);
		count -= rv;
	} while (count);
	return 0;
}

static
int	tgf_write_byte(tgf_stream_out *s,char b)
{
	if (!(s->text_end - s->text))
		STREAM_FLUSH(s);
	*(s->text)++ = b;
	return 0;
}

static
int	tgf_write_int16(tgf_stream_out *s,short sh)
{
	if ((s->text_end - s->text)<2)
		STREAM_FLUSH(s);
	*CAST_LV(s->text,tgf_sint16 *)++ = sh;
	return 0;
}

static
int	tgf_write_int32(tgf_stream_out *s,int i)
{
	if ((s->text_end - s->text)<4)
		STREAM_FLUSH(s);
	*CAST_LV(s->text,tgf_sint32 *)++ = i;
	return 0;
}

static INLINE
int	tgf_write_sctype(tgf_stream_out *s,tgf_sc_type type)
{
	return tgf_write_byte(s,type);
}

static
int	tgf_write_string(tgf_stream_out *s,char *str)
{
	int len;
	int rv;
	if (!str)
		len = 0;
	else
		len = strlen(str);
/*	if (len>255)
		return E_TGF_2BIG;*/
	rv = tgf_write_int32(s,len);
	if (rv<0)
		return s->last_err = rv;
	return tgf_write(s,str,len);
}

static
int	tgf_write_data(tgf_stream_out *s,char *str,int size)
{
	int rv;
	if (!str && size)
		return s->last_err = E_TGF_INVAL;
	rv = tgf_write_int32(s,size);
	if (rv<0)
		return 0;
	return tgf_write(s,str,size);
}

static
int	update_crc_argument(int *crc,tgf_argument *arg)
{
	int i;
	tgf_update_crc_uint32(crc,arg->type);
	switch (arg->type) {
	case TGF_INT32:
		tgf_update_crc_uint32(crc,arg->arg.a_int32);
		break;
	case TGF_INT16:
		tgf_update_crc_byte(crc,((char *)&(arg->arg.a_int16))[0]);
		tgf_update_crc_byte(crc,((char *)&(arg->arg.a_int16))[1]);
		break;
	case TGF_SCTYPE:
		tgf_update_crc_byte(crc,arg->arg.a_sctype);
		break;
	case TGF_INT64:
		return E_TGF_OPNOTSUPP;
	case TGF_FLOAT:
		tgf_update_crc(crc,(char *)&(arg->arg.a_float),sizeof(double));
		break;
	case TGF_DATA:
		tgf_update_crc_uint32(crc,arg->data_len);
		tgf_update_crc(crc,arg->arg.a_data,arg->data_len);
		break;
	case TGF_STRING:
	case TGF_LAZY_DATA:
		if (arg->arg.a_string) {
			i = strlen(arg->arg.a_string);
			tgf_update_crc_uint32(crc,i);
			tgf_update_crc(crc,arg->arg.a_string,i);
		}
		break;
	default:
		return E_TGF_INVARGS;
	}
	return 0;
}

static
int	tgf_write_argument(tgf_stream_out *s,tgf_argument *arg)
{
	int type = arg->type;
	int rv;
	if ((unsigned)type>TGF_ARG_LAST)
		return E_TGF_INVARGS;
	tgf_allign4_write(s);
	rv = tgf_write_int32(s,type);
	if (rv<0)
		return rv;
	switch (type) {
	case TGF_INT32:
		return tgf_write_int32(s,arg->arg.a_int32);
	case TGF_SCTYPE:
		return tgf_write_sctype(s,arg->arg.a_sctype);
	case TGF_INT16:
		return tgf_write_int16(s,(short)arg->arg.a_int16);
	case TGF_INT64:
		return E_TGF_OPNOTSUPP;
	case TGF_FLOAT:
		return tgf_write_allign4(s,&(arg->arg.a_float),sizeof(double)/4);
	case TGF_STRING:
	case TGF_LAZY_DATA:
		return tgf_write_string(s,arg->arg.a_string);
	case TGF_DATA:
		return tgf_write_data(s,arg->arg.a_data,arg->data_len);
	}
	return E_TGF_INVAL;
}

int	tgf_stream_out_set_compression(tgf_stream_out *s,int compr)
{
#	ifndef USE_ZLIB
		if (compr)
			return s->last_err = E_TGF_FEATURE;
#	endif
	s->compression = compr;
	return s->last_err = 0;
}

#ifdef USE_ZLIB
/* our task here is to replace tgf_write_stream component of stream */
static
int	setup_write_compression(tgf_stream_out *s)
{
	tgf_z_write_stream *fd;

	if (s->compression != TGF_COMPRESSION_Z)
		return s->last_err = E_TGF_INVAL;
	
	fd = malloc(sizeof(tgf_z_write_stream));
	if (!fd)
		return s->last_err = E_TGF_NOMEM;
	memcpy(fd,s,sizeof(struct _tgf_write_stream));
	s->buffer = malloc(TGF_WRITE_BUFSIZE);
	if (!s->buffer) {
		free(fd);
		return s->last_err = E_TGF_NOMEM;
	}
	s->text = s->buffer;
	s->text_end = s->text + TGF_WRITE_BUFSIZE;
	s->write_fn = (tgf_io_fn)z_writer;
	s->fd = fd;

	memset(&fd->z,0,sizeof(z_stream));
	deflateInit(&fd->z,Z_BEST_COMPRESSION);
	s->compression_started = 1; /* to indicate that compression structures
					need to be freed */
	s->last_err = tgf_stream_out_flush((tgf_stream_out *)(&fd->s));

	return s->last_err;
}
#endif /* USE_ZLIB */

int	tgf_stream_out_start(tgf_stream_out *s,char *tgf_id)
{
	static tgf_header _hdr = TGF_HEADER_INITIALIZER;
	tgf_header hdr;
	int id_len = strlen(tgf_id);
	int rv;

	memcpy(&hdr,&_hdr,sizeof(tgf_header));
	hdr.compression = s->compression;
	if (!s->compression) /* 3.0 format without compression */
		hdr.minor_ver--;
	if (id_len>255)
		id_len = 255;
	hdr.id_size = id_len;
	s->cmd_index = 0;
	rv = tgf_write(s,&hdr,TGF_HEADER_SIZE);
	if (rv<0)
		return s->last_err = rv;
	rv = tgf_write(s,tgf_id,id_len);
	if (rv<0)
		return s->last_err = rv;
#	ifdef USE_ZLIB
		if (s->compression)
			rv = setup_write_compression(s);
#	endif
	return rv;
}


int	tgf_write_command_hdr(tgf_stream_out *s,tgf_command_header *cmd)
{
	int rv;
/*	printf("%d %d %d\n",cmd->cmd_type,cmd->arg_cnt,cmd->simple_crc);*/
	rv = tgf_write_allign4(s,cmd,1);
	if (rv<0)
		return s->last_err = rv;
	return 0;
}

int	tgf_write_translated_command(tgf_stream_out *s,tgf_command *cmd)
{
	int i,rv;
	int crc = 0;
	tgf_command_header hdr;
	hdr.arg_cnt = cmd->arg_cnt;
	hdr.cmd_type = cmd->type;
	tgf_update_crc(&crc,&hdr,sizeof(tgf_command_header));
	for (i=0;i<cmd->arg_cnt;i++) {
		rv = update_crc_argument(&crc,cmd->arguments+i);
		if (rv<0)
			return s->last_err = rv;
	}
	rv = tgf_write_command_hdr(s,&hdr);
	if (rv<0)
		return rv;
	for (i=0;i<cmd->arg_cnt;i++) {
		rv = tgf_write_argument(s,cmd->arguments+i);
		if (rv<0)
			return rv;
	}
	tgf_write_byte(s,(char)crc);
	return 0;
}

int	tgf_write_command(tgf_stream_out *s,tgf_command *cmd,void *cl_id)
{
	int rv;
	if (s->last_err<0)
		return s->last_err;
	if (cl_id && !tgf_map_out_add(s->hash,cl_id,s->cmd_index)) {
		return E_TGF_CLNT;
	}
	s->cmd_index++;
	rv = tgf_command_translate_out(cmd,s->hash);
	if (rv<0)
		return s->last_err = rv;
	return tgf_write_translated_command(s,cmd);
}

int	tgf_stream_out_finish(tgf_stream_out *s)
{
	tgf_command cmd = {TGF_ENDOFSTREAM,0,0};
	int rv = tgf_write_command(s,&cmd,0);
	if (rv>=0) {
		rv = tgf_stream_out_flush(s);
#		ifdef USE_ZLIB
			if (rv>=0 && s->compression)
				rv = tgf_z_finish(s->fd);
#		endif
	}
	return s->last_err = rv;
}

int	tgf_stream_out_is_written(tgf_stream_out *s,void *user_id)
{
	int dummy;
	return tgf_map_out_lookup(s->hash,user_id,&dummy);
}
