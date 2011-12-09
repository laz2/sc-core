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
 * @file parser_func.c
 * @author ALK
 * @brief Parser functionality.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "list.h"
#include "values.h"
#include "sctext.h"

#include "misc.h" /* for my_dirname */

#define UNIFY_HASH_SIZE 65536

struct hash_entry {
	struct list_head list;
	int ref;
};

struct list_head unify_hash[UNIFY_HASH_SIZE];

static
void	init_unify_hash()
{
	int i;
	for (i=0;i<UNIFY_HASH_SIZE;i++)
		INIT_LIST_HEAD(unify_hash+i);
}

static
int	unify_hash_add(struct list_head *el)
{
	int ref = list_entry(el,elink,elist)->ref;
	struct list_head *bucket = unify_hash+ref%UNIFY_HASH_SIZE;
	struct list_head *p;
	struct hash_entry *e;
	list_for_each(p,bucket) {
		e = list_entry(p,struct hash_entry,list);
		if (e->ref == ref)
			return 0;
	}
	e = malloc(sizeof(*e));
	INIT_LIST_HEAD(&e->list);
	e->ref = ref;
	list_add_tail(bucket,&e->list);
	return 1;
}

/* eliminates duplicated values */
/* duplicated are searched with hash help */
int	elist_unify(struct list_head *head)
{
	struct list_head *p,*next;
	init_unify_hash();
	p = head->next;
	while (p != head) {
		next = p->next;
		if (!unify_hash_add(p)) {
			list_del(p);
			free(p);
		}
		p = next;
	}
	return 0;
}

INLINE
void	list_add_unify(struct list_head *new,struct list_head *head)
{
	struct list_head *pos;
	int nref = list_entry(new,elink,elist)->ref;
	while (sctext[nref].alias >= 0)
		nref = sctext[nref].alias;
	list_for_each(pos,head) {
		int ref = list_entry(pos,elink,elist)->ref;
		while (sctext[ref].alias>=0)
			ref = sctext[ref].alias;
		if (ref	== list_entry(new,elink,elist)->ref)
			return;
	}
	list_add(new,head);
}

/* also N*N but faster than list_splice ; elist_unify sequence
*/
INLINE
void	list_splice_unify(struct list_head *list,struct list_head *head)
{
	struct list_head *p = list->prev,*next;

	while (p != list) {
		next = p->prev;
		list_add_unify(p,head);
		p = next;
	}
}

/* adds all information in act to inact elist */
/* destroys act list. does not free head */
void	actives_to_inactives(struct list_head *act,struct list_head *inact)
{
	struct list_head *p,*next;
	for (p = act->next ; p != act ; p = next) {
		next = p->next;
/*		list_splice_unify(&list_entry(p,alink,alist)->attr_list,inact);
		list_add_unify(p,inact);*/
		list_splice(&list_entry(p,alink,alist)->attr_list,inact);
		list_add_tail(p,inact);
	}
	/*elist_unify(inact);*/
}

slink *	slink_new(char *str)
{
	slink *p = malloc(sizeof(slink));
	if (!p)
		return p;
	p->str = str;
	INIT_LIST_HEAD(&p->slist);
	return p;
}

alink * alink_new(int ref)
{
	alink *p = malloc(sizeof(alink));
	if (!p)
		return p;
	p->ref = ref;
	INIT_LIST_HEAD(&p->alist);
	INIT_LIST_HEAD(&p->attr_list);
	return p;
}

void	text_init(textvalue *text)
{
	INIT_LIST_HEAD(&text->active_list);
	INIT_LIST_HEAD(&text->inact_list);
}

int	text_new(textvalue **text)
{
	textvalue *p;
	if (!text)
		return 1;
	p = malloc(sizeof(textvalue));
	if (!p)
		return 1;
	INIT_LIST_HEAD(&p->active_list);
	INIT_LIST_HEAD(&p->inact_list);
	*text = p;
	return 0;
}

void	text_move(textvalue *oldtext,textvalue *newtext)
{
	list_new_head(&oldtext->active_list,&newtext->active_list);
	list_new_head(&oldtext->inact_list,&newtext->inact_list);
	free(oldtext);
}

void	text_add(textvalue *oldtext,textvalue *newtext)
{
	list_splice(&oldtext->active_list,&newtext->active_list);
	list_splice(&oldtext->inact_list,&newtext->inact_list);
	free(oldtext);
}

int	rule_text(textvalue *text,textvalue *sent)
{
	list_splice(&sent->inact_list,&text->inact_list);
	actives_to_inactives(&text->active_list,&text->inact_list);
	list_new_head(&sent->active_list,&text->active_list);
	free(sent);
	return 0;
}

