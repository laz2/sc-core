/*
 *  SCPx processing module project.
 *
 *  tgfmap.c - map utility for libTGF
 *
 *  Created at Mon 07 May 2001 01:10:05 AM EEST by ALK
 *
 */
#include "tgfmap.h"

#include <stdlib.h>

#define	P2I(ptr)	((int)ptr)
#define I2P(i)		((void *)i)

typedef	int	(*map_comp_fn)(const void *a,const void *b);

static	int	map_comp_int(const void *a,const void *b);
static	int	map_comp_ptr(const void *a,const void *b);

typedef struct	_map_node {
	struct _map_node *left,*right,*parent;
	int balance;
	void *key;
	void *value;
} map_node;

struct	_map {
	map_comp_fn	comp_fn;
	map_node	root;
};
typedef struct _map map;

static	map_node *map_node_new(void *key,void *value,map_node *parent);
static	void	map_node_destroy(map_node *);

static	map	*map_new(map_comp_fn);
static	void	map_destroy(map *);

static	int	map_insert(map *,void *key,void *value);
static	int	map_find(map *,void *key,void **value);
/* Note: This functions doesn't care about memory deallocation for key/values*/
/*static	int	map_delete(map *,void *key);*/
/*static	int	map_delete_node(map_node *);*/
/*static	int	map_delete_all(map *);*/

tgf_map_in *tgf_map_in_new(void)
{
	return map_new(map_comp_int);
}

int	tgf_map_in_add(tgf_map_in *m,int key,void *value)
{
	return map_insert((map *)m,I2P(key),value);
}

int	tgf_map_in_lookup(tgf_map_in *m,int key,void **value)
{
	return map_find((map *)m,I2P(key),value);
}

void	tgf_map_in_destroy(tgf_map_in *m)
{
	map_destroy((map *)m);
}

tgf_map_out *tgf_map_out_new(void)
{
	return map_new(map_comp_ptr);
}

int	tgf_map_out_add(tgf_map_out *m,void *key,int value)
{
	return map_insert((map *)m,key,I2P(value));
}

int	tgf_map_out_lookup(tgf_map_out *m,void *key,int *value)
{
	return map_find((map *)m,key,(void **)value);
}

void	tgf_map_out_destroy(tgf_map_out *m)
{
	map_destroy((map *)m);
}

#if defined(CATCH_MAP_BUGS)

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define	MAP_BUG_PRINT() \
	fprintf(stderr,"Map bug in %s:%s at line %d\n", \
			__FILE__,__FUNCTION__,__LINE__); \
	abort();
#else
#define	MAP_BUG_PRINT() \
	fprintf(stderr,"Map bug in file %s at line %d\n", \
			__FILE__,__LINE__); \
	abort();
#endif /* __GNUC__ && !__STRICT_ANSI__ */

#define MAP_BUG() \
	do { \
		MAP_BUG_PRINT() \
	} while(0)

#else
#define MAP_BUG() do {} while(0)
#endif /* CATCH_MAP_BUGS */



int	map_comp_int(const void *a,const void *b)
{
	return P2I(a)-P2I(b);
}

int	map_comp_ptr(const void *a,const void *b)
{
	return a>b?1:a==b?0:-1;
}

map_node *map_node_new(void *key,void *value,map_node *parent)
{
	map_node *p;
	p=(map_node *)malloc(sizeof(map_node));
	if (!p)
		return p;
	p->balance=0;
	p->value = value;
	p->key = key;
	p->left = p->right = NULL;
	p->parent = parent;
	return p;
}

void	map_node_destroy(map_node *p)
{
	free(p);
}

/* returns true if depth of balanced node decremented and false otherwise */
/* Note that in case of tree insertion balancing always decrements height */
static
int	map_do_balance(map_node *node)
{
	map_node **pp,*p,*q;
	int rv=1;
	if (node->parent->left == node)
		pp = &(node->parent->left);
	else
		pp = &(node->parent->right);
	if (node->balance>0) {
		p = node->right;
		if (p->balance>=0) {
			rv = p->balance;
			*pp = p;

			node->right = p->left;
			if (p->left)
				p->left->parent = node;

			node->balance = !p->balance;
			p->balance=-node->balance;

			p->left = node;
			p->parent = node->parent;
			node -> parent = p;
		} else {
			q = p->left;
			p->balance = q->balance>=0?0:1;
			node->balance = q->balance<=0?0:-1;
			q->balance = 0;

			q->parent = node->parent;
			*pp = q;
			p->parent = node->parent = q;
			node->right = q->left;
			if (node->right)
				node->right->parent=node;
			p->left = q->right;
			if (p->left)
				p->left->parent = p;
			q->left = node;
			q->right = p;
		}
	} else {
		p = node->left;
		if (p->balance<=0) {
			rv = p->balance;
			*pp = p;

			node->left = p->right;
			if (p->right)
				p->right->parent = node;

			node->balance = -(!p->balance);
			p->balance=-node->balance;

			p->right = node;
			p->parent = node->parent;
			node -> parent = p;
		} else {
			q = p->right;
			p->balance = q->balance<=0?0:-1;
			node->balance = q->balance>=0?0:1;
			q->balance = 0;

			q->parent = node->parent;
			*pp = q;
			p->parent = node->parent = q;
			node->left = q->right;
			if (node->left)
				node->left->parent=node;
			p->right = q->left;
			if (p->right)
				p->right->parent = p;
			q->right = node;
			q->left = p;
		}
	}
	return rv;
}

