/*
 * This source file is part of OSTIS (Open Semantic Technology for Intelligent
 * Systems) For the latest info, see http://www.ostis.net
 *
 * Copyright (c) 2010 OSTIS
 *
 * OSTIS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * OSTIS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
 */

grammar SCPs;

options {
	language = C;
	memoize = true;
	backtrack = true;
}

@parser::preincludes {
	#include <libscc/precompiled_p.h>
}

@parser::includes {
	#include <libscc/scps_language.h>
	#include <libscc/scps_compiler.h>
	#include <libscc/scps_program.h>
	#include <libscc/scps_cond_block.h>
	#include <libscc/antlr3_scc_utils.h>
}

@parser::postinclude {
	inline
	scc::file_position *file_position(pANTLR3_COMMON_TOKEN tok)
	{
		return scc::cc->new_position(to_const_char(tok->input->fileName),
			tok->line, tok->charPosition + 1);
	}

	#define TOKEN_FILE_POS(o) file_position(LT(o))

	#define PREV_TOKEN_FILE_POS() TOKEN_FILE_POS(-1)
}

@lexer::preincludes {
	#include <libscc/precompiled_p.h>
}

@lexer::includes {
	#include <libscc/scps_lexer.h>
	#include <libscc/scps_compiler.h>
	#include <libscc/antlr3_scc_utils.h>
}

text
	: textEntry* EOF
	;

textEntry
	: LINE_COMMAND
	| sctextRoot
	| programDeclr
	;

//
// SCPs grammar rules
//

programDeclr
scope { scc::program *prg; }
@init {
	$programDeclr::prg = 0;
	sc_addr prg_sign = SCADDR_NIL;
}
	: '@programSCP' ( termIdtf { prg_sign = $termIdtf.ref->obj()->sign(); } )? {
		$programDeclr::prg = scc::cc->create_program(prg_sign);
		scc::cc->push_program($programDeclr::prg);
	}
	  programParams embeddedStatement {
		$programDeclr::prg->set_main_block($embeddedStatement.blk);
		scc::cc->pop_program();
		std::cout << *$programDeclr::prg << std::endl;
	}
	;

programParams
	: '(' ( programParam ( ',' programParam )* )? ')'
	;

programParam
	: scattributesWithReturn termIdtf {
		$programDeclr::prg->add_prm($termIdtf.ref->obj()->sign(), *$scattributesWithReturn.attrs);
		delete_not_empty($scattributesWithReturn.attrs);
	}
	;

embeddedStatement returns [scc::code_block *blk]
@init { $blk = 0; }
	: statement
	| '{' {
		scc::cc->push_scope();
		$blk = new scc::sequence_code_block();
	}
		( st=statement { if ($st.blk) $blk->add($st.blk); } )*
	  '}' { scc::cc->pop_scope(); }
	;

statement returns [scc::code_block *blk]
@init { $blk = 0; }
	: constStatement
	| varStatement
	| controlStatement { $blk = $controlStatement.blk; }
	;

constStatement
	: 'const' 
		'{' { scc::cc->generator().enter_system($programDeclr::prg->get_consts()); }
			sctext
		'}' ';' { scc::cc->generator().leave_system(); }
	;

varStatement
	: 'var' t1=termIdtf { $programDeclr::prg->add_var($t1.ref->obj()->sign()); }
		( ',' t2=termIdtf { $programDeclr::prg->add_var($t2.ref->obj()->sign()); } )* ';'
	;

controlStatement returns [scc::code_block *blk]
@init { $blk = 0; }
	: ( l=IDTF ':' {
		scc::code_block *lbl = $programDeclr::prg->create_label(to_const_char($l.text));
		if ($blk == 0)
			$blk = new scc::sequence_code_block;
		$blk->add(lbl);
	} )*
	operatorStatement {
		if ($blk != 0) {
			$blk->add($operatorStatement.blk);
		} else {
			$blk = $operatorStatement.blk;
		}
	}
	;

operatorStatement returns [scc::code_block *blk]
scope { scc::custom_operator *op; }
@init {
	$operatorStatement::op = 0;
	$blk = 0;
}
	: termIdtf {
		if (scc::custom_operator::is_scp_op($termIdtf.ref->obj()->sign())) {
			$blk = $operatorStatement::op = new scc::custom_operator($programDeclr::prg);
			$operatorStatement::op->set_type($termIdtf.ref->obj()->sign());
		} else {
			$blk = $operatorStatement::op = new scc::call_operator($programDeclr::prg);
		}
	}
	'(' ( operatorOpnd ( ',' operatorOpnd )* )? ')' ';'
	;

