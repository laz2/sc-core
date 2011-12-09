/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <tclap/CmdLine.h>

#include <libtgf/tgfin.h>

TCLAP::UnlabeledValueArg<std::string> arg_input_file("input-file", "Input tgf file", true, "", "input file");
TCLAP::ValueArg<std::string> arg_output_file("o", "output", "Generated output file", false, "", "path to file");
TCLAP::SwitchArg arg_raw_mode("r", "raw", "Turn on raw tgf output", true);

namespace fs = boost::filesystem;

#ifndef _MSC_VER 
#include <unistd.h>
#endif

static int counter=0;
static char *idtfs[65536*16];

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
	case TGF_EDGE:
		strcat(s, "edge");
		ADD_CONST_TYPE
			break;
	case TGF_ARC:
		strcat(s, "arc");
		ADD_CONST_TYPE
			break;
	case TGF_ARC_ACCESSORY:
		if ((type & TGF_CONSTANCY_MASK) == TGF_CONST
				&& (type & TGF_FUZZYNESS_MASK) == TGF_POS
					&& !(type & TGF_PERMANENCY_MASK)) {
				strcat(s, "arc-main");
				break;
		}

		strcat(s, "arc-accessory");
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
		const char *conttype = cmd->arg_cnt < 3 ? "nothing" : conttypes[cmd->arguments[2].type];

		std::cout << boost::format("N%1%: GenEl( type=%2%, idtf=\"%3%\", content_type=%4%")
			% counter % type2string(t) % idtf % conttype;

		if (cmd->arguments[2].type != TGF_DATA && cmd->arguments[2].type != TGF_LAZY_DATA) {
			std::cout << "(";

			switch (cmd->arguments[2].type) {
			case TGF_INT32:
				std::cout << cmd->arguments[2].arg.a_int32;
				break;
			case TGF_FLOAT:
				std::cout << cmd->arguments[2].arg.a_float;
				break;
			case TGF_INT16:
				std::cout << cmd->arguments[2].arg.a_int16;
				break;
			case TGF_STRING:
				std::cout << "\"" << cmd->arguments[2].arg.a_string << "\"";
				break;
			}

			std::cout << ")";
		}

		std::cout << " )" << std::endl;

		if (!*idtf) {
			idtf = reinterpret_cast<char *>(malloc(16));
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

int main(int argc, char *argv[])
{
	try {
		TCLAP::CmdLine cmd("Dumper of TGF format to text representation", ' ', "1.0");

		cmd.add(arg_output_file);
		cmd.add(arg_input_file);
		cmd.add(arg_raw_mode);

		cmd.parse(argc, argv);
	} catch (TCLAP::ArgException &e) {
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}

	// Check input tgf file.
	//
	fs::path input_path  = fs::absolute(arg_input_file.getValue());

	if (!fs::exists(input_path)) {
		std::cerr << "Error: Input tgf-file for path " << input_path << " not found." << std::endl;
		return 2;
	}

	if (!fs::is_regular_file(input_path)) {
		std::cerr << "Input tgf-file path " << input_path << " isn't path of regular file." << std::endl;
		return 3;
	}

	if (arg_raw_mode.getValue()) {
		FILE *in = fopen(input_path.string().c_str(), "rb");

		if (!in) {
			std::cerr << "failed to open input file" << std::endl;
			return 4;
		}

		int fd = fileno(in);

		tgf_stream_in *s = tgf_stream_in_new();
		if (!s) {
			std::cerr << "failed to create input stream" << std::endl;
			return 5;
		}

		tgf_stream_in_fd(s,fd);

		if (tgf_stream_in_start(s,0)<0) {
			std::cerr << "tgf_stream_in_start" << std::endl;
			return 5;
		}

		if (tgf_stream_in_process_all(s,libtgf_callback,0)<0) {
			std::cerr << "tgf_stream_in_process_all" << std::endl;
			return 5;
		}

		tgf_stream_in_destroy(s);
	}


	return 0;
}