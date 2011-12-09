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
 * @file sctext.c
 * @author ALK
 * @brief SC text functionality.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <memory.h> /* some systems may need this for memset() */
			/* on glibc it's just alias for string.h */

#include "sctext.h"
#include "map.h"
#include "list.h"
#include "values.h"
#include "parser.h"

/* INLINE actually gets defined in list.h, but who knows how it will change */
/* I use __inline__ instead of inline to
 * not confuse -ansi and/or -pedantic GCC */
#ifndef INLINE
#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif /* __GCC__ */
#endif /* INLINE */

static	map *idmap = 0;

#define NONARCS_MAX	16384

/* elements [0,8] are occupied by special nodes @node,@arc,... */
scelement sctext[SCTEXT_MAXSIZE];
int sctext_size=0;

#define TYPE_NODES_START 0
#define NODE_SET	0
#define ARC_SET		1
#define UNDF_SET	2
#define CONST_SET	3
#define VAR_SET		4
#define METAVAR_SET	5
#define POS_SET		6
#define NEG_SET		7
#define FUZ_SET		8
#define TYPE_NODES_END	9

#define IS_SPECIAL_ELEMENT(ref) ((ref)<9)

static struct {
	int left,right;
} nonarcs[NONARCS_MAX];
int nonarcs_size=0;

static int leftcounts[SCTEXT_MAXSIZE];


int	comp_strings(const void *a,const void *b)
{
	return strcmp(a,b);
}

int	init_idmap(void)
{
	if (idmap) {
		map_destroy(idmap);
	}
	idmap = map_new(comp_strings);
	return !idmap;		/* 0 in case of success */
}

int	create_id_pair(char *id,int ref)
{
	return !map_insert(idmap,id,I2P(ref));
}

int	search_by_id(char *id)
{
	void *t;
	if (!id)
		return -1;
	if (!map_find(idmap,id,&t))
		return -1;
	return P2I(t);
}


/* this function computes hamming weight of given number */
static INLINE
int	count_bits(unsigned int a)
{
	unsigned int v;
	v = (a & 0x55555555) + ((a & 0xAAAAAAAA)>>1);
	v = (v & 0x33333333) + ((v & 0xCCCCCCCC)>>2);
	v = (v & 0x0F0F0F0F) + ((v & 0xF0F0F0F0)>>4);
	v = (v & 0x00FF00FF) + ((v & 0xFF00FF00)>>8);
	v = (v & 0x0000FFFF) + ((v & 0xFFFF0000)>>16);
	return v;
}

/* speedup still possible */
static
int	merge_types(sc_type *type,int patch)
{
	int res = *type | patch;
	if (count_bits(res & SC_CONSTANCY_MASK) > 1)
		return -1;
	if (count_bits(res & SC_FUZZYNESS_MASK) > 1)
		return -1;
	if (count_bits(res & SC_GENERIC_TYPE_MASK) > 1)
		return -1;
	*type = res;
	return 0;
}

/* if ref is alias finds real element's ref */
static INLINE
int	get_real_element(int ref)
{
	while (sctext[ref].alias >= 0)
		ref = sctext[ref].alias;
	return ref;
}

static INLINE
int	element_is_node(int ref)
{
	if (ref<0)
		return 0;
	return merge_types(&(sctext[get_real_element(ref)].type),SC_NODE);
}

static
int	merge_el(int ref,int type,int left,int right)
{
	ref = get_real_element(ref);

	if (merge_types(&(sctext[ref].type),type))
		return -1;

	if (sctext[ref].type & SC_ARC) {
		if (sctext[ref].beg == -1) {
			sctext[ref].beg = left;
		} else if (left != -1 && sctext[ref].beg != left)
			return -1;

		if (sctext[ref].end == -1)
			sctext[ref].end = right;
		else if (right != -1 && sctext[ref].end != right)
			return -1;
	}

	if (left != -1)
		element_is_node(sctext[ref].beg);

	return ref;
}

int	add_el_full(char *id,int type,int left,int right)
{
	int ref;

	if (sctext_size == SCTEXT_MAXSIZE)
		return -1;

	if (id) {
		if ((ref = search_by_id(id)) >= 0)
			return merge_el(ref,type,left,right);
		if (create_id_pair(id,sctext_size))
			return -1;
	}

	ref = sctext_size++;
	sctext[ref].id = id;
	sctext[ref].type = type;
	sctext[ref].beg = left;
	sctext[ref].end = right;
	sctext[ref].alias = -1;
	sctext[ref].hidden = 0;

	if (left != -1)
		element_is_node(sctext[ref].beg);

	return ref;
}