operatorOpnd
@init { scc::lang_object *opnd = 0; }
	: scattributesWithReturn                          { scc::cc->set_expect_label(true); }
		( termSimple { opnd = $termSimple.ref->obj(); } )? {
		if (!($scattributesWithReturn.attrs->empty() && !opnd)) {
			//$operatorStatement::op->add_opnd($scattributesWithReturn.attrs, opnd);
			delete_not_empty($scattributesWithReturn.attrs);
		}
	}
	;

/*
ifStatement returns [if_structure *blk]
@init { $blk = new if_structure(); }
	: 'if' '(' condExpr ')'      { $blk->set_cond($condExpr.blk); }
	  embeddedStatement          { $blk->set_then($embeddedStatement.blk); }
	  ( ('else')=> elseStatement { $blk->set_else($elseStatement.blk); } )?
	;

elseStatement returns [code_block *blk]
@init { $blk = 0; }
	: 'else' embeddedStatement { $blk = $embeddedStatement.blk; }
	;
*/

condExpr returns [scc::code_block *blk]
@init { $blk = 0; }
	: fst=logicalAndExpr
		( '||' snd=logicalAndExpr {
			if (!$blk) {
				$blk = new scc::or_cond_block();
				$blk->add($fst.blk);
			}
			$blk->add($snd.blk);
		} )* { if (!$blk) $blk = $fst.blk; }
	;

logicalAndExpr returns [scc::code_block *blk]
@init { $blk = 0; }
	: fst=logicalUnaryExpr
		( '&&' snd=logicalUnaryExpr {
			if (!$blk) {
				$blk = new scc::and_cond_block();
				$blk->add($fst.blk);
			}
			$blk->add($snd.blk);
		} )* { if (!$blk) $blk = $fst.blk; }
	;

logicalUnaryExpr returns [scc::code_block *blk]
@init { $blk = 0; }
	: operatorStatement      { $blk = $operatorStatement.blk;           }
	| '!' e=logicalUnaryExpr { $blk = new scc::not_cond_block($e.blk); }
	| '(' condExpr ')'       { $blk = $condExpr.blk;                    }
	;

/*
whileStatement returns [while_block *blk]
@init { $blk = new while_block(); }
	: 'while' '(' condExpr { $blk->set_cond($condExpr.blk); } ')'
		embeddedStatement { $blk->set_body(embeddedStatement.blk); }
	;

doWhileStatement returns [do_while_block *blk]
@init { $blk = new do_while_block(); }
	: 'do' embeddedStatement { $blk->set_body($embeddedStatement.blk); }
		'while' '(' condExpr ')' { $blk->set_cond(condExpr.blk); }
	;
*/

//
// SCs grammar rules
//

sctextRoot
	: sctext
	;

sctext
	: ( scsentence ';' )+
	;

sctextNoSemi
	: ( scsentence ( ';' scsentence )* )*
	;

scsentence
@init { scc::sc_idents_list *from = 0; }
	: f=scidentsWithReturn { from = $f.idents; }
		( connector o=scidentsWithReturn {
			if ($connector.rd)
				scc::cc->scsentence(*from, $connector.t, $connector.pos, *$o.idents);
			else
				scc::cc->scsentence(*$o.idents, $connector.t, $connector.pos, *from);

			scc::delete_members_and_container(from);
			from = $o.idents;
		} )* {
			scc::delete_members_and_container(from);
		}
	;

scidents
	: scident ( ',' scident )*
	;

scidentsWithReturn returns [scc::sc_idents_list *idents]
@init { $idents = new scc::sc_idents_list(); }
	: f=scidentWithReturn { $idents->push_back($f.ident); }
		( ',' o=scidentWithReturn { $idents->push_back($o.ident); } )*
	;

scident returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: scattributes aliases termWithContent {
		$ref = scc::cc->assign_aliases($termWithContent.ref, *$aliases.list);
		delete_members_and_container($aliases.list);
	}
	;

