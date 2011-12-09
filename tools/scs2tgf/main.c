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

/**
 * @file main.c
 * @brief main converter file.
 * @author ALK
 */

#include "parser.h"
#include "sctext.h"
#include "values.h"

#include <libtgf/tgfout.h>
#include <libtgf/tgfmachine.h>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32

#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#define O_BINARY 0

#else
/*#error "Write this"*/
#include <io.h>
#include <fcntl.h>
#define open _open

#endif

#include "parser.tab.h"


int do_not_complain_constancy;

char *include_paths[256];
int last_include;

/*
 * We have defined sc_type according to tgf_sc_type
 */
static
tgf_sc_type get_tgf_type(sc_type type)
{
	int t = type & SC_CONSTANCY_MASK;
	int output = 0;
	switch (t) {
	case SC_CONST:
		output |= TGF_CONST;
		break;
	case SC_VAR:
		output |= TGF_VAR;
		break;
	case SC_METAVAR:
		output |= TGF_METAVAR;
		break;
	default:
	__int_bug:
		fprintf(stderr,"Internal bug: found element with invalid sc_type\n");
		abort();
	}
	t = type & SC_FUZZYNESS_MASK;
	switch (t) {
	case SC_POS:
		output |= TGF_POS;
		break;
	case SC_NEG:
		output |= TGF_NEG;
		break;
	case SC_FUZ:
		break;
	default:
		if (t)
			goto __int_bug;
	}
	t = type & SC_GENERIC_TYPE_MASK;
	switch (t) {
	case SC_NODE:
		output |= TGF_NODE;
		break;
	case SC_ARC:
		output |= TGF_ARC_ACCESSORY;
		break;
	case SC_UNDF:
		output |= TGF_UNDF;
		break;
	default:
		goto __int_bug;
	}
	return output;
}

static
void *	get_tgf_ref(int ref)
{
	while (sctext[ref].alias >= 0)
		ref = sctext[ref].alias;
	return TGF_I2P(ref+1);
}

#ifdef	SCS2TGF_UNIX
#include <sys/stat.h>
#include <unistd.h>
#elif WIN32
#include <sys/stat.h>
#include <io.h>
#else
#error
#endif


static
int	get_file_size(char *filename)
{
	struct stat st;
	if (stat(filename,&st))
		return -1;
	return st.st_size;
}

static
int	read_cont_file(char *filename,char *buf,int size)
{
	FILE *inp = fopen(filename,"rb");
	if (!inp)
		return -1;
	if (!size)
		return 0;
	if (fread(buf,size,1,inp)<1) {
		fclose(inp);
		return -1;
	}
	fclose(inp);
	return 0;
}

static
char *	segments[8192];
static
int	segments_cnt = 0;

/* simple linear time algorithm */
static
int	register_segment(const char *seg,int seglen)
{
	int i;
	char *ptr;
	for (i=0;i<segments_cnt;i++) {
		if (strlen(segments[i]) == seglen && !strncmp(segments[i],seg,seglen))
			return i;
	}
	ptr = segments[segments_cnt++] = malloc(seglen+1);
	memcpy(ptr,seg,seglen);
	ptr[seglen] = 0;
	return i;
}

static
int	get_seglen(const char *seg)
{
	int pos=-1;
	int i;
	for (i=0;seg[i] != 0;i++)
		if (seg[i] == '/')
			pos = i;
	if (pos<0) {
		fprintf(stderr,"Internal error: link have incorrect pathname\n");
		abort();
	}
	if (pos+1 == i) {
		fprintf(stderr,"Trying to reference sign of segment ?(%s)\n",
				seg);
		abort();
	}
	return pos;
}

