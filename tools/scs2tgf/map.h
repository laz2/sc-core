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
 * @file map.h
 * @author ALK
 * @brief utility to associate on kind of objects with another.
 *
 * This utility uses AVL tree to order pairs.
 */

#ifndef __TGFMAP_H__
#define __TGFMAP_H__

struct _map;
typedef struct _map map;

#define	P2I(ptr)	((int)(long)(ptr))
#define I2P(i)		((void *)(long)(i))

typedef	int	(*map_comp_fn)(const void *a,const void *b);

int	map_comp_int(const void *a,const void *b);
int	map_comp_ptr(const void *a,const void *b);

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

map_node *map_node_new(void *key,void *value,map_node *parent);
void	map_node_destroy(map_node *);

map	*map_new(map_comp_fn);
void	map_destroy(map *);

int	map_insert(map *,void *key,void *value);
int	map_find(map *,void *key,void **value);
/* Note: This functions doesn't care about memory deallocation for key/values*/
int	map_delete(map *,void *key);
int	map_delete_node(map_node *);
int	map_delete_all(map *);

#endif /* __TGFMAP_H__ */