/* default type is UNDF CONST POS */
void	set_text_defaults(void)
{
	int i;
	for (i=0;i<sctext_size;i++) {
		sc_type *type;
		if (sctext[i].alias >= 0)
			continue;
		type = &(sctext[i].type);
		if (!(*type & SC_GENERIC_TYPE_MASK))
			*type |= SC_UNDF;
		if (!(*type & SC_CONSTANCY_MASK))
			*type |= SC_CONST;
		if ((*type & SC_ARC) && !(*type & SC_FUZZYNESS_MASK))
			*type |= SC_POS;
	}
}

static
int	type_subst(int prim,int second)
{
	if (!(prim & SC_GENERIC_TYPE_MASK))
		prim |= (second & SC_GENERIC_TYPE_MASK);
	if (!(prim & SC_CONSTANCY_MASK))
		prim |= (second & SC_CONSTANCY_MASK);
	if (!(prim & SC_FUZZYNESS_MASK))
		prim |= (second & SC_FUZZYNESS_MASK);
	return prim;
}

int	add_arc_attr(int ref,int con,alink *right,char *id,struct list_head *inact)
{
	int arc = add_el_full(id,(con & SC_TYPE_MASK_BASE) | SC_ARC,ref,right->ref);
	struct list_head *p;
	if (arc<0)
		return arc;
	list_for_each(p,&right->attr_list) {
		elink *e = list_entry(p,elink,elist);
		int t = add_el_full(0,
				type_subst(e->attr_con_type,
						SC_CONST|SC_POS|SC_ARC),
				e->ref,arc);
		if (t<0)
			return t;
		add_elink_ref_begin(t,inact);
	}
	return arc;
}

int	add_nonarc_con(int left,int con,int right,struct list_head *inact)
{
	int l=get_real_element(left),r=get_real_element(right);
	int i;
	
	if ((con & CON_DIR_MASK) == CON_DIR_LEFT)
		l = right, r = left;
	if (nonarcs_size >= NONARCS_MAX)
		return -1;
	for (i=0;i<nonarcs_size;i++)
		if (nonarcs[i].left == l && nonarcs[i].right == r)
			return 0;
	nonarcs[nonarcs_size].left = l;
	nonarcs[nonarcs_size++].right = r;
	return 0;
}

static	
int resolve_nonarcs_errcnt = 0;

static
void	nonarc_err(int pair,const char *str)
{
	
	resolve_nonarcs_errcnt++;
	fprintf(stderr,"non-arcs resolve error in pair %s >- %s: %s\n",
			sctext[nonarcs[pair].left].id,
			sctext[nonarcs[pair].right].id,
			str);
}

/* may be not quite optimal, but it's enough I think */
int	resolve_nonarcs(void)
{
	int res_flag,end_flag,i;

	if (!nonarcs_size)
		return 0;

	memset(leftcounts,0,sizeof(leftcounts));
	for (i=0;i<nonarcs_size;i++)
		leftcounts[nonarcs[i].left]++;

	resolve_nonarcs_errcnt = 0;
	do {
		end_flag = 1;
		res_flag = 0;
		for (i=0;i<nonarcs_size;i++) {
			int l,r,t;
			if (nonarcs[i].left<0)
				continue;
			l = nonarcs[i].left;
			r = nonarcs[i].right;
			end_flag = 0;
			if (leftcounts[l]>1)
				if (element_is_node(l)) {
					nonarc_err(i,"left member is not node, while it's used more then once from left");
					nonarcs[i].left = -1;
					continue;
				}
			t = sctext[l].type & SC_GENERIC_TYPE_MASK;
			switch (t) {
			case SC_ARC:
				if (merge_el(l,0,-1,r)<0)
					nonarc_err(i,"left member (arc) already have end");
				goto __continue;
			case SC_NODE:
				if (merge_el(r,SC_ARC,l,-1)<0) {
					if (sctext[r].beg >= 0)
						nonarc_err(i,"right member (arc) already have begin");
					else
						nonarc_err(i,"right member is not arc");
				}
				goto __continue;
			case SC_UNDF:
				nonarc_err(i,"left member is Undf");
			__continue:
				res_flag = 1;
				nonarcs[i].left = -1;
				continue;
			}
			t = sctext[r].type & SC_GENERIC_TYPE_MASK;
			/*
			 * here we know that left member is of unknown type
			 */
			if (t != SC_ARC && t) {
				res_flag = 1;
				if (merge_el(l,SC_ARC,-1,r)<0)
					nonarc_err(i,"left member (arc) already have end");
				nonarcs[i].left = -1;
			} else if (t == SC_ARC && sctext[r].beg>=0) {
				res_flag = 1;
				if (merge_el(l,SC_ARC,-1,r)<0)
					abort();
				nonarcs[i].left = -1;
			}
		}
	} while (!end_flag && res_flag);
	if (res_flag)
		return resolve_nonarcs_errcnt;

	/* here leftcounts are actually rightcounts */
	memset(leftcounts,0,sizeof(leftcounts));
	for (i=0;i<nonarcs_size;i++)
		if (nonarcs[i].left>=0)
			leftcounts[nonarcs[i].right]++;
	for (i=0;i<nonarcs_size;i++) {
		if (nonarcs[i].left < 0)
			continue;
		if ((sctext[nonarcs[i].right].type & SC_ARC) &&
					leftcounts[nonarcs[i].right]==1) {
			int l,r;
			if (leftcounts[nonarcs[i].right]>1)
				continue;
			l = nonarcs[i].left;
			r = nonarcs[i].right;
			nonarcs[i].left = -1;
			if (sctext[r].beg<0) {
				fprintf(stderr,"Warning: assuming %s is node\n",
						sctext[l].id);
				if (merge_el(r,SC_ARC,l,-1)<0)
					abort();
			}
		}
	}
	for (i=0;i<nonarcs_size;i++) {
		if (nonarcs[i].left < 0)
			continue;
		nonarc_err(i,"pair is not resolveable");
	}
	return resolve_nonarcs_errcnt;
}