scidentWithReturn returns [scc::sc_ident *ident]
@init { $ident = 0; }
	: scattributesWithReturn aliases termWithContent {
		scc::lang_object_ref *ref = scc::cc->assign_aliases($termWithContent.ref, *$aliases.list);
		delete_members_and_container($aliases.list);
		$ident = new scc::sc_ident($scattributesWithReturn.attrs, ref);
	}
	;

aliases returns [scc::sc_aliases_list *list]
@init { $list = new scc::sc_aliases_list(); }
	: ( alias { $list->push_back($alias.alias); } )*
	;

alias returns [scc::sc_alias *alias]
@init { $alias = 0; }
	: idtf=( IDTF | IDTF_IN_QUOTES ) '=' { $alias = new scc::sc_alias(to_const_char($idtf.text), TOKEN_FILE_POS(-2)); }
	;

scattributesWithReturn returns [scc::sc_attributes *attrs]
@init { $attrs = 0; }
	: ( scattributeWithReturn {
			if (!$attrs)
				$attrs = new scc::sc_attributes();
			$attrs->add($scattributeWithReturn.ref->obj(), $scattributeWithReturn.arc_type);
	} )*
	;

scattributeWithReturn returns [scc::lang_object_ref *ref, sc_type arc_type]
@init { $ref = 0; }
	: termIdtf { $ref = $termIdtf.ref; }
	( ':'      { $arc_type = SC_CONST;   }
	| '::'     { $arc_type = SC_VAR;     }
	| ':::'    { $arc_type = SC_METAVAR; }
	)
	;

scattributes
	: ( scattribute )*
	;

scattribute
	: termIdtf ':'   { scc::cc->scattribute($termIdtf.ref, SC_CONST,   TOKEN_FILE_POS(-2)); }
	| termIdtf '::'  { scc::cc->scattribute($termIdtf.ref, SC_VAR,     TOKEN_FILE_POS(-2)); }
	| termIdtf ':::' { scc::cc->scattribute($termIdtf.ref, SC_METAVAR, TOKEN_FILE_POS(-2)); }
	;

termWithContent returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: term { $ref = $term.ref; }
		( '=c=' str=( STRING_BASE64 | STRING ) { scc::cc->assign_string_content($ref, to_const_char($str.text)); }
		| '=n=' (
					NUMBER_INTEGER  { scc::cc->assign_integer_content($ref, to_const_char($NUMBER_INTEGER.text),
										PREV_TOKEN_FILE_POS()); }
					| NUMBER_DOUBLE { scc::cc->assign_double_content($ref, to_const_char($NUMBER_DOUBLE.text),
										PREV_TOKEN_FILE_POS()); }
				)
		| '=fe=' IDTF_IN_QUOTES { scc::cc->assign_file_content($ref, to_const_char($IDTF_IN_QUOTES.text), PREV_TOKEN_FILE_POS()); }
		)?
	| str=( STRING_BASE64 | STRING ) {
		$ref = scc::cc->term_from_content(PREV_TOKEN_FILE_POS());
		scc::cc->assign_string_content($ref, to_const_char($str.text));
	}
	| NUMBER_INTEGER_LONG {
		$ref = scc::cc->term_from_content(PREV_TOKEN_FILE_POS());
		scc::cc->assign_integer_content($ref, to_const_char($NUMBER_INTEGER_LONG.text), PREV_TOKEN_FILE_POS());
	}
	| NUMBER_DOUBLE_LONG {
		$ref = scc::cc->term_from_content(PREV_TOKEN_FILE_POS());
		scc::cc->assign_double_content($ref, to_const_char($NUMBER_DOUBLE_LONG.text),
										PREV_TOKEN_FILE_POS()); }
	;

term returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: termSimple { $ref = $termSimple.ref; }
	| termSet    { $ref = $termSet.ref;    }
	| termSys    { $ref = $termSys.ref;    }
	| '(' from=scidentWithReturn connector to=scidentWithReturn ')' {
		if ($connector.rd) {
			$ref = scc::cc->connector($from.ident, $connector.t, $connector.pos, $to.ident);
		} else {
			$ref = scc::cc->connector($to.ident, $connector.t, $connector.pos, $from.ident);
		}

		delete $from.ident;
		delete $to.ident;
	}
	;

termSimple returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: termIdtf       { $ref = scc::cc->term_simple($termIdtf.ref); }
	;

