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
 * @file sctext.h
 * @author ALK
 * @brief sc-text structures definition.
 */
 
#ifndef __SCTEXT_H__
#define __SCTEXT_H__

#include "parser.h"
#include "list.h"

/* SC type masks */
#define SC_METAVAR		0x100
#define SC_CONST		0x80
#define SC_VAR			0x40
#define SC_CONSTANCY_MASK	(SC_CONST | SC_VAR | SC_METAVAR)
#define SC_POS			0x20
#define SC_NEG			0x10
#define	SC_FUZ			0x08
#define SC_FUZZYNESS_MASK	(SC_POS | SC_NEG | SC_FUZ)
#define SC_TYPE_MASK_BASE 	(SC_CONSTANCY_MASK | SC_FUZZYNESS_MASK)
#define SC_NODE			0x04
#define SC_ARC			0x02
#define SC_UNDF			0x01
#define SC_GENERIC_TYPE_MASK	(SC_NODE | SC_ARC | SC_UNDF)

/* it shouldn't be here. But this constant is used in main.c:write_tgf() */
/* Not anymore */
#define SCTEXT_MAXSIZE	65536 * 2

/* types definitions */
typedef unsigned int sc_type;
typedef struct _scelement {
	char *id;
	sc_type type;
	char hidden;
	int alias; /* if >=0 points to real element */
	int beg,end; /* only for arc. Indexes */
	int cont_type;
	char *str;
	long double real;
	int segment;
	int datalen;
} scelement;

extern scelement sctext[];
extern int sctext_size;

/*extern char *segments[];
extern int segments_size;*/

/* id map functionality */
int	create_id_pair(char *id,int ref);
int	search_by_id(char *id);
int	init_idmap(void);

/* text manipulation functions */
int	add_el_full(char *id,int type,int left,int right);
int	add_arc_attr(int ref,int con,alink *right,char *id,struct list_head *inact);
int	add_con_attrs(alink *left,int con,alink *right,struct list_head *inact);
int	add_node(char *id,int type);
int	add_arc(int left,int con,int right,char *id);
int	add_el(char *id,int type);
int	add_alias(char *id,int ref);
int	add_cont(int ref,char *cont,int cont_type);

/* sets defaults to element's types */
void	set_text_defaults(void);
/* resolve all stored non-arc connectors */
int	resolve_nonarcs(void);

void	init_special_nodes();
int	handle_special_nodes();

#endif /*__SCTEXT_H__*/