int	add_con_attrs(alink *left,int con,alink *right,struct list_head *inact)
{
	int _con;
	if ((con & CON_TYPE_MASK) != CON_TYPE_ARC)
		return add_nonarc_con(left->ref,con,right->ref,inact);
	_con = con & CON_DIR_MASK;
	if (_con == CON_DIR_RIGHT)
		return add_arc_attr(left->ref,con,right,0,inact);
	return add_arc_attr(right->ref,con,left,0,inact);
}

int	add_node(char *id,int type)
{
	type = (type & SC_TYPE_MASK_BASE) | SC_NODE;
	return add_el_full(id,type,-1,-1);
}

int	add_arc(int left,int con,int right,char *id)
{
	con = (con & SC_TYPE_MASK_BASE) | SC_ARC;
	return add_el_full(id,con,left,right);
}

int	add_el(char *id,int type)
{
	return add_el_full(id,type,-1,-1);
}

int	add_alias(char *id,int ref)
{
	int _t;
	sc_type aliastype;

	while (sctext[ref].alias>=0)
		ref = sctext[ref].alias;

	if ((_t = search_by_id(id))>=0 && _t == ref)
		return 0;
	/*
	 * TODO: give warning if we force element with id. be alias
	 */
	/*
	 * if there is another element with given id
	 * we must make real alias
	 */
	if (_t >= 0) {
		if (IS_SPECIAL_ELEMENT(ref) || IS_SPECIAL_ELEMENT(_t)) {
			fprintf(stderr,"Error: special elements cannot be merged\n");
			return -1;
		}
		while (sctext[_t].alias>=0)
			_t = sctext[_t].alias;

		if (_t == ref)
			return 0;

		if (sctext[ref].id && sctext[ref].id[0] && sctext[ref].id[0] != '$') {
			int t = ref;
			ref = _t;
			_t = t;
		}
		/*fprintf(stderr,"Dangerous alias %s = %s\n",id,sctext[ref].id);*/
		if (merge_el(_t,sctext[ref].type,sctext[ref].beg,sctext[ref].end)<0) {
			fprintf(stderr,"Cannot merge two elements with idtf %s\n",id);
			return -1;
		}

        sctext[ref].alias = _t;
		
        if (sctext[ref].cont_type == CONT_NONE)
			return 0;
		if (sctext[_t].cont_type != CONT_NONE) {
			fprintf(stderr,"Cannot merge content\n");
			return -1;
		}
		sctext[_t].cont_type = sctext[ref].cont_type;
		sctext[_t].str = sctext[ref].str;
		sctext[_t].real = sctext[ref].real;
		sctext[_t].datalen = sctext[ref].datalen;
		return 0;
	}

	aliastype = SC_CONST;
	if (id[0] == '_' || !strncmp(id,"$_",2)) {
		if (!strncmp(id,"__",2) || !strncmp(id,"$__",3))
			aliastype = SC_METAVAR;
		else
			aliastype = SC_VAR;
	}
	if (!do_not_complain_constancy && sctext[ref].type & (SC_CONSTANCY_MASK - aliastype)) {
		char *str = "CONST";
		if (aliastype == SC_VAR)
			str = "VAR";
		else if (aliastype == SC_METAVAR)
			str = "METAVAR";
		fprintf(stderr,"Error: cannot make %s alias to element with other constancy\n",str);
		return -1;
	}
	if (!sctext[ref].id || sctext[ref].id[0] == '$')
		sctext[ref].id = id;
	return create_id_pair(id,ref);
}

