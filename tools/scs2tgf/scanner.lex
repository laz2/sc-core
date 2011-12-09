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
 * @file scanner.lex
 * @author ALK
 * @brief flex scanner definition for SCs.
 */

%{
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser.h"
#include "values.h"
#include "parser.tab.h"
#include "sctext.h"

extern YYLTYPE yylloc;

#define YY_NEVER_INTERACTIVE 1
#define YY_NO_UNISTD_H 1
#define YY_NO_INPUT 1

typedef struct _state {
	char *filename;
	int line;
	YY_BUFFER_STATE bufstate;
} state;

#define MAX_INCLUDE_DEPTH 16
state include_stack[MAX_INCLUDE_DEPTH];
int stack_ptr = 0;

char *cfilename = 0;

#include <string.h>
#include "misc.h"

%}

%option nounput
%option noyywrap
%option never-interactive

%x COMMENT
%x INCLUDE
%x COMMENT_OR_INCLUDE
%x XCOMMENT
%%

^"#"			BEGIN(COMMENT_OR_INCLUDE);
"#"			BEGIN(XCOMMENT);
<COMMENT_OR_INCLUDE>[ \t]*
<COMMENT_OR_INCLUDE>"include"[ \t]*"\""	BEGIN(INCLUDE);
<COMMENT_OR_INCLUDE>"toplevel"		BEGIN(0); return TOK_PRAGMA_TOPLEVEL;
<COMMENT_OR_INCLUDE>.			BEGIN(XCOMMENT);
<XCOMMENT>.*	BEGIN(0);

<INCLUDE>[^"]+"\"" {
		static char filename[256];
		char *path;
		char *path2;
		char **include = include_paths;

		if (strlen(yytext) > 256-1) {
			fprintf(stderr,"include filename %s is too long\n",yytext);
			exit(2);
		}
		strcpy(filename,yytext);
		filename[strlen(filename)-1] = 0;

		if (stack_ptr >= MAX_INCLUDE_DEPTH) {
			fprintf(stderr,"Includes nested too deeply\n");
			exit(1);
		}

		include_stack[stack_ptr].filename = cfilename;
		include_stack[stack_ptr].line = yylloc.last_line;
		include_stack[stack_ptr].bufstate = YY_CURRENT_BUFFER;
		yylloc.last_line = 1;
		stack_ptr++;

		path = strdup(cfilename);
		path2 = my_dirname(path);

		yyin = 0;
		while (*include && !yyin) {
			/*if (**include == '/') {*/
				cfilename = malloc(strlen(*include)+strlen(filename)+2);
				strcpy(cfilename,*include++);
				strcat(cfilename,"/");
				strcat(cfilename,filename);
			/*} else {
				cfilename = malloc(strlen(*include)+strlen(filename)+strlen(path)+3);
				strcpy(cfilename,path2);
				strcat(cfilename,"/");
				strcat(cfilename,*include++);
				strcat(cfilename,"/");
				strcat(cfilename,filename);
			}*/
			yyin = fopen(cfilename,"rt");
			if (!yyin)
				free(cfilename);
		}
		if (!yyin) {
			cfilename = malloc(strlen(path2)+strlen(filename)+3);
			strcpy(cfilename,path2);
			strcat(cfilename,"/");
			strcat(cfilename,filename);
			yyin = fopen(cfilename,"rt");
		}

		if (!yyin) {
			fprintf(stderr,"cannot open file %s: %s included from %s:%d\n",
				cfilename, strerror(errno), include_stack[stack_ptr-1].filename, include_stack[stack_ptr-1].line);
			exit(2);
		}

		free(path);
		yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
		BEGIN(0);
	}
<<EOF>>	{
		if (--stack_ptr<0)
			yyterminate();
		else {
			free(cfilename);
			cfilename = include_stack[stack_ptr].filename;
			yylloc.last_line = include_stack[stack_ptr].line;
			fclose(YY_CURRENT_BUFFER->yy_input_file);
			yy_delete_buffer(YY_CURRENT_BUFFER);
			yy_switch_to_buffer(include_stack[stack_ptr].bufstate);
		}
	}
		/* skip comment rules. Do not forget to count lines */
"/*"		BEGIN(COMMENT);
<COMMENT>("\n\r"|"\r\n"|"\r"|"\n")	{yylloc.last_line++;}
		/* a little broken rule it will allow a bit incorrect input */
<COMMENT>[^*/\n\r]*
<COMMENT>"*"+
<COMMENT>"/"+
		/* <COMMENT>"*"+/[^/] */
<COMMENT>"*"+"/"	BEGIN(0);

("//").*("\n\r"|"\r\n"|"\n"|"\r")	yylloc.last_line++;

		/* eat whitespace */
(" "|"\t")+
		/* special rule to count lines */
("\n\r"|"\r\n"|"\r"|"\n")	yylloc.last_line++;
		/* identifier rule */
		/* Be careful with value. It's duplicated ! */
[\x80-\xff$A-Za-z0-9_-]*[\x80-\xff$A-Za-z0-9_] yylval.str = strdup(yytext); return TOK_IDENTIFIER;

		/* these identifiers are special exception */
("@node"|"@arc"|"@undf"|"@const"|"@var"|"@metavar"|"@pos"|"@neg"|"@fuz") {
			yylval.str = strdup(yytext); return TOK_IDENTIFIER;
		}

		/* quoted identifier */
		/* if malloc fails it just returns empty identifier */
		/* this should be replaced on special token 'lexer error' */
\"((\\\")?[\[\]\\~`!@#$%^&*()_\-+={}:;'/?.>,<|A-Za-z0-9_\t \r\n\x80-\xff]*)+\" {
				int l = strlen(yytext)-2;
				char *str = malloc(l+1),*p,*q;
				if (!str) {
					yylval.str = str;
					return TOK_IDENTIFIER;
				}
				p = str ; q = yytext+1;
				while (l-->0) {
					if (*q != '\\') {
						*p++ = *q++;
						continue;
					}
					q++; l--;
					if (l<0) {
						*q++ = '\\';
						break;
					}
					switch (*q) {
					case 'n':
						*p++ = '\n';
						break;
					case 'r':
						*p++ = '\r';
						break;
					case 't':
						*p++ = '\t';
						break;
					default:
						*p++ = *q;
					}
					q++;
				}
				*p = 0;
				yylval.str = str;
				return TOK_IDENTIFIER;
			}

