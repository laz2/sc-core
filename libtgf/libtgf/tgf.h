
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


/*
 *  SCPx processing module project.
 *
 *  tgf.h - tgf format definition and misc tgf functions
 *
 *  Created at Thu 03 May 2001 12:53:17 AM EEST by ALK
 *
 *  Temporality support was added at Mon 12 Dec 2011 by IVP (TGF 3.2)
 */
#ifndef __TGF_H__
#define __TGF_H__


#ifdef WIN32
// стандартный макрос для статической библиотеки
#	ifdef _LIB
#		define LIBTGF_API
// _USRDLL - стандартный макрос для динамической библиотеки
#	else
#		ifdef libtgf_EXPORTS
#			define LIBTGF_API __declspec(dllexport)
#		else
#			define LIBTGF_API __declspec(dllimport)
#		endif
#	endif
#else
#	define LIBTGF_API
#endif
/*
#ifdef WIN32
#	ifdef libTGF_EXPORTS
//#		define USE_ZLIB		1
#		ifdef _LIB
#			define LIBTGF_API
#		else
#			define LIBTGF_API __declspec(dllexport)
#		endif
#	else
#		define LIBTGF_API __declspec(dllimport)
#	endif
#else
#	define LIBTGF_API
#endif
*/
#ifdef WIN32
#	pragma warning (disable : 4996)
#endif


#ifdef __cplusplus
#define __TGF_BEGIN_DECL extern "C" {
#define __TGF_END_DECL }
#else
#define __TGF_BEGIN_DECL
#define __TGF_END_DECL
#endif

#include "tgfmachine.h"

__TGF_BEGIN_DECL

//#define _NOTEMPORALITY

/* TGF format version */

#define	TGF_MAJOR_VER	3
/* yet minor ver is 1, not 3 */
#ifndef _NOTEMPORALITY
#define	TGF_MINOR_VER	2
#else
#define	TGF_MINOR_VER	1
#endif//_NOTEMPORALITY
/* TGF error codes section */

/* OK. Not actually error */
#define E_TGF_OK	(0)
/* invalid type */
#define	E_TGF_INVTYPE	(-1)
/* invalid header */
#define E_TGF_INVHDR	(-2)
/* invalid crc */
#define E_TGF_INVCRC	(-3)
/* invalid command arguments */
#define	E_TGF_INVARGS	(-4)
/* unexpected EOF */
#define E_TGF_EOF	(-5)
/* unsupported version */
#define E_TGF_VER	(-6)
/* not enough space */
#define E_TGF_NOSPC	(-7)
/* operation not supported */
#define E_TGF_OPNOTSUPP	(-8)
/* I/O error */
#define E_TGF_IO	(-9)
/* not enough memory */
#define E_TGF_NOMEM	(-10)
/* invalid data passed to libTGF function */
#define E_TGF_INVAL	(-11)
/* client's side error */
#define E_TGF_CLNT	(-12)
/* internal error (i.e. BUG :) ) */
#define E_TGF_BUG	(-13)
/* TGF command failed */
#define E_TGF_CMDFAIL	(-14)
/* value too big */
#define E_TGF_2BIG	(-15)
/* unsupported feature (e.g. deflate compression) */
#define E_TGF_FEATURE	(-16)
#define E_TGF_LAST	(-16)

/* function to get error string by error code */
LIBTGF_API const char *tgf_error(int err);


/* TGF format definition */

typedef unsigned char tgf_sc_type;

/**
 * @name Syntactic types of tgf-element.
 * @{
 */
#define TGF_UNDF            0x30 /* 0x01 0000 0001 */ /* 0x30 0011 0000 */
#define TGF_NODE            0x20 /* 0x02 0000 0010 */ /* 0x20 0010 0000 */
#define TGF_EDGE            0x00 /* 0x03 0000 0011 */ /* 0x00 0000 0000 */
#define TGF_ARC             0x10 /* 0x04 0000 0100 */ /* 0x10 0001 0000 */
#define TGF_ARC_ACCESSORY   0x14 /* 0x05 0000 0101 */ /* 0x14 0001 0100 */
#define TGF_LINK            0x24 /* 0x06 0000 0110 */ /* 0x30 0011 0000 */

#define TGF_SYNTACTIC_MASK  (TGF_UNDF|TGF_NODE|TGF_EDGE|TGF_ARC|TGF_ARC_ACCESSORY|TGF_LINK)
#define TGF_SYNTACTIC_SHIFT 2
/** @} */

/**
 * @name Constancy types of tgf-element.
 * @{
 */
