
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


/*
 *  SCPx processing module project.
 *
 *  map.h - utility to associate on kind of objects with another.
 *
 *  This utility uses AVL (balanced) tree to order pairs.
 *
 *  Created at Sat 05 May 2001 09:57:47 PM EEST by ALK
 *
 */

#ifndef __TGFMAP_H__
#define __TGFMAP_H__

/* maps int to void * */
typedef	void tgf_map_in;
/* maps void * to int */
typedef	void tgf_map_out;

tgf_map_in *tgf_map_in_new(void);
int	tgf_map_in_add(tgf_map_in *,int,void *);
int	tgf_map_in_lookup(tgf_map_in *,int,void **);
void	tgf_map_in_destroy(tgf_map_in *);

tgf_map_out *tgf_map_out_new(void);
int	tgf_map_out_add(tgf_map_out *,void *,int);
int	tgf_map_out_lookup(tgf_map_out *,void *,int *);
void	tgf_map_out_destroy(tgf_map_out *);

#endif /* __TGFMAP_H__ */