termIdtf returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: IDTF           { $ref = scc::cc->term_idtf(to_const_char($IDTF.text), PREV_TOKEN_FILE_POS());           }
	| IDTF_IN_QUOTES { $ref = scc::cc->term_idtf(to_const_char($IDTF_IN_QUOTES.text), PREV_TOKEN_FILE_POS()); }
	;

connector returns [sc_type t, bool rd, const scc::file_position *pos]
/* rd - right direction of arc. */
@init { $t = SC_EMPTY; $rd = false; }
	:
	(
		(
			( '~' { $t |= SC_TEMPORARY; } )?
			(
				'->'   { $t |= SC_ARC_ACCESSORY|SC_CONST|SC_POS;   } |
				'->>'  { $t |= SC_ARC_ACCESSORY|SC_VAR|SC_POS;     } |
				'->>>' { $t |= SC_ARC_ACCESSORY|SC_METAVAR|SC_POS; } |

				'~>'   { $t |= SC_ARC_ACCESSORY|SC_CONST|SC_FUZ;   } |
				'~>>'  { $t |= SC_ARC_ACCESSORY|SC_VAR|SC_FUZ;     } |
				'~>>>' { $t |= SC_ARC_ACCESSORY|SC_METAVAR|SC_FUZ; } |

				'/>'   { $t |= SC_ARC_ACCESSORY|SC_CONST|SC_NEG;   } |
				'/>>'  { $t |= SC_ARC_ACCESSORY|SC_VAR|SC_NEG;     } |
				'/>>>' { $t |= SC_ARC_ACCESSORY|SC_METAVAR|SC_NEG; }
			) {
				$t = sc_type_add_default($t, SC_PERMANENCY_MASK, SC_PERMANENT);
				$rd = true;
			}
		) |

		(
			(
				'<-'   { $t |= SC_ARC_ACCESSORY|SC_CONST|SC_POS;   } |
				'<<-'  { $t |= SC_ARC_ACCESSORY|SC_VAR|SC_POS;     } |
				'<<<-' { $t |= SC_ARC_ACCESSORY|SC_METAVAR|SC_POS; } |

				'<~'   { $t |= SC_ARC_ACCESSORY|SC_CONST|SC_FUZ;   } |
				'<<~'  { $t |= SC_ARC_ACCESSORY|SC_VAR|SC_FUZ;     } |
				'<<<~' { $t |= SC_ARC_ACCESSORY|SC_METAVAR|SC_FUZ; } |

				'</'   { $t |= SC_ARC_ACCESSORY|SC_CONST|SC_NEG;   } |
				'<</'  { $t |= SC_ARC_ACCESSORY|SC_VAR|SC_NEG;     } |
				'<<</' { $t |= SC_ARC_ACCESSORY|SC_METAVAR|SC_NEG; }
			) {
				$t = sc_type_add_default($t, SC_PERMANENCY_MASK, SC_PERMANENT);
				$rd = false;
			}
			( '~' { $t |= SC_TEMPORARY; } )?
		) |

		( '=>'   { $rd = true; } | '<='   { $rd = false; } ) { $t |= SC_ARC|SC_CONST;   } |
		( '=>>'  { $rd = true; } | '<<='  { $rd = false; } ) { $t |= SC_ARC|SC_VAR;     } |
		( '=>>>' { $rd = true; } | '<<<=' { $rd = false; } ) { $t |= SC_ARC|SC_METAVAR; } |
	) { $pos = PREV_TOKEN_FILE_POS(); }
	;

termSet returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: '{'   { $ref = scc::cc->enter_term_set(SC_N_CONST, PREV_TOKEN_FILE_POS());   } scidents? '}'
		{ scc::cc->leave_term_set(); }
	| '{.'  { $ref = scc::cc->enter_term_set(SC_N_VAR, PREV_TOKEN_FILE_POS());     } scidents? '.}'
		{ scc::cc->leave_term_set(); }
	| '{..' { $ref = scc::cc->enter_term_set(SC_N_METAVAR, PREV_TOKEN_FILE_POS()); } scidents? '..}'
		{ scc::cc->leave_term_set(); }
	;