#define TGF_CONST           0x01 /* 0x08 0000 1000 */ /* 0x01 0000 0001 */
#define TGF_VAR             0x02 /* 0x10 0001 0000 */ /* 0x02 0000 0010 */
#define TGF_METAVAR         0x03 /* 0x18 0001 1000 */ /* 0x03 0000 0011 */

#define TGF_CONSTANCY_MASK  (TGF_CONST|TGF_VAR|TGF_METAVAR)
#define TGF_CONSTANCY_SHIFT 0
/** @} */

/**
 * @name Fuzzyness types of tgf-arc accessory.
 * @{
 */
#define TGF_POS             0x04 /* 0x20 0010 0000 */ /* 0x14 0000 0100 */
#define TGF_NEG             0x08 /* 0x40 0100 0000 */ /* 0x18 0000 1000 */
#define TGF_FUZ		    0x0C /* 0x60	   */ /* 0x1C 0000 1100 */

#define TGF_FUZZYNESS_MASK  (TGF_POS|TGF_NEG|TGF_FUZ)
#define TGF_FUZZYNESS_SHIFT 2
/** @} */

/**
 * @name Permanency types of tgf-arc accessory.
 * @{
 */
#ifndef _NOTEMPORALITY


#define TGF_TEMPORARY       0x00 /* 0x80 1000 0000 */ /* 0x10 0001 0000 */
#define TGF_PERMANENT       0x80		      /* 0x90 1001 0000 */
#else


#define TGF_TEMPORARY       0x00 /* 0x80 1000 0000 */ /* 0x10 0001 0000 */
#define TGF_PERMANENT       0x00		      /* 0x90 1001 0000 */
#endif//_NOTEMPORALITY
#define TGF_PERMANENCY_MASK  (TGF_TEMPORARY|TGF_PERMANENT)
#define TGF_PERMANENCY_SHIFT 7


/** @} */

/**
 * @name Structure types of tgf-node.
 * @{
 */
#define TGF_TUPLE        0x20 /* 0010 0000 */
#define TGF_STRUCT       0x40 /* 0100 0000 */
#define TGF_ROLE         0x60 /* 0110 0000 */
#define TGF_RELATION     0x80 /* 1000 0000 */
#define TGF_CONCEPT      0xA0 /* 1010 0000 */
#define TGF_ABSTRACT     0xE0 /* 1100 0000 */

#define TGF_STRUCT_MASK  (TGF_TUPLE|TGF_STRUCT|TGF_ROLE|TGF_RELATION|TGF_CONCEPT|TGF_ABSTRACT)
#define TGF_STRUCT_SHIFT 5
/** @} */

enum tgf_arg_type {
	TGF_INT32=0,
	TGF_INT64=1,
	TGF_FLOAT=2,
	TGF_DATA=3,
	TGF_SCTYPE=4,
	TGF_INT16=5,
	TGF_STRING=6,
	TGF_LAZY_DATA=7,
	TGF_ARG_LAST=8,
	TGF_USERID=254, /* for clients communication only */
	TGF_NONE=255 /* for empty tgf_cont */
};

enum tgf_cmd_type {
	TGF_NOP,
	TGF_GENEL,
	TGF_DECLARE_SEGMENT,
	TGF_SWITCH_TO_SEGMENT,
	TGF_SETBEG,
	TGF_SETEND,
	TGF_FINDBYIDTF,
	TGF_ENDOFSTREAM,
	TGF_CMD_LAST=TGF_ENDOFSTREAM
};

enum tgf_checksumming_mode {
	TGF_CHECKSUM_NONE,
	TGF_CHECKSUM_XOR
};

enum tgf_compression_mode {
	TGF_COMPRESSION_NONE,
	TGF_COMPRESSION_Z
};

typedef struct _tgf_header tgf_header;
typedef struct _tgf_header_user tgf_header_user;
typedef struct _tgf_command_header tgf_command_header;
typedef struct _tgf_argument_user tgf_argument;
typedef struct _tgf_command_user tgf_command;

struct _tgf_header {
	char	signature[3];			/* 0 */
	tgf_uchar	minor_ver;		/* 3 */
	tgf_uchar	major_ver;		/* 4 */
	tgf_uchar	endianness;		/* 5 */
	tgf_uint16	compression;		/* 6 */
	tgf_uchar	res1;			/* 8 */
	tgf_uchar	checksum;		/* 9 */
	tgf_uchar	_res2[2];		/* 10 */
	tgf_uchar	id_size;		/* 12 */
};	/* total size is 16 because of allignment of size */
/* size of the header in stream */
#define TGF_HEADER_SIZE	13

