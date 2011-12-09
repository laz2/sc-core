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
 * @file parser.y
 * @author ALK
 * @brief bison parser definition for SCs.
 */

%{

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * bison generated scanner requires free(). It can't get it by himself. BUG !
 * On some systems it will also use malloc() and (probably) include
 * <malloc.h> or <stdlib.h>
 * Installed on my system bison template version does not include anything
 * except <alloca.h> on some Unices. Certainly, this is a BUG !
 */
#include <stdlib.h>

#include "sctext.h"
#include "parser.h"
#include "values.h"
#include "list.h"

#define YYERROR_VERBOSE

#define test_call(expr,m) do { if ((expr)) { fprintf(stderr,"rule failed at file %s line %d\n",cfilename,m.last_line);YYERROR;} } while (0)
#define __test_call(expr) do { if ((expr)) {YYERROR;} } while (0)

void	yyerror(const char *);

#define YYDEBUG 1

#define dummy(a)

%}

%union {
	textvalue *text;
	elink	*list;
	slink *strings;
	con_type con;
	char *str;
	int ref;
	int number;
}

%token	<str>		TOK_IDENTIFIER
%token	<str>		TOK_STRING
%token	<con>		TOK_ARC_CON
%token	<con>		TOK_NONARC_CON
%token	<str>		TOK_BASE64
%token	<str>		TOK_NUMBER

%token	TOK_VARSET_OPEN TOK_VARSET_CLOSE TOK_VARSYS_OPEN TOK_VARSYS_CLOSE
%token	TOK_METAVARSET_OPEN TOK_METAVARSET_CLOSE TOK_METAVARSYS_OPEN TOK_METAVARSYS_CLOSE
%token	TOK_ATTR_VAR TOK_ATTR_METAVAR
%token	TOK_FILE_CONT TOK_STRING_CONT TOK_NUM_CONT
%token	TOK_PRAGMA_TOPLEVEL

%type	<text>		sctext
%type	<text>		scsentence
%type	<text>		scidents
%type	<text>		scident
%type	<text>		attributes
%type	<strings>	aliases
%type	<text>		term
%type	<con>		connector
%type	<text>		sctext_top_level
%type	<text>		sctext_tl_prim
%type	<text>		sctext_no_semi
%type	<text>		sc_empty
%type	<text>		sc_text_may_empty
%type	<text>		sc_idents_may_empty
%type	<text>		content_sentence
%type	<text>		string_content_sentence
%type	<text>		numeric_content_sentence
%type	<text>		file_content_sentence

%%

sctext_top_level:	sctext_tl_prim {$$ = $1;}
		|	sctext_top_level sctext_tl_prim {text_add($1,$2); $$ = $2;}
		;

sctext_tl_prim:		sctext_no_semi ';' {$$ = $1;dummy(@1);}
		|	TOK_PRAGMA_TOPLEVEL sctext_no_semi ';' {$$ = $2}
		|	TOK_PRAGMA_TOPLEVEL {test_call(text_new(&($$)),@1);}
		;

sctext:		sctext_no_semi		{$$ = $1;}
		| sctext_no_semi ';'	{$$ = $1;}
		;

sctext_no_semi: scsentence  { $$ = $1; }
		| sctext_no_semi ';' scsentence { test_call(rule_text($1,$3),@1);$$ = $1;}
		;

scsentence:	scidents		{$$ = $1;}
		| scsentence connector scidents
					{test_call(rule_sent($1,$3,$2),@1);$$ = $1;}
		| content_sentence	{$$ = $1;}
		;

string_content_sentence:
		term TOK_STRING_CONT TOK_STRING {$$=$1;test_call(assign_cont($1,$3,CONT_STRING),@1);}
		;

numeric_content_sentence:
		term TOK_NUM_CONT TOK_IDENTIFIER {$$=$1;test_call(assign_cont($1,$3,CONT_NUMERIC),@1);}
		| term 
		;

file_content_sentence:
		term TOK_FILE_CONT TOK_IDENTIFIER	{$$=$1;test_call(assign_cont($1,$3,CONT_FILE),@1);}
		;

content_sentence:
		file_content_sentence	{$$ = $1;}
		| numeric_content_sentence	{$$ = $1;}
		| string_content_sentence	{$$ = $1;}
		;