static
int	write_links(tgf_stream_out *s)
{
	int i,j,rv;
	tgf_command cmd;
	tgf_argument args[2];

	cmd.arguments = args;
	/* phase 1. collect segments */
	for (i=0;i<sctext_size;i++) {
		char *ptr;
		int len,pos;
		sctext[i].segment = -1;
		if (sctext[i].alias >= 0)
			continue;
		if (!sctext[i].id || sctext[i].id[0] != '/')
			continue;
		/* put `in segment' idtf instead of full uri */
		sctext[i].segment = register_segment(sctext[i].id,
					pos = get_seglen(sctext[i].id));
		pos++;
		len = strlen(sctext[i].id+pos);
		ptr = malloc(len+1);
		memcpy(ptr,sctext[i].id+pos,len);
		ptr[len] = 0;
		free(sctext[i].id);
		sctext[i].id = ptr;
	}
	/* phase 2. write segments one by one */
	for (j=0;j<segments_cnt;j++) {
		cmd.type = TGF_DECLARE_SEGMENT;
		cmd.arg_cnt = 1;
		args[0].type = TGF_STRING;
		args[0].arg.a_string = segments[j];
		rv = tgf_write_command(s,&cmd,TGF_I2P(j+SCTEXT_MAXSIZE+1));
		if (rv<0)
			return rv;
		cmd.type = TGF_GENEL;
		cmd.arg_cnt = 2;
		args[0].type = TGF_STRING;
		args[1].type = TGF_SCTYPE;
		for (i=0;i<sctext_size;i++) {
			if (sctext[i].segment<0 || sctext[i].segment != j || sctext[i].hidden)
				continue;
			if (sctext[i].cont_type) {
				fprintf(stderr,"Multiple segments are not fully supported yet."
					" So you cannot place content to elements of other segments, yet \n");
				fprintf(stderr,"idtf is %s\n",sctext[i].id);
				abort();
			}
			args[0].arg.a_string = sctext[i].id;
			args[1].arg.a_sctype = get_tgf_type(sctext[i].type);
			rv = tgf_write_command(s,&cmd,TGF_I2P(i+1));
			if (rv<0)
				return rv;
		}
	}
	return 0;
}

static
int	write_tgf(int fd_out)
{
	tgf_stream_out *s;
	int i,rv;
	tgf_argument args[3];
	tgf_command cmd;

	cmd.arguments = args;

	s = tgf_stream_out_new();
	if (!s) {
		fprintf(stderr,"Internal error: cannot create tgf stream\n");
		return 7;
	}
	tgf_stream_out_fd(s,fd_out);
	tgf_stream_out_start(s,"parsed");

	/* phase 0.5 remove $ identifiers */
	for (i=0;i<sctext_size;i++) {
		if (!sctext[i].id || sctext[i].id[0] == '/' || sctext[i].id[0] != '$')
			continue;
		free(sctext[i].id);
		sctext[i].id = 0;
	}

	/* phase 1 write elements */
	rv = 0;
	cmd.type = TGF_GENEL;
	args[0].type = TGF_STRING;
	args[1].type = TGF_SCTYPE;
	for (i=0;i<sctext_size && rv>=0 ;i++) {
		int release_flag = 0;
		char *file_cont = 0;
		int file_size;

		if (sctext[i].alias >= 0 || sctext[i].hidden)
			continue;
		if (sctext[i].id && sctext[i].id[0] == '/')
			continue;
		cmd.arg_cnt = 2;
		args[0].arg.a_string = sctext[i].id;
		args[1].arg.a_sctype = get_tgf_type(sctext[i].type);
		if (sctext[i].cont_type) {
			switch (sctext[i].cont_type) {
			case CONT_STRING:
				args[2].arg.a_string = sctext[i].str;
				args[2].type = TGF_STRING;
				cmd.arg_cnt = 3;
				break;
			case CONT_NUMERIC:
				args[2].arg.a_float = sctext[i].real;
				args[2].type = TGF_FLOAT;
				cmd.arg_cnt = 3;
				break;
			case CONT_DATA:
				args[2].arg.a_data = sctext[i].str;
				args[2].data_len = sctext[i].datalen;
				args[2].type = TGF_DATA;
				cmd.arg_cnt = 3;
				break;
			case CONT_FILE:
				file_size = get_file_size(sctext[i].str);
				if (file_size<0) {
					printf("File %s doesn't exist\n",sctext[i].str);
					return 100;
				}
				file_cont = malloc(file_size);
				if (!file_cont)
					abort();
				release_flag = 1;
				if (read_cont_file(sctext[i].str,file_cont,file_size)) {
					printf("Cannot read file %s\n",sctext[i].str);
					return 101;
				}
				args[2].arg.a_data = file_cont;
				args[2].data_len = file_size;
				args[2].type = TGF_DATA;
				cmd.arg_cnt = 3;
			}
		}
		if (!s)
			abort();
		rv = tgf_write_command(s,&cmd,TGF_I2P(i+1));
		if (release_flag)
			free(file_cont);
	}
	/* phase 1.5 write "links" */
	if (rv<0 || (rv = write_links(s))) {
		fprintf(stderr,"Internal error: tgf error: %s\n",tgf_error(rv));
		return 8;
	}
	/* phase 2 write connections */
	cmd.arg_cnt = 2;
	args[0].type = TGF_USERID;
	args[1].type = TGF_USERID;
	rv = 0;
	for (i=0;i<sctext_size && rv >= 0;i++) {
		if (sctext[i].alias >= 0 || sctext[i].hidden)
			continue;
		if (!(sctext[i].type & SC_ARC))
			continue;
		if (sctext[i].beg >= 0) {
			cmd.type = TGF_SETBEG;
			args[0].type = TGF_USERID;
			args[1].type = TGF_USERID;
			args[0].arg.a_userid = TGF_I2P(i+1);
			args[1].arg.a_userid = get_tgf_ref(sctext[i].beg);
			rv = tgf_write_command(s,&cmd,0);
			if (rv<0)
				break;
		}
		if (sctext[i].end >= 0) {
			cmd.type = TGF_SETEND;
			args[0].type = TGF_USERID;
			args[1].type = TGF_USERID;
			args[0].arg.a_userid = TGF_I2P(i+1);
			args[1].arg.a_userid = get_tgf_ref(sctext[i].end);
			rv = tgf_write_command(s,&cmd,0);
		}
	}
	if (rv<0) {
		fprintf(stderr,"Internal error: tgf error: %s\n",tgf_error(rv));
		return 8;
	}
	rv = tgf_stream_out_finish(s);
	if (rv<0) {
		fprintf(stderr,"Internal error: tgf error: %s\n",tgf_error(rv));
		return 8;
	}
	tgf_stream_out_destroy(s);
	return 0;
}

