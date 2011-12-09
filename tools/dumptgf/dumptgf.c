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
#include <libtgf/tgfin.h>

#include "eret.h"

#ifndef _MSC_VER 
#include <unistd.h>
#endif

static	int counter=0;
static	char *idtfs[65536*16];



const char *type2string(tgf_sc_type type) {
	static char s[100];
	s[0]='\0';

	#define ADD_CONST_TYPE \
		switch (type & TGF_CONSTANCY_MASK) { \
		case TGF_CONST: \
			strcat(s, "|const"); \
			break; \
		case TGF_VAR: \
			strcat(s, "|var"); \
			break; \
		case TGF_METAVAR: \
			strcat(s, "|metavar"); \
			break; \
		default: \
			return "invalid-constancy"; \
		}

	switch (type & TGF_SYNTACTIC_MASK) {
	case TGF_UNDF:
		strcat(s, "undf");
		ADD_CONST_TYPE
		break;
	case TGF_NODE:
		strcat(s, "node");

		ADD_CONST_TYPE

		switch (type & TGF_STRUCT_MASK) {
		case TGF_TUPLE:
			strcat(s, "|tuple");
			break;
		case TGF_STRUCT:
			strcat(s, "|struct");
			break;
		case TGF_ROLE:
			strcat(s, "|role");
			break;
		case TGF_RELATION:
			strcat(s, "|relation");
			break;
		case TGF_CONCEPT:
			strcat(s, "|concept");
			break;
		case TGF_ABSTRACT:
			strcat(s, "|abstract");
			break;
		default:
			break;
		}

		break;
	case TGF_EDGE_COMMON:
		strcat(s, "edge-common");
		ADD_CONST_TYPE
		break;
	case TGF_ARC_COMMON:
		strcat(s, "arc-common");
		ADD_CONST_TYPE
		break;
	case TGF_ARC_ACCESSORY:
		if ((type & TGF_CONSTANCY_MASK) == TGF_CONST
				&& (type & TGF_FUZZYNESS_MASK) == TGF_POS
					&& !(type & TGF_PERMANENCY_MASK)) {
			strcat(s, "arc-main");
			break;
		}

		strcat(s, "arc|accessory");
		ADD_CONST_TYPE

		switch (type & TGF_FUZZYNESS_MASK) {
		case TGF_POS:
			strcat(s, "|pos");
			break;
		case TGF_NEG:
			strcat(s, "|neg");
			break;
		default:
			strcat(s, "|fuz");
		}

		switch (type & TGF_PERMANENCY_MASK) {
		case TGF_TEMPORARY:
			strcat(s, "|temp");
			break;
		default:
			strcat(s, "|perm");
		}

		break;
	case TGF_LINK:
		strcat(s, "|link");
	default:
		return "invalid-syntactic";
	}

	return s;
}

int	libtgf_callback(tgf_command *cmd,void **cl_id,void *extra)
{
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

		printf("N%d: GenEl( type=%s, idtf=\"%s\", content_type=%s )\n",
			counter,
			type2string(t),
			idtf,
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
		printf("N%d: DeclareSegment( %s )\n",counter,
			cmd->arguments[0].arg.a_string);
		idtfs[counter] = strdup(cmd->arguments[0].arg.a_string);
	} else if (cmd->type == TGF_SWITCH_TO_SEGMENT) {
		if (cmd->arguments[0].arg.a_userid == (void *)(-1))
			printf("N%d: SwitchToFreeSegment( )\n",counter);
		else
			printf("N%d: SwitchToSegment( %s )\n",counter,
				(char *)(cmd->arguments[0].arg.a_userid));
	} else if (cmd->type == TGF_SETBEG || cmd->type == TGF_SETEND) {
		printf("N%d: Set%s( %s, %s )\n",
			counter,
			cmd->type == TGF_SETBEG?"Beg":"End",
			(char *)(cmd->arguments[0].arg.a_userid),
			(char *)(cmd->arguments[1].arg.a_userid)
		);
	} else if (cmd->type == TGF_FINDBYIDTF) {
		printf("N%d: FindElByIdtf( %s )\n",
			counter,
			cmd->arguments[0].arg.a_string
		);
	} else if (cmd->type == TGF_ENDOFSTREAM) {
		printf("N%d: EndOfStream( )\n",counter);
	} else if (cmd->type == TGF_NOP) {
		printf("N%d: NOP( )\n",counter);
	} else
		return 1;
	*cl_id = idtfs[counter++];
	return 0;
}

#define ERET(string) ERET_BASE("dumptgf", string)

int	main(int args,char **argv)
{
	FILE *inp = fopen(argv[1],"rb");
	int fd;
	tgf_stream_in *s = 0;

	if (!inp)
		ERET("failed to open input file")

	fd = fileno(inp);

	s = tgf_stream_in_new();
	if (!s)
		ERET("failed to create input stream")

	tgf_stream_in_fd(s,fd);

	if (tgf_stream_in_start(s,0)<0)
		ERET("tgf_stream_in_start");

	if (tgf_stream_in_process_all(s,libtgf_callback,0)<0)
		ERET("tgf_stream_in_process_all");

	tgf_stream_in_destroy(s);

	return 0;
}