scidents:	scident			{$$ = $1;}
		| scidents ',' scident	{text_add($1,$3);$$ = $3;}
		;

scident:	attributes aliases term	{test_call(rule_ident($2,$1,$3),@1);$$ = $3;}
		| aliases attributes term {test_call(rule_ident($1,$2,$3),@1); $$ = $3;}
		| aliases term		{test_call(rule_ident($1,0,$2),@1);$$ = $2;}
		| attributes term	{test_call(rule_ident(0,$1,$2),@1);$$ = $2;}
		| term			{test_call(rule_ident(0,0,$1),@1);$$ = $1;}
		;


attributes:	term ':' {
			list_entry(($1)->active_list.next,alink,alist)->attr_con_type = SC_CONST;
			$$ = $1;
		}
		| attributes term ':' {
			list_entry(($2)->active_list.next,alink,alist)->attr_con_type = SC_CONST;
			text_add($2,$1);
			$$ = $1;
		}
		| term TOK_ATTR_VAR {
			list_entry(($1)->active_list.next,alink,alist)->attr_con_type = SC_VAR;
			$$ = $1;
		}
		| term TOK_ATTR_METAVAR {
			list_entry(($1)->active_list.next,alink,alist)->attr_con_type = SC_METAVAR;
			$$ = $1;
		}
		| attributes term TOK_ATTR_VAR {
			list_entry(($2)->active_list.next,alink,alist)->attr_con_type = SC_VAR;
			text_add($2,$1);
			$$ = $1;
		}
		| attributes term TOK_ATTR_METAVAR {
			list_entry(($2)->active_list.next,alink,alist)->attr_con_type = SC_METAVAR;
			text_add($2,$1);
			$$ = $1;
		}
		;

aliases:	TOK_IDENTIFIER '=' {
			$$ = slink_new($1);
			if (!($$))
				YYERROR;
		}
		| aliases TOK_IDENTIFIER '='
					{test_call(rule_aliases($1,$2),@1);$$ = $1;}
		;

sc_empty:		{__test_call(text_new(&($$)));}
		;

sc_idents_may_empty:
		scidents {$$ = $1;}
		| sc_empty {$$ = $1;}
		;

sc_text_may_empty:
		sctext {$$ = $1}
		| sc_empty {$$ = $1;}
		;

		/*
		 * '(' idents ')' rule is to support old style SCs.
		 *  We should have warning option on this!
		 */
term:		TOK_IDENTIFIER		{test_call(rule_term_id($1,&($$)),@1);}
		| '[' sc_text_may_empty ']'	{test_call(rule_term_text($2,SC_CONST),@1);$$ = $2;}
		| '{' sc_idents_may_empty '}'	{test_call(rule_term_set($2,SC_CONST),@1);$$ = $2;}
		| '(' sc_idents_may_empty ')'	{test_call(rule_term_set($2,SC_CONST),@1);$$ = $2;}
		| '(' scident TOK_ARC_CON scident ')'
					{test_call(rule_term_arc($2,$3,$4,&($$)),@1);}
		| TOK_VARSET_OPEN sc_idents_may_empty TOK_VARSET_CLOSE
					{test_call(rule_term_set($2,SC_VAR),@1);$$ = $2;}
		| TOK_VARSYS_OPEN sc_text_may_empty TOK_VARSYS_CLOSE
					{test_call(rule_term_text($2,SC_VAR),@1);$$ = $2;}
		| TOK_METAVARSET_OPEN sc_idents_may_empty TOK_METAVARSET_CLOSE
					{test_call(rule_term_set($2,SC_METAVAR),@1);$$ = $2;}
		| TOK_METAVARSYS_OPEN sc_text_may_empty TOK_METAVARSYS_CLOSE
					{test_call(rule_term_text($2,SC_METAVAR),@1);$$ = $2;}
		| TOK_STRING		{test_call(rule_term_strcont($1,&($$)),@1);}
		| TOK_BASE64		{test_call(rule_term_base64cont($1,&($$)),@1);}
		| TOK_NUMBER		{test_call(rule_term_number($1,&($$)),@1);}
		;

connector:	TOK_ARC_CON		{$$ = $1;}
		| TOK_NONARC_CON	{$$ = $1;}
		;

%%

void	yyerror(const char *s)
{
	fprintf(stderr,"yyerror: %s at %s line number %d\n",s,cfilename, yylloc.last_line);
}