/* idents are added to sent. Some structures also made and added of course */
/* left to right direction is from sent to idents */
int	rule_sent(textvalue *sent,textvalue *idents,
				con_type con)
{
	struct list_head *p,*q;
	list_splice(&idents->inact_list,&sent->inact_list);
	list_for_each(p,&sent->active_list) {
		list_for_each(q,&idents->active_list) {
			elink *new = malloc(sizeof(elink));
			if (!new)
				return -1;
			INIT_LIST_HEAD(&new->elist);
			new->ref = add_con_attrs(
					list_entry(p,alink,alist),
					con,
					list_entry(q,alink,alist),
					&sent->inact_list);
			if (new->ref<0)
				return -1;
			if ((con & CON_TYPE_MASK) == CON_TYPE_ARC)
				list_add(&new->elist,&sent->inact_list);
		}
	}
	actives_to_inactives(&sent->active_list,&sent->inact_list);
	list_new_head(&idents->active_list,&sent->active_list);
	free(idents);
	return 0;
}

/* term becomes ident */
int	rule_ident(slink *aliases,textvalue *attrs,textvalue *term)
{
	if (attrs) {
		list_splice(&attrs->active_list,
				&list_entry(term->active_list.next,alink,alist)->attr_list);
		list_splice_unify(&attrs->inact_list,&term->inact_list);
		free(attrs);
	}

	if (aliases) {
		struct list_head *p,*q = aliases->slist.prev,*next;
		int ref = list_entry(term->active_list.next,alink,alist)->ref;
		p = q;
		do {
			next = p->prev;
			if (add_alias(list_entry(p,slink,slist)->str,ref))
				return -1;
			/*free(list_entry(p,slink,slist)->str);*/
			free(list_entry(p,slink,slist));
			p = next;
		} while (p != q);
	}

	return 0;
}

/* keeps order in which aliases are written */
int	rule_aliases(slink *aliases,char *alias)
{
	slink *p = slink_new(alias);
	if (!p)
		return -1;
	list_add(&p->slist,aliases->slist.prev);
	return 0;
}

int	rule_term_id(char *id,textvalue **term)
{
	alink *p;
	int ref;
	int type = 0;

	if (!strncmp(id,"_",1) || !strncmp(id,"$_",2)) {
		if (!strncmp(id,"__",2) || !strncmp(id,"$__",3))
			type = SC_METAVAR;
		else
			type = SC_VAR;
	}
	ref = add_el(id,type);
	if (ref<0)
		return -1;
	if (!(p = alink_new(ref)))
		return -1;
	if (text_new(term)) {
		free(p);
		return -1;
	}
	list_add(&p->alist,&(*term)->active_list);
	return 0;
}

int	rule_term_strcont(char *str,textvalue **term)
{
	alink *p;
	int ref;

	ref = add_el(0,0);
	if (ref<0)
		return -1;
	if (add_cont(ref,str,CONT_STRING)<0)
		return -1;
	if (!(p = alink_new(ref)))
		return -1;
	if (text_new(term)) {
		free(p);
		return -1;
	}
	list_add(&p->alist,&(*term)->active_list);
	return 0;
}

static
unsigned char base64_table[] = {
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	62,	128,	128,	128,	63,
52,	53,	54,	55,	56,	57,	58,	59,
60,	61,	128,	128,	128,	0,	128,	128,
128,	0,	1,	2,	3,	4,	5,	6,
7,	8,	9,	10,	11,	12,	13,	14,
15,	16,	17,	18,	19,	20,	21,	22,
23,	24,	25,	128,	128,	128,	128,	128,
128,	26,	27,	28,	29,	30,	31,	32,
33,	34,	35,	36,	37,	38,	39,	40,
41,	42,	43,	44,	45,	46,	47,	48,
49,	50,	51,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128,
128,	128,	128,	128,	128,	128,	128,	128
};

static
unsigned char *	unpack_base64(char *_input,int len,int *outlen)
{
	unsigned char *res = malloc(len);
	unsigned char *out = res;
	unsigned char *input = (unsigned char *)_input;
	if (!res)
		return res;
	while (len>0) {
		unsigned c;
		unsigned char chars[4];
		int i;
		unsigned num=0;
		unsigned char shift=18;
		for (i=0;i<4;i++) {
			do {
				if (len <= 0) {
					if (i == 0)
						goto out;
					goto err;
				}
				c = *input++;
				len--;
			} while (isspace(c));
			chars[i] = c;
			c = base64_table[c];
			if (c & 0x80)
				goto err;
			num |= c << shift;
			shift -= 6;
		}
		*out++ = (num>>16);
		if (chars[2] != '=')
			*out++ = (num>>8);
		if (chars[3] != '=')
			*out++ = (num);
	}
out:
	*outlen = out-res;
	return res;
err:
	free(res);
	return 0;
}

