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
 * @file values.h
 * @author ALK
 * @brief some token values definitions.
 */

#ifndef __VALUES_H__
#define __VALUES_H__

typedef unsigned int con_type;

/* connector directions */
#define CON_DIR_MASK	0x200
#define CON_DIR_LEFT	0x000
#define	CON_DIR_RIGHT	0x200

/* connector types: {arc, non-arc} */
#define CON_TYPE_MASK	0x400
#define CON_TYPE_ARC	0x400
#define CON_TYPE_NONARC	0x000

/* scanner defines */
#define CON_PCR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_CONST|SC_POS)
#define CON_PVR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_VAR|SC_POS)
#define CON_PMR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_METAVAR|SC_POS)
#define CON_FCR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_CONST|SC_FUZ)
#define CON_FVR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_VAR|SC_FUZ)
#define CON_FMR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_METAVAR|SC_FUZ)
#define CON_NCR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_CONST|SC_NEG)
#define CON_NVR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_VAR|SC_NEG)
#define CON_NMR		(CON_TYPE_ARC|CON_DIR_RIGHT|SC_METAVAR|SC_NEG)
#define CON_PCL		(CON_TYPE_ARC|CON_DIR_LEFT|SC_CONST|SC_POS)
#define CON_PVL		(CON_TYPE_ARC|CON_DIR_LEFT|SC_VAR|SC_POS)
#define CON_PML		(CON_TYPE_ARC|CON_DIR_LEFT|SC_METAVAR|SC_POS)
#define CON_FCL		(CON_TYPE_ARC|CON_DIR_LEFT|SC_CONST|SC_FUZ)
#define CON_FVL		(CON_TYPE_ARC|CON_DIR_LEFT|SC_VAR|SC_FUZ)
#define CON_FML		(CON_TYPE_ARC|CON_DIR_LEFT|SC_METAVAR|SC_FUZ)
#define CON_NCL		(CON_TYPE_ARC|CON_DIR_LEFT|SC_CONST|SC_NEG)
#define CON_NVL		(CON_TYPE_ARC|CON_DIR_LEFT|SC_VAR|SC_NEG)
#define CON_NML		(CON_TYPE_ARC|CON_DIR_LEFT|SC_METAVAR|SC_NEG)

#define CON_NAR		(CON_TYPE_NONARC|CON_DIR_RIGHT)
#define CON_NAL		(CON_TYPE_NONARC|CON_DIR_LEFT)

#define CONT_NONE	0
#define CONT_STRING	1
#define CONT_NUMERIC	2
#define CONT_DATA	3
#define	CONT_FILE	4

#endif /* __VALUES_H__ */
