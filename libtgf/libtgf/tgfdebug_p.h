
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
 *  tgfdebug.h - libtgf debugging facilities (For internal purposes only)
 *
 *  Created at Wed 09 May 2001 01:08:34 AM EEST by ALK
 *
 */
#ifndef __TGFDEBUG_H__
#define __TGFDEBUG_H__

#if defined(CATCH_TGF_BUGS)

#if defined(TGF_BUG_ABORT)
#define TGF_BUG_END \
	abort();
#else
#define TGF_BUG_END
#endif /* TGF_BUG_ABORT */

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define	TGF_BUG_PRINT() \
	fprintf(stderr,"libTGF bug in %s:%s at line %d\n", \
			__FILE__,__FUNCTION__,__LINE__);
#else
#define	TGF_BUG_PRINT() \
	fprintf(stderr,"libTGF bug in file %s at line %d\n", \
			__FILE__,__LINE__);
#endif /* __GNUC__ && !__STRICT_ANSI__ */

#define TGF_BUG() \
	do { \
		TGF_BUG_PRINT() \
		TGF_BUG_END \
	} while(0)

#else
#define TGF_BUG() do {} while(0)
#endif /* CATCH_TGF_BUGS */

#endif /*__TGFDEBUG_H__*/
