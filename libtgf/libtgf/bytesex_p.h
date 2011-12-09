
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
 *  bytesex.h - byteorder conversions.
 *
 *  Created at Thu 03 May 2001 12:53:17 AM EEST by ALK
 *
 */
#ifndef __BYTESEX_H__
#define __BYTESEX_H__

/* here we assume some platform with BSD'ish byte conversions */
#if defined(TGF_UNIX)
#include <netinet/in.h>

#define tgf_htonl(hl)	htonl(hl)
#define	tgf_htons(hs)	htons(hs)
#define	tgf_ntohl(nl)	ntohl(nl)
#define	tgf_ntohs(ns)	ntohs(ns)

#elif defined(WIN32)

static long tgf_ntohl(long nl)
{
	char *buf = (char *)&nl;
	char t;
	t = buf[0];
	buf[0] = buf[3];
	buf[3] = t;
	t = buf[2];
	buf[2] = buf[1];
	buf[1] = t;

	return nl;
}

static short tgf_ntohs(short ns)
{
	char *buf = (char *)&ns;
	char t;
	t = buf[0];
	buf[0] = buf[1];
	buf[1] = t;

	return ns;
}

#else
#error "Cannot handle platforms other than Unix or Windoze, yet.\n"
#endif

#endif /* __BYTESEX_H__ */
