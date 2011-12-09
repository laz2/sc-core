
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

%{
//  #include <std_constraints.h>
%}
%module stdconstraints
%inline %{
extern LIBSC_API sc_uint
	__constr_all_input,
	__constr_all_output,
	__constr_arc_type_input,
	__constr_arc_type_output,
	__constr_3faa,
	__constr_3faf,
	__constr_3aaf,
	__constr_3dfd,
	__constr_5faaaa,
	__constr_5faaaf,
	__constr_5fafaa,
	__constr_5fafaf,
	__constr_5aaaaf,
	__constr_5aafaa,
	__constr_5aafaf,
	__constr_ord_bin_conn1,
	__constr_ord_bin_conn1_def,
	__constr_ord_bin_conn2,
	__constr_3l2_faaaf,
	__constr_5_3_aaaafaf,
	__constr_in_set,
	__constr_sely3_p1,
	__constr_sely3_u1p2,
	__constr_sely3_u1u2p3,
	__constr_sely5_p1,
	__constr_sely5_u1p2,
	__constr_sely5_u1u2p3,
	__constr_sely5_u1u2u3p4,
	__constr_sely5_u1u2u3u4p5,
	__constr_intersect2,
	__constr_3l2_5faaaf,
	__constr_bin_conn_unord1,
	__constr_4ln5_faaaaaffa,
	__constr_4l5_faaaaaffa,
	__constr_ord_bin_conn1a,
	__constr_on_segment;
%}
#define CONSTR_ALL_INPUT	__constr_all_input
#define CONSTR_ALL_OUTPUT	__constr_all_output
#define CONSTR_ARC_TYPE_INPUT	__constr_arc_type_input
#define CONSTR_ARC_TYPE_OUTPUT	__constr_arc_type_output
#define CONSTR_3_f_a_a		__constr_3faa
#define CONSTR_3_f_a_f		__constr_3faf
#define CONSTR_3_a_a_f		__constr_3aaf
#define CONSTR_3_d_f_d		__constr_3dfd
#define CONSTR_5_f_a_a_a_a	__constr_5faaaa
#define CONSTR_5_f_a_a_a_f	__constr_5faaaf
#define CONSTR_5_f_a_f_a_a	__constr_5fafaa
#define CONSTR_5_f_a_f_a_f	__constr_5fafaf
#define CONSTR_5_a_a_a_a_f	__constr_5aaaaf
#define CONSTR_5_a_a_f_a_a	__constr_5aafaa
#define CONSTR_5_a_a_f_a_f	__constr_5aafaf
#define CONSTR_ORD_BIN_CONN1	__constr_ord_bin_conn1
#define CONSTR_ORD_BIN_CONN1_DEF	__constr_ord_bin_conn1_def
#define CONSTR_ORD_BIN_CONN2	__constr_ord_bin_conn2
#define CONSTR_3l2_f_a_a_a_f	__constr_3l2_faaaf
#define CONSTR_5_3_a_a_a_a_f_a_f __constr_5_3_aaaafaf
#define CONSTR_in_set		__constr_in_set
#define CONSTR_sely3_p1		__constr_sely3_p1
#define CONSTR_sely3_u1p2	__constr_sely3_u1p2
#define CONSTR_sely3_u1u2p3	__constr_sely3_u1u2p3
#define CONSTR_sely5_p1		__constr_sely5_p1
#define CONSTR_sely5_u1p2	__constr_sely5_u1p2
#define CONSTR_sely5_u1u2p3	__constr_sely5_u1u2p3
#define CONSTR_sely5_u1u2u3p4	__constr_sely5_u1u2u3p4
#define CONSTR_sely5_u1u2u3u4p5	__constr_sely5_u1u2u3u4p5
#define CONSTR_intersect2	__constr_intersect2
#define CONSTR_3l2_5faaaf	__constr_3l2_5faaaf
#define CONSTR_bin_conn_unord1	__constr_bin_conn_unord1
#define CONSTR_4ln5_faaaaaffa	__constr_4ln5_faaaaaffa
#define CONSTR_4l5_faaaaaffa	__constr_4l5_faaaaaffa
#define CONSTR_ORD_BIN_CONN1A	__constr_ord_bin_conn1a
#define CONSTR_ON_SEGMENT	__constr_on_segment

%rename(postinit_std_constraints) __postinit_std_constraints(sc_session *);
%rename(init_std_constraints)     __init_std_constraints();