"/"\"((\\\")?[\[\]\\~`!@#$%^&*()_\-+={}:;'/?.>,<|A-Za-z0-9_\t \r\n\x80-\xff]*)+\""/" {
				int l = strlen(yytext)-4;
				char *str = malloc(l+1),*p,*q;
				if (!str) {
					yylval.str = str;
					return TOK_STRING;
				}
				p = str ; q = yytext+2;
				while (l-->0) {
					if (*q == '\n') {
						*p++ = *q++;
						if (*q == '\r')
							*p++ = *q++;
						yylloc.last_line++;
						continue;
					}
					if (*q == '\r') {
						*p++ = *q++;
						if (*q == '\n')
							*p++ = *q++;
						yylloc.last_line++;
						continue;
					}
					if (*q != '\\') {
						*p++ = *q++;
						continue;
					}
					q++; l--;
					if (l<0) {
						*q++ = '\\';
						break;
					}
					switch (*q) {
					case 'n':
						*p++ = '\n';
						break;
					case 'r':
						*p++ = '\r';
						break;
					case 't':
						*p++ = '\t';
						break;
					default:
						*p++ = *q;
					}
					q++;
				}
				*p = 0;
				yylval.str = str;
				return TOK_STRING;
			}


		/* arc connectors */
		/* first value means Positive, Const, to Right - PCR */