/*#define __TOSTR2(a) #a

#define __TOSTR(a) __TOSTR2(a)

#define VERSION_STRING __TOSTR(VERSION)*/

int	main(int argc,char **argv)
{
	int rv;
	int fd_out;
	FILE *input;
	static char namebuf[4096];
	char *name2;
	extern YYLTYPE yylloc;

/* little debugging hack */
#	if 0
		extern int yydebug;
		yydebug = 1;
#	endif
	yylloc.last_line = 1;

	printf("scs2tgf converter version 0.9.18\n");

	if (argc<2) {
		fprintf(stderr,"Should give me at least name!\n");
		return 1;
	}
more_opts:
	if (!strcmp(argv[1],"--no-constancy-complain") || !strcmp(argv[1],"-nc")) {
		argc--;
		argv++;
		do_not_complain_constancy = 1;
	}
	if (!strncmp(argv[1],"-I",2)) {
		if (last_include == sizeof(include_paths)/sizeof(*include_paths)-1) {
			fprintf(stderr,"Max includes length approached\n");
			exit(2);
		}
		if (strlen(argv[1]+2) < 1) {
			fprintf(stderr,"Empty include path\n");
			exit(2);
		}
		include_paths[last_include++] = argv[1]+2;
		argc--;
		argv++;
		goto more_opts;
	}

	if (argv[1][0] == '-' && argv[1][1] == 0) {
		input = stdin;
		cfilename = strdup("(stdin)");
	} else {
		input = fopen(argv[1],"rt");
		if (!input) {
			perror("cannot open input file");
			return 2;
		}
		cfilename = strdup(argv[1]);
	}
	if (argc<3) {
		if (input == stdin) {
			fprintf(stderr,"Supply output file name\n");
			return 3;
		}
		strcpy(namebuf,argv[1]);
		strcat(namebuf,".tgf");
		name2 = namebuf;
	} else
		name2 = argv[2];
	fd_out = open(name2,O_WRONLY|O_BINARY|O_CREAT|O_TRUNC,0644);
	if (fd_out<0) {
		perror("Failed to open output file");
		return 2;
	}

	if (init_idmap()) {
		fprintf(stderr,"Internal error: init_idmap failed\n");
		close(fd_out);
		remove(name2);
		return 4;
	}
	init_special_nodes();

	yyin = input;
	rv = yyparse() || handle_special_nodes() || resolve_nonarcs();
	if (rv) {
		fprintf(stderr,"parse error occured\n");
		close(fd_out);
		remove(name2);
		return 5;
	}
	set_text_defaults();

	rv = write_tgf(fd_out);
	if (rv) {
		close(fd_out);
		remove(name2);
		return rv;
	}

	close(fd_out);
	fclose(input);

	return rv;
}
