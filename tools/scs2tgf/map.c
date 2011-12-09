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
 * @file map.c
 * @brief utility to associate on kind of objects with another.
 * @author ALK
 *
 * This utility uses AVL (balanced) tree to order pairs.
 */

#include <stdlib.h>
#include <stdio.h>

#include "map.h"

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
#endif				/* CATCH_MAP_BUGS */

int map_comp_int(const void *a, const void *b)
{
	return P2I(a) - P2I(b);
}

int map_comp_ptr(const void *a, const void *b)
{
	return a > b ? 1 : a == b ? 0 : -1;
}

map_node *map_node_new(void *key, void *value, map_node * parent)
{
	map_node *p;
	p = (map_node *) malloc(sizeof(map_node));
	if (!p)
		return p;
	p->balance = 0;
	p->value = value;
	p->key = key;
	p->left = p->right = NULL;
	p->parent = parent;
	return p;
}

void map_node_destroy(map_node * p)
{
	free(p);
}

/* returns true if depth of balanced node decremented and false otherwise */
/* Note that in case of tree insertion balancing always decrements height */
int map_do_balance(map_node * node)
{
	map_node **pp, *p, *q;
	int rv = 1;
	if (node->parent->left == node)
		pp = &(node->parent->left);
	else
		pp = &(node->parent->right);
	if (node->balance > 0) {
		p = node->right;
		if (p->balance >= 0) {
			rv = p->balance;
			*pp = p;

			node->right = p->left;
			if (p->left)
				p->left->parent = node;

			node->balance = !p->balance;
			p->balance = -node->balance;

			p->left = node;
			p->parent = node->parent;
			node->parent = p;
		} else {
			q = p->left;
			p->balance = q->balance >= 0 ? 0 : 1;
			node->balance = q->balance <= 0 ? 0 : -1;
			q->balance = 0;

			q->parent = node->parent;
			*pp = q;
			p->parent = node->parent = q;
			node->right = q->left;
			if (node->right)
				node->right->parent = node;
			p->left = q->right;
			if (p->left)
				p->left->parent = p;
			q->left = node;
			q->right = p;
		}
	} else {
		p = node->left;
		if (p->balance <= 0) {
			rv = p->balance;
			*pp = p;

			node->left = p->right;
			if (p->right)
				p->right->parent = node;

			node->balance = -(!p->balance);
			p->balance = -node->balance;

			p->right = node;
			p->parent = node->parent;
			node->parent = p;
		} else {
			q = p->right;
			p->balance = q->balance <= 0 ? 0 : -1;
			node->balance = q->balance >= 0 ? 0 : 1;
			q->balance = 0;

			q->parent = node->parent;
			*pp = q;
			p->parent = node->parent = q;
			node->left = q->right;
			if (node->left)
				node->left->parent = node;
			p->right = q->left;
			if (p->right)
				p->right->parent = p;
			q->right = node;
			q->left = p;
		}
	}
	return rv;
}

int map_insert(map * m, void *key, void *value)
{
	map_node *p = m->root.left, *q;
	int cr;
	if (!p) {
		m->root.left = map_node_new(key, value, &(m->root));
		return 1;
	}
	while ((cr = m->comp_fn(key, p->key))) {
		if (cr > 0) {
			if (!p->right) {
				q = map_node_new(key, value, p);
				p->right = q;
				if (++p->balance)
					goto do_balance;
				return 1;
			}
			p = p->right;
		} else {
			if (!p->left) {
				q = map_node_new(key, value, p);
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
			if (q->balance < -2)
				MAP_BUG();
			if (q->balance == -2) {
				if (map_do_balance(q))
					break;
				MAP_BUG();
			}
		} else {
			q->balance++;
			if (q->balance > 2)
				MAP_BUG();
			if (q->balance == 2) {
				if (map_do_balance(q))
					break;
				MAP_BUG();
			}
		}
		p = q;
	} while (q->balance);
	return 1;
}

void map_remove_leaf_node(map_node * node)
{
	map_node *p, *q;
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
void map_remove_node(map_node * node)
{
	map_node *p, *q;
	if (!node->left && !node->right) {
		map_remove_leaf_node(node);
		return;
	}
	if (node->balance >= 0 && node->right) {
	      get_from_right:
		p = node->right;
		while ((q = p->left))
			p = q;
	} else {
		if (!node->left)
			goto get_from_right;
		p = node->left;
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
	node->left = node->right = node->parent = NULL;
	node->balance = 0;
}

map_node *map_find_node(map * m, void *key)
{
	map_node *p = m->root.left;
	int cr;
	while (p && (cr = m->comp_fn(key, p->key)))
		if (cr > 0)
			p = p->right;
		else
			p = p->left;
	return p;
}

int map_find(map * m, void *key, void **value)
{
	map_node *p = map_find_node(m, key);
	if (!p)
		return 0;
	*value = p->value;
	return 1;
}

/* Note: This functions doesn't care about memory deallocation for key/values*/
int map_delete_node(map_node * node)
{
	if (!node)
		return 0;
	map_remove_node(node);
	map_node_destroy(node);
	return 1;
}

int map_delete(map * m, void *key)
{
	map_node *p = map_find_node(m, key);
	return map_delete_node(p);
}

int map_delete_tree(map_node * node)
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

int map_delete_all(map * m)
{
	map_delete_tree(m->root.left);
	return 1;
}

map *map_new(map_comp_fn fn)
{
	map *p = (map *) malloc(sizeof(map));
	if (p) {
		p->root.left = p->root.right = p->root.parent = NULL;
		p->root.balance = 0;
		p->comp_fn = fn;
	}
	return p;
}

void map_destroy(map * p)
{
	map_delete_all(p);
	free(p);
}