"->"	yylval.con = CON_PCR; return TOK_ARC_CON;
"<-"	yylval.con = CON_PCL; return TOK_ARC_CON;
"->_"	yylval.con = CON_PVR; return TOK_ARC_CON;
"->>"	yylval.con = CON_PVR; return TOK_ARC_CON;
"-->"	yylval.con = CON_PVR; return TOK_ARC_CON;
"->>>"	yylval.con = CON_PMR; return TOK_ARC_CON;
"--->"	yylval.con = CON_PMR; return TOK_ARC_CON;
"_<-"	yylval.con = CON_PVL; return TOK_ARC_CON;
"<<-"	yylval.con = CON_PVL; return TOK_ARC_CON;
"<--"	yylval.con = CON_PVL; return TOK_ARC_CON;
"<<<-"	yylval.con = CON_PML; return TOK_ARC_CON;
"<---"	yylval.con = CON_PML; return TOK_ARC_CON;
"~>"	yylval.con = CON_FCR; return TOK_ARC_CON;
"<~"	yylval.con = CON_FCL; return TOK_ARC_CON;
"~>_"	yylval.con = CON_FVR; return TOK_ARC_CON;
"~>>"	yylval.con = CON_FVR; return TOK_ARC_CON;
"-~>"	yylval.con = CON_FVR; return TOK_ARC_CON;
"~>>>"	yylval.con = CON_FMR; return TOK_ARC_CON;
"--~>"	yylval.con = CON_FMR; return TOK_ARC_CON;
"_<~"	yylval.con = CON_FVL; return TOK_ARC_CON;
"<<~"	yylval.con = CON_FVL; return TOK_ARC_CON;
"<~-"	yylval.con = CON_FVL; return TOK_ARC_CON;
"/>"	yylval.con = CON_NCR; return TOK_ARC_CON;
"</"	yylval.con = CON_NCL; return TOK_ARC_CON;
"/>>"	yylval.con = CON_NVR; return TOK_ARC_CON;
"-/>"	yylval.con = CON_NVR; return TOK_ARC_CON;
"<</"	yylval.con = CON_NVL; return TOK_ARC_CON;
"</-"	yylval.con = CON_NVL; return TOK_ARC_CON;
"/>>>"	yylval.con = CON_NMR; return TOK_ARC_CON;
"--/>"	yylval.con = CON_NMR; return TOK_ARC_CON;
"<<</"	yylval.con = CON_NML; return TOK_ARC_CON;
"</--"	yylval.con = CON_NML; return TOK_ARC_CON;


		/* non-arc connectors */
		/* first value means Non-Arc to Right - NAR */
">-"	yylval.con = CON_NAR; return TOK_NONARC_CON;
"-<"	yylval.con = CON_NAL; return TOK_NONARC_CON;
		/* old style non-arc connectors */
"*>"	yylval.con = CON_NAR; return TOK_NONARC_CON;
"<*"	yylval.con = CON_NAL; return TOK_NONARC_CON;

"n/"[-+]?[.0-9]+"/" {
		int len = strlen(yytext);
		char *p;
		yylval.str = malloc(len-2);
		memcpy(yylval.str,yytext+2,len-3);
		yylval.str[len-3] = 0;
		p = yylval.str;
		while (*p) {
			if (*p == '\n') {
				p++;
				if (*p == '\r')
					p++;
				yylloc.last_line ++;
				continue;
			} else if (*p == '\r') {
				p++;
				if (*p == '\n')
					p++;
				yylloc.last_line ++;
				continue;
			}
			p++;
			continue;
		}
		return TOK_NUMBER;
	}
"b64/\""([A-Za-z0-9+/= ]|"\n"|"\t"|"\r")*"\"/" {
		int len = strlen(yytext);
		char *p;
		yylval.str = malloc(len-6);
		memcpy(yylval.str,yytext+5,len-7);
		yylval.str[len-7] = 0;
		p = yylval.str;
		while (*p) {
			if (*p == '\n') {
				p++;
				if (*p == '\r')
					p++;
				yylloc.last_line ++;
				continue;
			} else if (*p == '\r') {
				p++;
				if (*p == '\n')
					p++;
				yylloc.last_line ++;
				continue;
			}
			p++;
		}
		return TOK_BASE64;
	}

"{."	return TOK_VARSET_OPEN;
".}"	return TOK_VARSET_CLOSE;
"[."	return TOK_VARSYS_OPEN;
".]"	return TOK_VARSYS_CLOSE;
"{.."	return TOK_METAVARSET_OPEN;
"..}"	return TOK_METAVARSET_CLOSE;
"[.."	return TOK_METAVARSYS_OPEN;
"..]"	return TOK_METAVARSYS_CLOSE;

"::"	return TOK_ATTR_VAR;
":::"	return TOK_ATTR_METAVAR;

"=c="	return TOK_STRING_CONT;
"=fe="	return TOK_FILE_CONT;
"=n="	return TOK_NUM_CONT;

		/* by default return token of one symbol */
.	return *yytext;