int	rule_term_base64cont(char *str,textvalue **term)
{
	int ref = add_el(0,0);
	int len;
	unsigned char *res;
	int reslen;
	alink *p;

	if (ref<0)
		return -1;
	len = strlen(str);
	res = unpack_base64(str,len,&reslen);
	if (!res)
		return -1;
	if (add_cont(ref,(char *)res,CONT_DATA)<0) {
		free(res);
		return -1;
	}
	sctext[ref].datalen = reslen;
	if (!(p = alink_new(ref))) {
		free(res);
		return -1;
	}
	if (text_new(term)) {
		free(res);
		free(p);
		return -1;
	}
	list_add(&p->alist,&(*term)->active_list);
	return 0;
}

int	rule_term_number(char *str,textvalue **term)
{
	int ref = add_el(0,0);
	alink *p;

	if (ref<0)
		return -1;
	if (add_cont(ref,str,CONT_NUMERIC)<0)
		return -1;
	if (!(p = alink_new(ref)))
		return -1;
	if (text_new(term)) {
		free(p);
		return -1;
	}
	list_add(&p->alist,&(*term)->active_list);
	return 0;
}

/* must add to begin of list. rule_term_text depends on it */
int	add_elink_ref_begin(int ref,struct list_head *head)
{
	elink *p = malloc(sizeof(elink));
	if (!p)
		return -1;
	list_add(&p->elist,head);
	p->ref = ref;
	return 0;
}

/* text becomes term */
int	rule_term_text(textvalue *text,int constancy)
{
	alink *p;
	int ref;
	struct list_head *q;

	ref = add_node(0,constancy);
	if (ref<0)
		return -1;
	if (!(p = alink_new(ref)))
		return -1;

	constancy |= SC_POS;

	actives_to_inactives(&text->active_list,&text->inact_list);
	elist_unify(&text->inact_list);

	list_for_each(q,&text->inact_list) {
		int t = add_arc(ref,constancy,list_entry(q,elink,elist)->ref,0);
		if (t<0)
			return -1;
		if (add_elink_ref_begin(t,&text->inact_list))
			return -1;
	}

	INIT_LIST_HEAD(&text->active_list);
	list_add(&p->alist,&text->active_list);

	return 0;
}

/* set becomes term */
int	rule_term_set(textvalue *set,int constancy)
{
	int ref;
	struct list_head *p;
	alink *ap;

	ref = add_node(0,constancy);
	if (ref<0)
		return -1;
	ap = alink_new(ref);
	if (!ap)
		return -1;
	constancy |= SC_POS;

	list_for_each (p,&set->active_list) {
		int t = add_arc_attr(ref,constancy,list_entry(p,alink,alist),0,&set->inact_list);
		if (t<0)
			return -1;
		if (add_elink_ref_begin(t,&set->inact_list))
			return -1;
	}
	actives_to_inactives(&set->active_list,&set->inact_list);
	INIT_LIST_HEAD(&set->active_list);
	list_add(&ap->alist,&set->active_list);
	return 0;
}

/* no special checks yet */
int	rule_term_arc(textvalue *id1,con_type con,
				textvalue *id2,textvalue **term)
{
	int ref;
	alink *p;
	
	if (text_new(term))
		return -1;

	list_splice_unify(&id1->inact_list,&(*term)->inact_list);
	list_splice_unify(&id2->inact_list,&(*term)->inact_list);
	ref = add_con_attrs(
			list_entry(id1->active_list.next,alink,alist),
			con,
			list_entry(id2->active_list.next,alink,alist),
			&(*term)->inact_list);
	if (ref<0)
		return -1;
	if (!(p = alink_new(ref))) {
		free(term);
		return -1;
	}
	actives_to_inactives(&id1->active_list,&(*term)->inact_list);
	actives_to_inactives(&id2->active_list,&(*term)->inact_list);
	list_add(&p->alist,&(*term)->active_list);
	free(id1);
	free(id2);
	return 0;
}

int	assign_cont(textvalue *term,char *text,int cont_type)
{
	int element = list_entry(term->active_list.next,alink,alist)->ref;
	if (cont_type == CONT_FILE) {
		char *path,*path2,*res;
		path = strdup(cfilename);
		path2 = my_dirname(path);
		res = malloc(strlen(path2) + strlen(text) + 2);
		strcpy(res,path2);
		strcat(res,"/");
		strcat(res,text);
		text = res;
		free(path);
	}
	return add_cont(element,text,cont_type);
}
