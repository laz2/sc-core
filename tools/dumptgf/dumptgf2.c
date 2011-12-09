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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgfin.h>

#ifndef _MSC_VER 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#endif

#include "eret.h"

static	int counter=0;
static	char *idtfs[16384*64];

int	libtgf_callback(tgf_command *cmd,void **cl_id,void *extra)
{
	static char *eclass_names[] = {"Invalid","ARC","NODE","UNDF"};
	if (cmd->type == TGF_GENEL) {
		int t = cmd->arguments[1].arg.a_sctype;
		char *idtf = cmd->arguments[0].arg.a_string;
		char *conttypes[] = {
			"int32",
			"int64",
			"float",
			"data",
			"sctype",
			"int16",
			"string"
		};
		char *conttype = cmd->arg_cnt < 3 ? "nothing" : conttypes[cmd->arguments[2].type];
		printf("Command N%d to generate %s with idtf=\"%s\" and type=0x%x (content type %s)\n",
			counter,
			eclass_names[(t&TGF_ELMNCLASS_MASK)>>TGF_ELMNCLASS_SHIFT],
			idtf,
			t,
			conttype
		);
		if (!*idtf) {
			idtf = malloc(16);
			if (!idtf)
				return 2;
			strcpy(idtf,"__N");
			sprintf(idtf+3,"%d",counter);
		} else
			idtf = strdup(idtf);
		idtfs[counter] = idtf;
	} else if (cmd->type == TGF_DECLARE_SEGMENT) {
		printf("Command N%d to declare segment %s\n",counter,
			cmd->arguments[0].arg.a_string);
		idtfs[counter] = strdup(cmd->arguments[0].arg.a_string);
	} else if (cmd->type == TGF_SWITCH_TO_SEGMENT) {
		if (cmd->arguments[0].arg.a_userid == (void *)(-1))
			printf("Command N%d to switch to free segment\n",counter);
		else
			printf("Command N%d to switch to segment %s\n",counter,
				(char *)(cmd->arguments[0].arg.a_userid));
	} else if (cmd->type == TGF_SETBEG || cmd->type == TGF_SETEND) {
		printf("Command N%d to set %s of arc %s to %s\n",
			counter,
			cmd->type == TGF_SETBEG?"begin":"end",
			(char *)(cmd->arguments[0].arg.a_userid),
			(char *)(cmd->arguments[1].arg.a_userid)
		);
	} else if (cmd->type == TGF_FINDBYIDTF) {
		printf("Command N%d to find element by idtf %s\n",
			counter,
			cmd->arguments[0].arg.a_string
		);
	} else if (cmd->type == TGF_ENDOFSTREAM) {
		printf("Command N%d - end of stream\n",counter);
	} else if (cmd->type == TGF_NOP) {
		printf("Command N%d - NOP\n",counter);
	} else
		return 1;
	*cl_id = idtfs[counter++];
	return 0;
}

#define ERET(string) ERET_BASE("dumptgf2", string)

int	main(int args,char **argv)
{
	FILE *inp = fopen(argv[1],"rb");
	int fd;
	tgf_stream_in *s = 0;
	char ch;
	int state = 0;

	if (!inp)
		ERET("failed to open input file")

	fd = fileno(inp);

	read(fd,&ch,1);
	do {
		read(fd,&ch,1);
		switch (state) {
		case 0:
			if (ch == 'T')
				state = 1;
			break;
		case 1:
			if (ch == 'G')
				state = 2;
			else
				state = 0;
			break;
		case 2:
			if (ch == 'F')
				goto __out;
		default:
			abort();
		}
	} while (1);
__out:

	lseek(fd,-3,SEEK_CUR);

	s = tgf_stream_in_new();
	if (!s)
		ERET("failed to create input stream")

	tgf_stream_in_fd(s,fd);

	if (tgf_stream_in_start(s,0)<0)
		ERET("tgf_stream_in_start");

	if (tgf_stream_in_process_all(s,libtgf_callback,0)<0)
		ERET("tgf_stream_in_process_all");

	return 0;
}