termSys returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: '['   { $ref = scc::cc->enter_term_sys(SC_N_CONST, PREV_TOKEN_FILE_POS());   } ( sctextNoSemi ';' )* ']'
		{ scc::cc->leave_term_sys(); }
	| '[.'  { $ref = scc::cc->enter_term_sys(SC_N_VAR, PREV_TOKEN_FILE_POS());     } ( sctextNoSemi ';' )* '.]'
		{ scc::cc->leave_term_sys(); }
	| '[..' { $ref = scc::cc->enter_term_sys(SC_N_METAVAR, PREV_TOKEN_FILE_POS()); } ( sctextNoSemi ';' )* '..]'
		{ scc::cc->leave_term_sys(); }
	;

termOrdTuple returns [scc::lang_object_ref *ref]
@init { $ref = 0; }
	: '('   { $ref = scc::cc->enter_term_ord_tuple(SC_N_CONST,   PREV_TOKEN_FILE_POS()); } scidents? ')'
		{ scc::cc->leave_term_ord_tuple(); }
	| '(.'  { $ref = scc::cc->enter_term_ord_tuple(SC_N_VAR,     PREV_TOKEN_FILE_POS()); } scidents? '.)'
		{ scc::cc->leave_term_ord_tuple(); }
	| '(..' { $ref = scc::cc->enter_term_ord_tuple(SC_N_METAVAR, PREV_TOKEN_FILE_POS()); } scidents? '..)'
		{ scc::cc->leave_term_ord_tuple(); }
	;

//
// SCs lexer
//

NUMBER_INTEGER
	: ( '+' | '-' )? DIGIT+
	;

NUMBER_INTEGER_LONG
	: 'n/' num=NUMBER_INTEGER '/' { SETTEXT($num.text); }
	;

NUMBER_DOUBLE
	: ( '+' | '-' )? DIGIT+ '.' DIGIT*
	;

NUMBER_DOUBLE_LONG
	: 'n/' num=NUMBER_DOUBLE '/' { SETTEXT($num.text); }
	;

fragment
STRING_GUTS : ( ~( '\\' | '"' ) )* ;

LINE_COMMAND
@init { scc::file_position *pos = 0;}
	: { pos = scc::cc->new_position(to_const_char(LEXER->input->fileName),
			GETLINE(), GETCHARPOSITIONINLINE() + 1); }
	'#' ( ' ' | '\t' )*
	(
		'include' ( ' ' | '\t' )+ '"' file=STRING_GUTS '"' ( ' ' | '\t' )* '\r'? '\n' {
			SCC_TRY {
				fs::path included_path = scc::cc->find_include_file(to_const_char($file.text));
				pANTLR3_INPUT_STREAM in = open_file(included_path);
				PUSHSTREAM(in);
			} SCC_CATCH_POS(pos)
		}
	| ( ('0'..'9')=>('0'..'9') )+ ~( '\n' | '\r' )* '\r'? '\n'
	) { $channel = HIDDEN; }
	;


MULTILINE_COMMENT
	: '/*' ( options { greedy=false; }: . )* '*/' { $channel = HIDDEN; }
	;

LINE_COMMENT
	: '//' .* NEWLINE { $channel = HIDDEN; }
	;

IDTF
	: ( '$' )? ( LETTER | DIGIT | '_' | '*' | '@' )+
	;

fragment
IDTF_IN_QUOTES_CONTENT
	: ( ~( '"' ) )+
	;

IDTF_IN_QUOTES
	: '"' idtf=IDTF_IN_QUOTES_CONTENT '"' { SETTEXT($idtf.text); }
	;

STRING_BASE64
	: 'b64/"' ( ( 'A'..'Z' | 'a'..'z' | '0'..'9' | '+' | '/' | '=' | ' ' | '\r' | '\n' | '\t' ) )* '"/' {
		SETTEXT(scc::process_string_base64_token($text));
	}
	;

STRING
	: '/"' ( ~( '"' ) )* '"/' { SETTEXT(scc::process_string_token($text)); }
	;

fragment DIGIT
	: ( '0'..'9' )
	;

fragment LETTER
	: ( 'A'..'Z' | 'a'..'z' | '\u0080'..'\ufffe' )
	;

fragment NEWLINE
	: ( '\r\n' | '\n\r' | '\r' | '\n' )
	;

fragment WS
	: ( ' ' | '\t' )
	;

NEWLINE_HIDE
	: NEWLINE { $channel = HIDDEN; }
	;

WS_HIDE
	: WS { $channel = HIDDEN; }
	;