/* more convenient for user */
struct _tgf_header_user {
	char	signature[3];			/* 0 */
	tgf_uchar	minor_ver;		/* 3 */
	tgf_uchar	major_ver;		/* 4 */
	tgf_uchar	endianness;		/* 5 */
	tgf_uint16	compression;		/* 6 */
	tgf_uchar	_res1;			/* 8 */
	tgf_uchar	checksum;		/* 9 */
	tgf_uchar	_res2[2];		/* 10 */
	tgf_uchar	id_size;		/* 12 */
	char *	tgf_id;
};

#define TGF_HEADER_INITIALIZER \
	{ \
		{'T','G','F'},	/* signature */ \
		TGF_MINOR_VER,	/* minor_ver */ \
		TGF_MAJOR_VER,	/* major_ver */ \
		TGF_HOST_ENDIANNESS, /* endianness */ \
		0,		/* compression */ \
		0,		/* res1 */ \
		TGF_CHECKSUM_XOR, /* XOR checksumming */ \
		{0,0},					\
		0		/* id_size */ \
	}

struct _tgf_command_header {
	tgf_uint16	cmd_type;	/* 0 */
	tgf_uint16	arg_cnt;	/* 2 */
};	/* 4 */

struct _tgf_argument_user {
	enum tgf_arg_type type;
	int data_len; /* for type == TGF_DATA */
	/*int alloced; *//* !0 if char * fields was alloced by malloc */
	union {
		tgf_sint32 a_int32;
		tgf_sint32 a_int16;
		tgf_sc_type a_sctype;
		char *a_data;
		char *a_string; /* used for string & lazy_data argument types */
		void *a_userid;
		double a_float;
	} arg;
};


struct _tgf_command_user {
	enum tgf_cmd_type type;
	int arg_cnt;
	tgf_argument *arguments;
};


/* types used in libTGF */

/* this type is used in tgf{in,out}.h */
/* this functions return number of bytes actually processed */
/* this functions returns 0 in case of EOF */
/* When read returns number less than requested it doesn't mean EOF reached ! */
/* If write returns less than requested it MEANS error ocurred */
/* It is good to implement __tgf_{read,write}_fn for every platform we need */
/*   See tgfio_unix.c for example */
typedef int (*tgf_io_fn) (void *fd,char *buf,int size);


__TGF_END_DECL

#ifdef	libtgf_EXPORTS
#include "tgfmap.h"
#include "tgfalloc.h"

/* frees memory associated with argument. Not argument itself ! */
/*void	tgf_argument_free(tgf_allocator *,tgf_argument *);
void	tgf_command_free(tgf_allocator *,tgf_command *);*/

/* If your favourite C compiler other than GCC and not C99 compliant
   supports `inline' correct the following */
/* not sure this is correct */
/*#if	defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#	define HAVE_INLINE
#	define INLINE __inline__*/
#if	defined(__GNUC__)
#	define INLINE __inline__
#	define HAVE_INLINE
#else
#	define INLINE
#endif

int	tgf_command_check(tgf_command *);
int	tgf_command_translate_in(tgf_command *,tgf_map_in *);
int	tgf_command_translate_out(tgf_command *,tgf_map_out *);

int	__tgf_read_fn(void *fd,char *buf,int size);
int	__tgf_write_fn(void *fd,char *,int size);

#define	MAX(a,b) ((a)<(b)?(b):(a))
#define MIN(a,b) ((a)>(b)?(b):(a))

/* this function cannot touch *crc during processing ! */
static INLINE
void	tgf_update_crc(int *crc,const void *buf,int size)
{
	const unsigned char *p=(const unsigned char *)buf;
	int i;
	unsigned char _crc=*crc; /* see comment before */
	for (i=size;i>0;i--)
		_crc ^= *p++;
	*crc = _crc;
}

static INLINE
void	tgf_update_crc_byte(int *crc,int b)
{
	*crc ^= (unsigned char)b;
}

static INLINE
void	tgf_update_crc_uint32(int *_crc,tgf_uint32 i)
{
	register unsigned char *p = (unsigned char *)&i;
	unsigned char crc = *_crc;
	/* order is not important here */
	crc ^= p[0];
	crc ^= p[1];
	crc ^= p[2];
	crc ^= p[3];
	*_crc = crc;
}

#endif /* libTGF_EXPORTS */

#endif /*__TGF_H__*/
