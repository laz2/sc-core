
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

%module sctypes
%{
#include <libsc/sc_types.h>
%}

%include cpointer.i
%pointer_functions(sc_global_addr,sc_global_addrp)


typedef std::string sc_string;

struct sc_addr_ll_struct {};

typedef sc_addr_ll_struct *sc_addr_ll;

#define SCADDR_LL_NIL 0
#define SCADDR_NIL 0

// forward declarations
struct sc_constraint;
class sc_session;
class sc_segment;
class sc_iterator;

struct sc_global_addr;

typedef sc_global_addr *sc_addr;



typedef int sc_int;
typedef unsigned sc_uint;
typedef short sc_short;
typedef unsigned short sc_ushort;
typedef long sc_long;
typedef unsigned long sc_ulong;
typedef char sc_char;
typedef unsigned char sc_uchar;
typedef sc_char sc_int8;
typedef sc_uchar sc_uint8;
typedef sc_int sc_int32;
typedef sc_uchar sc_uint32;
typedef sc_short sc_int16;
typedef sc_ushort sc_uint16;
typedef sc_uint16 sc_type;

#define SC_EMPTY		0
#define SC_CONST		1
#define SC_VAR 			2
#define SC_METAVAR		4
#define SC_POS			8
#define SC_NEG			16
#define SC_FUZ			32
#define SC_UNDF			64
#define SC_ARC			128
#define SC_NODE			256

#define SC_TEMPORARY		512
#define SC_PERMANENT		1024

#define SC_PHANTOM		2048
#define SC_ACTUAL		4096

#define SC_FACTUAL		8192  

#define SC_PERMANENCY		(SC_TEMPORARY|SC_PERMANENT)
#define SC_ACTUALITY		(SC_ACTUAL|SC_PHANTOM)

#define SC_FUZZYNESS		(SC_POS|SC_NEG|SC_FUZ)
#define SC_ELMNCLASS		(SC_NODE|SC_ARC|SC_UNDF)
#define SC_CONSTANCY		(SC_CONST|SC_VAR|SC_METAVAR)

#define SC_U_CONST		(SC_UNDF|SC_CONST)
#define SC_U_VAR		(SC_UNDF|SC_VAR)
#define SC_U_METAVAR		(SC_UNDF|SC_METAVAR)
#define SC_N_CONST		(SC_NODE|SC_CONST)
#define SC_N_VAR		(SC_NODE|SC_VAR)
#define SC_N_METAVAR		(SC_NODE|SC_METAVAR)
#define SC_A_CONST		(SC_ARC|SC_CONST)
#define SC_A_VAR		(SC_ARC|SC_VAR)
#define SC_A_METAVAR		(SC_ARC|SC_METAVAR)

#define SC_UNCR			SC_UNDF
#define SC_A_PRMN		(SC_PERMANENT|SC_ACTUAL)
#define SC_A_TMPR		(SC_TEMPORARY)

#define SC_T_DFLT		(SC_ACTUAL & SC_ACTUALITY)

#define SC_R_DFLT		(SC_PHANTOM & SC_ACTUALITY)

#define SC_XPERMANENCY		(SC_PERMANENCY|SC_FUZ)
#define SC_POOR_MASK		(SC_CONSTANCY|SC_FUZZYNESS|SC_ELMNCLASS)

static inline
bool	__check_sc_type(sc_type val,sc_type mask)
{
	return val == (val & mask);
}

static inline
sc_type sc_type_extend_mask_group(sc_type mask, sc_type group) 
{
	return mask |= (mask & group)? SC_EMPTY: group;
}

static inline
sc_type	sc_type_extend_mask(sc_type mask)
{
	return sc_type_extend_mask_group(
		sc_type_extend_mask_group(
		sc_type_extend_mask_group(
		sc_type_extend_mask_group(
		sc_type_extend_mask_group(mask,
		SC_FUZZYNESS),
		SC_ELMNCLASS),
		SC_CONSTANCY),
		SC_PERMANENCY),
		SC_ACTUALITY);
}

static inline
bool 	check_sc_type(sc_type val,sc_type mask)
{
	return __check_sc_type(val,sc_type_extend_mask(mask));
}
extern unsigned char sc_type_valid_table[];

typedef sc_int sc_retval;
#define RV_OK		0	// will always remain 0
#define RV_THEN		RV_OK
#define RV_ERR_GEN	-1	/* generic error */
#define RV_ELSE_GEN	1	/* generic else */

#define RV_IS_ERR(rv)	(rv<0)
#define RV_IS_ELSE(rv)	(rv>0)

union sc_param {
	sc_type type;
	sc_addr addr;
	int i;
	sc_segment *seg;
};

class LIBSC_API sc_activity {
public:
	void *ctx;
	bool on_main_stack;
	bool sched_added;
	sc_activity();
	virtual sc_retval init(sc_addr _this) = 0;
	bool is_on_main_stack() {return on_main_stack;}
	virtual void done() = 0;
	virtual sc_retval activate(sc_session *,sc_addr _this,sc_addr param1,sc_addr prm2,sc_addr prm3) = 0;
	virtual ~sc_activity();
};