int	map_insert(map *m,void *key,void *value)
{
	map_node *p=m->root.left,*q;
	int cr;
	if (!p) {
		m->root.left = map_node_new(key,value,&(m->root));
		return 1;
	}
	while ((cr = m->comp_fn(key,p->key))) {
		if (cr>0) {
			if (!p->right) {
				q = map_node_new(key,value,p);
				p->right = q;
				if (++p->balance)
					goto do_balance;
				return 1;
			}
			p = p->right;
		} else {
			if (!p->left) {
				q = map_node_new(key,value,p);
				p->left = q;
				if (--p->balance)
					goto do_balance;
				return 1;
			}
			p = p->left;
		}
	}
	return 0;
do_balance:
	do {
		q = p->parent;
		if (!q->parent)
			return 1;
		if (q->left == p) {
			q->balance--;
			if (q->balance<-2)
				MAP_BUG();
			if (q->balance==-2) {
				if (map_do_balance(q))
					break;
				MAP_BUG();
			}
		} else {
			q->balance++;
			if (q->balance>2)
				MAP_BUG();
			if (q->balance==2) {
				if (map_do_balance(q))
					break;
				MAP_BUG();
			}
		}
		p = q;
	} while (q->balance);
	return 1;
}

static
void	map_remove_leaf_node(map_node *node)
{
	map_node *p,*q;
	int bal;

	p = node;
	q = p->parent;
	while (q->parent) {
		if (q->right == p) {
			bal = --q->balance;
			if (bal < -2)
				MAP_BUG();
			if (bal == -1)
				break;
		} else {
			bal = ++q->balance;
			if (bal > 2)
				MAP_BUG();
			if (bal == 1)
				break;
		}
		p = q;
		q = q->parent;
		if (bal == 2 || bal == -2)
			if (!map_do_balance(p))
				break;
	}
	if (node->parent->left == node)
		node->parent->left = NULL;
	else
		node->parent->right = NULL;
	node->left = node->right = node->parent = NULL;
	node->balance = 0;
}

/* probably, not the best algorithm */
static
void	map_remove_node(map_node *node)
{
	map_node *p,*q;
	if (!node->left && !node->right) {
		map_remove_leaf_node(node);
		return;
	}
	if (node->balance>=0 && node->right) {
	get_from_right:
		p=node->right;
		while ((q = p->left))
			p = q;
	} else {
		if (!node->left)
			goto get_from_right;
		p=node->left;
		while ((q = p->right))
			p = q;
	}
	map_remove_node(p);
	p->left = node->left;
	if (p->left)
		p->left->parent = p;
	p->right = node->right;
	if (p->right)
		p->right->parent = p;
	p->balance = node->balance;
	p->parent = node->parent;
	if (node->parent->left == node)
		node->parent->left = p;
	else
		node->parent->right = p;
	node->left = node->right = node -> parent = NULL;
	node->balance = 0;
}

static
map_node *map_find_node(map *m,void *key)
{
	map_node *p=m->root.left;
	int cr;
	while (p && (cr = m->comp_fn(key,p->key)))
		if (cr>0)
			p = p->right;
		else
			p = p->left;
	return p;
}

int	map_find(map *m,void *key,void **value)
{
	map_node *p=map_find_node(m,key);
	if (!p)
		return 0;
	*value = p->value;
	return 1;
}

#ifdef __GNUC__
static
int	map_delete_node(map_node *node) __attribute__((unused));
#endif

/* Note: This functions doesn't care about memory deallocation for key/values*/
static
int	map_delete_node(map_node *node)
{
	if (!node)
		return 0;
	map_remove_node(node);
	map_node_destroy(node);
	return 1;
}

/* This comment is to not confuse GCC with unused static function 
int	map_delete(map *m,void *key)
{
	map_node *p = map_find_node(m,key);
	return map_delete_node(p);
}*/


static
int	map_delete_tree(map_node *node)
{
	if (!node)
		return 0;
	if (node->left)
		map_delete_tree(node->left);
	if (node->right)
		map_delete_tree(node->right);
	map_node_destroy(node);
	return 1;
}

static
int	map_delete_all(map *m)
{
	map_delete_tree(m->root.left);
	return 1;
}

map	*map_new(map_comp_fn fn)
{
	map *p=(map *)malloc(sizeof(map));
	if (p) {
		p->root.left=p->root.right=p->root.parent=NULL;
		p->root.balance=0;
		p->comp_fn = fn;
	}
	return p;
}

void	map_destroy(map *p)
{
	map_delete_all(p);
	free(p);
}