int	add_cont(int ref,char *text,int cont_type)
{
	while (sctext[ref].alias>=0)
		ref = sctext[ref].alias;
	sctext[ref].cont_type = cont_type;
	switch (cont_type) {
	case CONT_NONE:
		return 0;
	case CONT_STRING:
	case CONT_FILE:
	case CONT_DATA:
		sctext[ref].str = text;
		break;
	case CONT_NUMERIC:
		{
			double t;
			if (sscanf(text,"%lg",&t)<1)
				return -1;
			sctext[ref].real = t;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

void	init_special_nodes()
{
	int rv;
	assert(sctext_size == 0);
	rv = add_el_full("@node",SC_NODE|SC_CONST,-1,-1);
	assert(rv == NODE_SET);
	rv = add_el_full("@arc",SC_NODE|SC_CONST,-1,-1);
	assert(rv == ARC_SET);
	rv = add_el_full("@undf",SC_NODE|SC_CONST,-1,-1);
	assert(rv == UNDF_SET);
	rv = add_el_full("@const",SC_NODE|SC_CONST,-1,-1);
	assert(rv == CONST_SET);
	rv = add_el_full("@var",SC_NODE|SC_CONST,-1,-1);
	assert(rv == VAR_SET);
	rv = add_el_full("@metavar",SC_NODE|SC_CONST,-1,-1);
	assert(rv == METAVAR_SET);
	rv = add_el_full("@pos",SC_NODE|SC_CONST,-1,-1);
	assert(rv == POS_SET);
	rv = add_el_full("@neg",SC_NODE|SC_CONST,-1,-1);
	assert(rv == NEG_SET);
	rv = add_el_full("@fuz",SC_NODE|SC_CONST,-1,-1);
	assert(rv == FUZ_SET);
}

static
void	propagate_hidden()
{
	int i;
	int flag;
	do {
		flag = 0;
		for (i=0;i<sctext_size;i++) {
			if (sctext[i].hidden || !(sctext[i].type & SC_ARC))
				continue;
			if ((sctext[i].beg >= 0 && sctext[sctext[i].beg].hidden) ||
					(sctext[i].end >= 0 && sctext[sctext[i].end].hidden)) {
				sctext[i].hidden = 1;
				flag = 1;
			}
		}
	} while (flag);
}

int	handle_special_nodes()
{
	int i;
	static struct {
		int type;
		int mask;
	} types[] = {
		/* order here is important ! */
		/* NODE_SET */	{SC_NODE, SC_GENERIC_TYPE_MASK},
		/* ARC_SET */	{SC_ARC, SC_GENERIC_TYPE_MASK},
		/* UNDF_SET */	{SC_UNDF, SC_GENERIC_TYPE_MASK},
		/* CONST_SET */	{SC_CONST, SC_CONSTANCY_MASK},
		/* VAR_SET */	{SC_VAR, SC_CONSTANCY_MASK},
		/* METAVAR_SET */{SC_METAVAR, SC_CONSTANCY_MASK},
		/* POS_SET */	{SC_POS, SC_FUZZYNESS_MASK},
		/* NEG_SET */	{SC_NEG, SC_FUZZYNESS_MASK},
		/* FUZ_SET */	{SC_FUZ, SC_FUZZYNESS_MASK}
	};
	for (i=TYPE_NODES_START;i<TYPE_NODES_END;i++)
		sctext[i].hidden = 1;
	for (i=0;i<sctext_size;i++) {
		int ref,beg,t;
		if (sctext[i].alias >=0 || !(sctext[i].type & SC_ARC))
			continue;
		beg = sctext[i].beg;
		if (beg<0 || !IS_SPECIAL_ELEMENT(beg))
			continue;
		ref = sctext[i].end;
		if (ref < 0) {
			fprintf(stderr,"Warning: arc from %s but with no end\n",sctext[beg].id);
			continue;
		}
		t = sctext[ref].type & types[beg-TYPE_NODES_START].mask;
		if (t && t != types[beg-TYPE_NODES_START].type) {
			fprintf(stderr,"Error: element %s current type (%d) is incompatible with type %s\n",sctext[ref].id,t,sctext[beg].id);
			return -1;
		}
		sctext[ref].type |= types[beg-TYPE_NODES_START].type;
		sctext[i].hidden = 1;
	}
	propagate_hidden();
	return 0;
}
