/*
 * This is machine generated file don't edit it by hand !
 */

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

#include "precompiled_p.h"

sc_uint
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

static	sc_param_spec	std_sely5_u1u2u3u4p5_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE
};
static	int std_sely5_u1u2u3u4p5_fix[] = {
	4
};
static struct sc_constraint_info std_sely5_u1u2u3u4p5 = {
	0,
	"std_sely5_u1u2u3u4p5",
	6,
	std_sely5_u1u2u3u4p5_ispec,
	1,
	std_sely5_u1u2u3u4p5_fix
};
static	sc_param_spec	std3_aaf_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR
};
static	int std3_aaf_fix[] = {
	2
};
static struct sc_constraint_info std3_aaf = {
	0,
	"std3_aaf",
	3,
	std3_aaf_ispec,
	1,
	std3_aaf_fix
};
static	sc_param_spec	arc_type_output_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE
};
static	int arc_type_output_fix[] = {
	0
};
static struct sc_constraint_info arc_type_output = {
	0,
	"arc_type_output",
	2,
	arc_type_output_ispec,
	1,
	arc_type_output_fix
};
static	sc_param_spec	std3_faf_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR
};
static	int std3_faf_fix[] = {
	0,	2
};
static struct sc_constraint_info std3_faf = {
	0,
	"std3_faf",
	3,
	std3_faf_ispec,
	2,
	std3_faf_fix
};
static	sc_param_spec	std_sely5_u1u2p3_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE
};
static	int std_sely5_u1u2p3_fix[] = {
	2
};
static struct sc_constraint_info std_sely5_u1u2p3 = {
	0,
	"std_sely5_u1u2p3",
	8,
	std_sely5_u1u2p3_ispec,
	1,
	std_sely5_u1u2p3_fix
};
static	sc_param_spec	std_ord_bin_conn1_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR
};
static	int std_ord_bin_conn1_fix[] = {
	0,	4,	8,	10
};
static struct sc_constraint_info std_ord_bin_conn1 = {
	0,
	"std_ord_bin_conn1",
	11,
	std_ord_bin_conn1_ispec,
	4,
	std_ord_bin_conn1_fix
};
static	sc_param_spec	std5_aaaaf_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR
};
static	int std5_aaaaf_fix[] = {
	4
};
static struct sc_constraint_info std5_aaaaf = {
	0,
	"std5_aaaaf",
	5,
	std5_aaaaf_ispec,
	1,
	std5_aaaaf_fix
};
static	sc_param_spec	std5_aafaa_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE
};
static	int std5_aafaa_fix[] = {
	2
};
static struct sc_constraint_info std5_aafaa = {
	0,
	"std5_aafaa",
	5,
	std5_aafaa_ispec,
	1,
	std5_aafaa_fix
};
static	sc_param_spec	std5_faaaa_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE
};
static	int std5_faaaa_fix[] = {
	0
};
static struct sc_constraint_info std5_faaaa = {
	0,
	"std5_faaaa",
	5,
	std5_faaaa_ispec,
	1,
	std5_faaaa_fix
};
static	sc_param_spec	std_intersect2_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR
};
static	int std_intersect2_fix[] = {
	0,	4
};
static struct sc_constraint_info std_intersect2 = {
	0,
	"std_intersect2",
	5,
	std_intersect2_ispec,
	2,
	std_intersect2_fix
};
static	sc_param_spec	std_sely3_u1p2_ispec[]={
	SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE
};
static	int std_sely3_u1p2_fix[] = {
	1
};
static struct sc_constraint_info std_sely3_u1p2 = {
	0,
	"std_sely3_u1p2",
	5,
	std_sely3_u1p2_ispec,
	1,
	std_sely3_u1p2_fix
};
static	sc_param_spec	std_ord_bin_conn1_def_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR
};
static	int std_ord_bin_conn1_def_fix[] = {
	0,	4,	8
};
static struct sc_constraint_info std_ord_bin_conn1_def = {
	0,
	"std_ord_bin_conn1_def",
	9,
	std_ord_bin_conn1_def_ispec,
	3,
	std_ord_bin_conn1_def_fix
};
static	sc_param_spec	std_sely5_p1_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE
};
static	int std_sely5_p1_fix[] = {
	0
};
static struct sc_constraint_info std_sely5_p1 = {
	0,
	"std_sely5_p1",
	10,
	std_sely5_p1_ispec,
	1,
	std_sely5_p1_fix
};
static	sc_param_spec	std_on_segment_ispec[]={
	SC_PD_SEGMENT,SC_PD_BOOLEAN
};
static	int std_on_segment_fix[] = {
	0,	1
};
static struct sc_constraint_info std_on_segment = {
	0,
	"std_on_segment",
	2,
	std_on_segment_ispec,
	2,
	std_on_segment_fix
};
static	sc_param_spec	arc_type_input_ispec[]={
	SC_PD_TYPE,SC_PD_ADDR
};
static	int arc_type_input_fix[] = {
	1
};
static struct sc_constraint_info arc_type_input = {
	0,
	"arc_type_input",
	2,
	arc_type_input_ispec,
	1,
	arc_type_input_fix
};
static	sc_param_spec	std5_faaaf_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR
};
static	int std5_faaaf_fix[] = {
	0,	4
};
static struct sc_constraint_info std5_faaaf = {
	0,
	"std5_faaaf",
	5,
	std5_faaaf_ispec,
	2,
	std5_faaaf_fix
};
static	sc_param_spec	std_sely5_u1u2u3p4_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE
};
static	int std_sely5_u1u2u3p4_fix[] = {
	3
};
static struct sc_constraint_info std_sely5_u1u2u3p4 = {
	0,
	"std_sely5_u1u2u3p4",
	7,
	std_sely5_u1u2u3p4_ispec,
	1,
	std_sely5_u1u2u3p4_fix
};
static	sc_param_spec	all_output_ispec[]={
	SC_PD_ADDR
};
static	int all_output_fix[] = {
	0
};
static struct sc_constraint_info all_output = {
	0,
	"all_output",
	1,
	all_output_ispec,
	1,
	all_output_fix
};
static	sc_param_spec	std3_faa_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE
};
static	int std3_faa_fix[] = {
	0
};
static struct sc_constraint_info std3_faa = {
	0,
	"std3_faa",
	3,
	std3_faa_ispec,
	1,
	std3_faa_fix
};
static	sc_param_spec	std_in_set_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE
};
static	int std_in_set_fix[] = {
	0
};
static struct sc_constraint_info std_in_set = {
	0,
	"std_in_set",
	2,
	std_in_set_ispec,
	1,
	std_in_set_fix
};
static	sc_param_spec	std_sely3_u1u2p3_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE
};
static	int std_sely3_u1u2p3_fix[] = {
	2
};
static struct sc_constraint_info std_sely3_u1u2p3 = {
	0,
	"std_sely3_u1u2p3",
	4,
	std_sely3_u1u2p3_ispec,
	1,
	std_sely3_u1u2p3_fix
};
static	sc_param_spec	std3l2_faaaf_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR
};
static	int std3l2_faaaf_fix[] = {
	0,	4
};
static struct sc_constraint_info std3l2_faaaf = {
	0,
	"std3l2_faaaf",
	5,
	std3l2_faaaf_ispec,
	2,
	std3l2_faaaf_fix
};
static	sc_param_spec	std_sely3_p1_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE
};
static	int std_sely3_p1_fix[] = {
	0
};
static struct sc_constraint_info std_sely3_p1 = {
	0,
	"std_sely3_p1",
	6,
	std_sely3_p1_ispec,
	1,
	std_sely3_p1_fix
};
static	sc_param_spec	std_sely5_u1p2_ispec[]={
	SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_TYPE
};
static	int std_sely5_u1p2_fix[] = {
	1
};
static struct sc_constraint_info std_sely5_u1p2 = {
	0,
	"std_sely5_u1p2",
	9,
	std_sely5_u1p2_ispec,
	1,
	std_sely5_u1p2_fix
};
static	sc_param_spec	std_4ln5_faaaaaffa_ispec[]={
	SC_PD_ADDR,SC_PD_ADDR,SC_PD_ADDR
};
static	int std_4ln5_faaaaaffa_fix[] = {
	0,	1,	2
};
static struct sc_constraint_info std_4ln5_faaaaaffa = {
	0,
	"std_4ln5_faaaaaffa",
	3,
	std_4ln5_faaaaaffa_ispec,
	3,
	std_4ln5_faaaaaffa_fix
};
static	sc_param_spec	std5_fafaf_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR
};
static	int std5_fafaf_fix[] = {
	0,	2,	4
};
static struct sc_constraint_info std5_fafaf = {
	0,
	"std5_fafaf",
	5,
	std5_fafaf_ispec,
	3,
	std5_fafaf_fix
};
static	sc_param_spec	std5_aafaf_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR
};
static	int std5_aafaf_fix[] = {
	2,	4
};
static struct sc_constraint_info std5_aafaf = {
	0,
	"std5_aafaf",
	5,
	std5_aafaf_ispec,
	2,
	std5_aafaf_fix
};
static	sc_param_spec	std_ord_bin_conn1a_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE
};
static	int std_ord_bin_conn1a_fix[] = {
	0,	4,	7
};
static struct sc_constraint_info std_ord_bin_conn1a = {
	0,
	"std_ord_bin_conn1a",
	9,
	std_ord_bin_conn1a_ispec,
	3,
	std_ord_bin_conn1a_fix
};
static	sc_param_spec	std_3l2_5faaaf_ispec[]={
	SC_PD_ADDR,SC_PD_ADDR,SC_PD_ADDR,SC_PD_ADDR,SC_PD_ADDR
};
static	int std_3l2_5faaaf_fix[] = {
	0,	1,	2,	3,	4
};
static struct sc_constraint_info std_3l2_5faaaf = {
	0,
	"std_3l2_5faaaf",
	5,
	std_3l2_5faaaf_ispec,
	5,
	std_3l2_5faaaf_fix
};
static	sc_param_spec	std3_dfd_ispec[]={
	SC_PD_ADDR_0,SC_PD_INT,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR_0,SC_PD_INT,SC_PD_TYPE
};
static	int std3_dfd_fix[] = {
	3
};
static struct sc_constraint_info std3_dfd = {
	0,
	"std3_dfd",
	8,
	std3_dfd_ispec,
	1,
	std3_dfd_fix
};
static	sc_param_spec	std5_fafaa_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE
};
static	int std5_fafaa_fix[] = {
	0,	2
};
static struct sc_constraint_info std5_fafaa = {
	0,
	"std5_fafaa",
	5,
	std5_fafaa_ispec,
	2,
	std5_fafaa_fix
};
static	sc_param_spec	all_input_ispec[]={
	SC_PD_ADDR
};
static	int all_input_fix[] = {
	0
};
static struct sc_constraint_info all_input = {
	0,
	"all_input",
	1,
	all_input_ispec,
	1,
	all_input_fix
};
static	sc_param_spec	std_4l5_faaaaaffa_ispec[]={
	SC_PD_ADDR,SC_PD_ADDR,SC_PD_ADDR
};
static	int std_4l5_faaaaaffa_fix[] = {
	0,	1,	2
};
static struct sc_constraint_info std_4l5_faaaaaffa = {
	0,
	"std_4l5_faaaaaffa",
	3,
	std_4l5_faaaaaffa_ispec,
	3,
	std_4l5_faaaaaffa_fix
};
static	sc_param_spec	std_bin_conn_unord1_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE
};
static	int std_bin_conn_unord1_fix[] = {
	0,	4
};
static struct sc_constraint_info std_bin_conn_unord1 = {
	0,
	"std_bin_conn_unord1",
	7,
	std_bin_conn_unord1_ispec,
	2,
	std_bin_conn_unord1_fix
};
static	sc_param_spec	std_ord_bin_conn2_ispec[]={
	SC_PD_ADDR,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR
};
static	int std_ord_bin_conn2_fix[] = {
	0,	4,	6,	8,	10
};
static struct sc_constraint_info std_ord_bin_conn2 = {
	0,
	"std_ord_bin_conn2",
	11,
	std_ord_bin_conn2_ispec,
	5,
	std_ord_bin_conn2_fix
};
static	sc_param_spec	std5_3_aaaafaf_ispec[]={
	SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_TYPE,SC_PD_ADDR,SC_PD_TYPE,SC_PD_ADDR
};
static	int std5_3_aaaafaf_fix[] = {
	4,	6
};
static struct sc_constraint_info std5_3_aaaafaf = {
	0,
	"std5_3_aaaafaf",
	7,
	std5_3_aaaafaf_ispec,
	2,
	std5_3_aaaafaf_fix
};
static	int dec_std3_faf_prg_i0_i[] = {
	2
};
static	int dec_std3_faf_prg_i0_o[] = {
	-1,	3
};
static	int dec_std3_faf_prg_i1_i[] = {
	3
};
static	int dec_std3_faf_prg_i1_o[] = {
	4
};
static	int dec_std3_faf_prg_i2_i[] = {
	0,	4
};
static	int dec_std3_faf_prg_i3_i[] = {
	3,	1
};
static	int dec_std3_faf_prg_i4_i[] = {
	0,	3,	2
};
static struct sc_deconstruct_prg_item dec_std3_faf_prg[] = {
	{
		SCD_CONSTR,
		1,
		dec_std3_faf_prg_i0_i,
		2,
		dec_std3_faf_prg_i0_o,
		"all_input"
	}, {
		SCD_FUNC,
		1,
		dec_std3_faf_prg_i1_i,
		1,
		dec_std3_faf_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std3_faf_prg_i2_i,
		0,
		0,
		"equal2"
	}, {
		SCD_FILTER,
		2,
		dec_std3_faf_prg_i3_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		3,
		dec_std3_faf_prg_i4_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std3_faf = {
	3,
	2,
	5,
	dec_std3_faf_prg
};
static	int dec_std_sely3_u1u2p3_prg_i0_i[] = {
	2,	3
};
static	int dec_std_sely3_u1u2p3_prg_i0_o[] = {
	6
};
static	int dec_std_sely3_u1u2p3_prg_i1_i[] = {
	0,	1,	6
};
static	int dec_std_sely3_u1u2p3_prg_i1_o[] = {
	4,	5
};
static	int dec_std_sely3_u1u2p3_prg_i2_i[] = {
	4,	5,	6
};
static struct sc_deconstruct_prg_item dec_std_sely3_u1u2p3_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely3_u1u2p3_prg_i0_i,
		1,
		dec_std_sely3_u1u2p3_prg_i0_o,
		"std_in_set"
	}, {
		SCD_CONSTR,
		3,
		dec_std_sely3_u1u2p3_prg_i1_i,
		2,
		dec_std_sely3_u1u2p3_prg_i1_o,
		"std3_aaf"
	}, {
		SCD_RETURN,
		3,
		dec_std_sely3_u1u2p3_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely3_u1u2p3 = {
	4,
	3,
	3,
	dec_std_sely3_u1u2p3_prg
};
static	int dec_std_bin_conn_unord1_prg_i0_i[] = {
	0,	1,	2,	3,	4
};
static	int dec_std_bin_conn_unord1_prg_i0_o[] = {
	-1,	7,	8,	9
};
static	int dec_std_bin_conn_unord1_prg_i1_i[] = {
	8,	5,	6
};
static	int dec_std_bin_conn_unord1_prg_i1_o[] = {
	-1,	10,	11
};
static	int dec_std_bin_conn_unord1_prg_i2_i[] = {
	0,	7,	8,	9,	4,	10,	11
};
static struct sc_deconstruct_prg_item dec_std_bin_conn_unord1_prg[] = {
	{
		SCD_CONSTR,
		5,
		dec_std_bin_conn_unord1_prg_i0_i,
		4,
		dec_std_bin_conn_unord1_prg_i0_o,
		"std3l2_faaaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std_bin_conn_unord1_prg_i1_i,
		3,
		dec_std_bin_conn_unord1_prg_i1_o,
		"std3_faa"
	}, {
		SCD_RETURN,
		7,
		dec_std_bin_conn_unord1_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_bin_conn_unord1 = {
	7,
	5,
	3,
	dec_std_bin_conn_unord1_prg
};
static	int dec_arc_type_output_prg_i0_i[] = {
	0
};
static	int dec_arc_type_output_prg_i0_o[] = {
	-1,	2
};
static	int dec_arc_type_output_prg_i1_i[] = {
	2,	1
};
static	int dec_arc_type_output_prg_i2_i[] = {
	0,	2
};
static struct sc_deconstruct_prg_item dec_arc_type_output_prg[] = {
	{
		SCD_CONSTR,
		1,
		dec_arc_type_output_prg_i0_i,
		2,
		dec_arc_type_output_prg_i0_o,
		"all_output"
	}, {
		SCD_FILTER,
		2,
		dec_arc_type_output_prg_i1_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		2,
		dec_arc_type_output_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_arc_type_output = {
	2,
	1,
	3,
	dec_arc_type_output_prg
};
static	int dec_std_sely5_u1p2_prg_i0_i[] = {
	1,	2
};
static	int dec_std_sely5_u1p2_prg_i0_o[] = {
	10
};
static	int dec_std_sely5_u1p2_prg_i1_i[] = {
	10
};
static	int dec_std_sely5_u1p2_prg_i1_o[] = {
	9
};
static	int dec_std_sely5_u1p2_prg_i2_i[] = {
	9,	0
};
static	int dec_std_sely5_u1p2_prg_i3_i[] = {
	10
};
static	int dec_std_sely5_u1p2_prg_i3_o[] = {
	11
};
static	int dec_std_sely5_u1p2_prg_i4_i[] = {
	11,	4
};
static	int dec_std_sely5_u1p2_prg_i5_i[] = {
	3,	11
};
static	int dec_std_sely5_u1p2_prg_i6_i[] = {
	8,	6,	10
};
static	int dec_std_sely5_u1p2_prg_i6_o[] = {
	13,	12
};
static	int dec_std_sely5_u1p2_prg_i7_i[] = {
	5,	12
};
static	int dec_std_sely5_u1p2_prg_i8_i[] = {
	13,	7
};
static	int dec_std_sely5_u1p2_prg_i9_i[] = {
	9,	10,	11,	12,	13
};
static struct sc_deconstruct_prg_item dec_std_sely5_u1p2_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely5_u1p2_prg_i0_i,
		1,
		dec_std_sely5_u1p2_prg_i0_o,
		"std_in_set"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely5_u1p2_prg_i1_i,
		1,
		dec_std_sely5_u1p2_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1p2_prg_i2_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely5_u1p2_prg_i3_i,
		1,
		dec_std_sely5_u1p2_prg_i3_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1p2_prg_i4_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1p2_prg_i5_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_CONSTR,
		3,
		dec_std_sely5_u1p2_prg_i6_i,
		2,
		dec_std_sely5_u1p2_prg_i6_o,
		"std3_aaf"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1p2_prg_i7_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1p2_prg_i8_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_RETURN,
		5,
		dec_std_sely5_u1p2_prg_i9_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely5_u1p2 = {
	9,
	5,
	10,
	dec_std_sely5_u1p2_prg
};
static	int dec_std_sely5_u1u2u3p4_prg_i0_i[] = {
	3,	4
};
static	int dec_std_sely5_u1u2u3p4_prg_i0_o[] = {
	10
};
static	int dec_std_sely5_u1u2u3p4_prg_i1_i[] = {
	10
};
static	int dec_std_sely5_u1u2u3p4_prg_i1_o[] = {
	11
};
static	int dec_std_sely5_u1u2u3p4_prg_i2_i[] = {
	11,	6
};
static	int dec_std_sely5_u1u2u3p4_prg_i3_i[] = {
	5,	11
};
static	int dec_std_sely5_u1u2u3p4_prg_i4_i[] = {
	10
};
static	int dec_std_sely5_u1u2u3p4_prg_i4_o[] = {
	8
};
static	int dec_std_sely5_u1u2u3p4_prg_i5_i[] = {
	8,	1
};
static	int dec_std_sely5_u1u2u3p4_prg_i6_i[] = {
	8
};
static	int dec_std_sely5_u1u2u3p4_prg_i6_o[] = {
	7
};
static	int dec_std_sely5_u1u2u3p4_prg_i7_i[] = {
	8
};
static	int dec_std_sely5_u1u2u3p4_prg_i7_o[] = {
	9
};
static	int dec_std_sely5_u1u2u3p4_prg_i8_i[] = {
	7,	0
};
static	int dec_std_sely5_u1u2u3p4_prg_i9_i[] = {
	9,	2
};
static	int dec_std_sely5_u1u2u3p4_prg_i10_i[] = {
	7,	8,	9,	10,	11
};
static struct sc_deconstruct_prg_item dec_std_sely5_u1u2u3p4_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely5_u1u2u3p4_prg_i0_i,
		1,
		dec_std_sely5_u1u2u3p4_prg_i0_o,
		"std_in_set"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely5_u1u2u3p4_prg_i1_i,
		1,
		dec_std_sely5_u1u2u3p4_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2u3p4_prg_i2_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2u3p4_prg_i3_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely5_u1u2u3p4_prg_i4_i,
		1,
		dec_std_sely5_u1u2u3p4_prg_i4_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2u3p4_prg_i5_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely5_u1u2u3p4_prg_i6_i,
		1,
		dec_std_sely5_u1u2u3p4_prg_i6_o,
		"get_beg"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely5_u1u2u3p4_prg_i7_i,
		1,
		dec_std_sely5_u1u2u3p4_prg_i7_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2u3p4_prg_i8_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2u3p4_prg_i9_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		5,
		dec_std_sely5_u1u2u3p4_prg_i10_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely5_u1u2u3p4 = {
	7,
	5,
	11,
	dec_std_sely5_u1u2u3p4_prg
};
static	int dec_std_sely5_u1u2p3_prg_i0_i[] = {
	2,	3
};
static	int dec_std_sely5_u1u2p3_prg_i0_o[] = {
	10
};
static	int dec_std_sely5_u1u2p3_prg_i1_i[] = {
	0,	1,	10
};
static	int dec_std_sely5_u1u2p3_prg_i1_o[] = {
	8,	9
};
static	int dec_std_sely5_u1u2p3_prg_i2_i[] = {
	7,	5,	9
};
static	int dec_std_sely5_u1u2p3_prg_i2_o[] = {
	12,	11
};
static	int dec_std_sely5_u1u2p3_prg_i3_i[] = {
	4,	11
};
static	int dec_std_sely5_u1u2p3_prg_i4_i[] = {
	6,	12
};
static	int dec_std_sely5_u1u2p3_prg_i5_i[] = {
	8,	9,	10,	11,	12
};
static struct sc_deconstruct_prg_item dec_std_sely5_u1u2p3_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely5_u1u2p3_prg_i0_i,
		1,
		dec_std_sely5_u1u2p3_prg_i0_o,
		"std_in_set"
	}, {
		SCD_CONSTR,
		3,
		dec_std_sely5_u1u2p3_prg_i1_i,
		2,
		dec_std_sely5_u1u2p3_prg_i1_o,
		"std3_aaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std_sely5_u1u2p3_prg_i2_i,
		2,
		dec_std_sely5_u1u2p3_prg_i2_o,
		"std3_aaf"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2p3_prg_i3_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_u1u2p3_prg_i4_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_RETURN,
		5,
		dec_std_sely5_u1u2p3_prg_i5_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely5_u1u2p3 = {
	8,
	5,
	6,
	dec_std_sely5_u1u2p3_prg
};
static	int dec_std5_faaaf_prg_i0_i[] = {
	0
};
static	int dec_std5_faaaf_prg_i0_o[] = {
	-1,	5
};
static	int dec_std5_faaaf_prg_i1_i[] = {
	5,	1
};
static	int dec_std5_faaaf_prg_i2_i[] = {
	5
};
static	int dec_std5_faaaf_prg_i2_o[] = {
	6
};
static	int dec_std5_faaaf_prg_i3_i[] = {
	6,	2
};
static	int dec_std5_faaaf_prg_i4_i[] = {
	5
};
static	int dec_std5_faaaf_prg_i4_o[] = {
	-1,	7
};
static	int dec_std5_faaaf_prg_i5_i[] = {
	7,	3
};
static	int dec_std5_faaaf_prg_i6_i[] = {
	7
};
static	int dec_std5_faaaf_prg_i6_o[] = {
	8
};
static	int dec_std5_faaaf_prg_i7_i[] = {
	4,	8
};
static	int dec_std5_faaaf_prg_i8_i[] = {
	0,	5,	6,	7,	4
};
static struct sc_deconstruct_prg_item dec_std5_faaaf_prg[] = {
	{
		SCD_CONSTR,
		1,
		dec_std5_faaaf_prg_i0_i,
		2,
		dec_std5_faaaf_prg_i0_o,
		"all_output"
	}, {
		SCD_FILTER,
		2,
		dec_std5_faaaf_prg_i1_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std5_faaaf_prg_i2_i,
		1,
		dec_std5_faaaf_prg_i2_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std5_faaaf_prg_i3_i,
		0,
		0,
		"check_type"
	}, {
		SCD_CONSTR,
		1,
		dec_std5_faaaf_prg_i4_i,
		2,
		dec_std5_faaaf_prg_i4_o,
		"all_input"
	}, {
		SCD_FILTER,
		2,
		dec_std5_faaaf_prg_i5_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std5_faaaf_prg_i6_i,
		1,
		dec_std5_faaaf_prg_i6_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std5_faaaf_prg_i7_i,
		0,
		0,
		"equal2"
	}, {
		SCD_RETURN,
		5,
		dec_std5_faaaf_prg_i8_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_faaaf = {
	5,
	4,
	9,
	dec_std5_faaaf_prg
};
static	int dec_std_4l5_faaaaaffa_prg_i0_o[] = {
	8
};
static	int dec_std_4l5_faaaaaffa_prg_i1_o[] = {
	9
};
static	int dec_std_4l5_faaaaaffa_prg_i2_o[] = {
	10
};
static	int dec_std_4l5_faaaaaffa_prg_i3_i[] = {
	0,	8,	10
};
static	int dec_std_4l5_faaaaaffa_prg_i3_o[] = {
	-1,	3,	4
};
static	int dec_std_4l5_faaaaaffa_prg_i4_i[] = {
	4,	8,	10
};
static	int dec_std_4l5_faaaaaffa_prg_i4_o[] = {
	-1,	5,	6
};
static	int dec_std_4l5_faaaaaffa_prg_i5_i[] = {
	1,	9,	6
};
static	int dec_std_4l5_faaaaaffa_prg_i5_o[] = {
	-1,	7
};
static	int dec_std_4l5_faaaaaffa_prg_i6_i[] = {
	2,	8,	5
};
static	int dec_std_4l5_faaaaaffa_prg_i6_o[] = {
	-1,	11
};
static	int dec_std_4l5_faaaaaffa_prg_i7_i[] = {
	0,	3,	4,	5,	6,	7,	1,	2,	11
};
static struct sc_deconstruct_prg_item dec_std_4l5_faaaaaffa_prg[] = {
	{
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_4l5_faaaaaffa_prg_i0_o,
		"get_cpa"
	}, {
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_4l5_faaaaaffa_prg_i1_o,
		"get_cna"
	}, {
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_4l5_faaaaaffa_prg_i2_o,
		"get_0"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4l5_faaaaaffa_prg_i3_i,
		3,
		dec_std_4l5_faaaaaffa_prg_i3_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4l5_faaaaaffa_prg_i4_i,
		3,
		dec_std_4l5_faaaaaffa_prg_i4_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4l5_faaaaaffa_prg_i5_i,
		2,
		dec_std_4l5_faaaaaffa_prg_i5_o,
		"std3_faf"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4l5_faaaaaffa_prg_i6_i,
		2,
		dec_std_4l5_faaaaaffa_prg_i6_o,
		"std3_faf"
	}, {
		SCD_RETURN,
		9,
		dec_std_4l5_faaaaaffa_prg_i7_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_4l5_faaaaaffa = {
	3,
	9,
	8,
	dec_std_4l5_faaaaaffa_prg
};
static	int dec_std3_aaf_prg_i0_i[] = {
	1,	2
};
static	int dec_std3_aaf_prg_i0_o[] = {
	4
};
static	int dec_std3_aaf_prg_i1_i[] = {
	4
};
static	int dec_std3_aaf_prg_i1_o[] = {
	3
};
static	int dec_std3_aaf_prg_i2_i[] = {
	3,	0
};
static	int dec_std3_aaf_prg_i3_i[] = {
	3,	4,	2
};
static struct sc_deconstruct_prg_item dec_std3_aaf_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std3_aaf_prg_i0_i,
		1,
		dec_std3_aaf_prg_i0_o,
		"arc_type_input"
	}, {
		SCD_FUNC,
		1,
		dec_std3_aaf_prg_i1_i,
		1,
		dec_std3_aaf_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std3_aaf_prg_i2_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		3,
		dec_std3_aaf_prg_i3_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std3_aaf = {
	3,
	2,
	4,
	dec_std3_aaf_prg
};
static	int dec_std5_aafaf_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std5_aafaf_prg_i0_o[] = {
	5,	6
};
static	int dec_std5_aafaf_prg_i1_i[] = {
	4,	3,	6
};
static	int dec_std5_aafaf_prg_i1_o[] = {
	-1,	8
};
static	int dec_std5_aafaf_prg_i2_i[] = {
	5,	6,	2,	8,	4
};
static struct sc_deconstruct_prg_item dec_std5_aafaf_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std5_aafaf_prg_i0_i,
		2,
		dec_std5_aafaf_prg_i0_o,
		"std3_aaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std5_aafaf_prg_i1_i,
		2,
		dec_std5_aafaf_prg_i1_o,
		"std3_faf"
	}, {
		SCD_RETURN,
		5,
		dec_std5_aafaf_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_aafaf = {
	5,
	4,
	3,
	dec_std5_aafaf_prg
};
static	int dec_std_intersect2_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std_intersect2_prg_i0_o[] = {
	-1,	5,	6
};
static	int dec_std_intersect2_prg_i1_i[] = {
	4,	3,	6
};
static	int dec_std_intersect2_prg_i1_o[] = {
	-1,	7
};
static	int dec_std_intersect2_prg_i2_i[] = {
	0,	5,	6,	7,	4
};
static struct sc_deconstruct_prg_item dec_std_intersect2_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std_intersect2_prg_i0_i,
		3,
		dec_std_intersect2_prg_i0_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		3,
		dec_std_intersect2_prg_i1_i,
		2,
		dec_std_intersect2_prg_i1_o,
		"std3_faf"
	}, {
		SCD_RETURN,
		5,
		dec_std_intersect2_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_intersect2 = {
	5,
	3,
	3,
	dec_std_intersect2_prg
};
static	int dec_std_sely5_u1u2u3u4p5_prg_i0_i[] = {
	4,	5
};
static	int dec_std_sely5_u1u2u3u4p5_prg_i0_o[] = {
	10
};
static	int dec_std_sely5_u1u2u3u4p5_prg_i1_i[] = {
	0,	1,	2,	3,	10
};
static	int dec_std_sely5_u1u2u3u4p5_prg_i1_o[] = {
	6,	7,	8,	9
};
static	int dec_std_sely5_u1u2u3u4p5_prg_i2_i[] = {
	6,	7,	8,	9,	10
};
static struct sc_deconstruct_prg_item dec_std_sely5_u1u2u3u4p5_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely5_u1u2u3u4p5_prg_i0_i,
		1,
		dec_std_sely5_u1u2u3u4p5_prg_i0_o,
		"std_in_set"
	}, {
		SCD_CONSTR,
		5,
		dec_std_sely5_u1u2u3u4p5_prg_i1_i,
		4,
		dec_std_sely5_u1u2u3u4p5_prg_i1_o,
		"std5_aaaaf"
	}, {
		SCD_RETURN,
		5,
		dec_std_sely5_u1u2u3u4p5_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely5_u1u2u3u4p5 = {
	6,
	5,
	3,
	dec_std_sely5_u1u2u3u4p5_prg
};
static	int dec_std5_aafaa_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std5_aafaa_prg_i0_o[] = {
	5,	6
};
static	int dec_std5_aafaa_prg_i1_i[] = {
	4,	3,	6
};
static	int dec_std5_aafaa_prg_i1_o[] = {
	8,	7
};
static	int dec_std5_aafaa_prg_i2_i[] = {
	5,	6,	2,	7,	8
};
static struct sc_deconstruct_prg_item dec_std5_aafaa_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std5_aafaa_prg_i0_i,
		2,
		dec_std5_aafaa_prg_i0_o,
		"std3_aaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std5_aafaa_prg_i1_i,
		2,
		dec_std5_aafaa_prg_i1_o,
		"std3_aaf"
	}, {
		SCD_RETURN,
		5,
		dec_std5_aafaa_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_aafaa = {
	5,
	4,
	3,
	dec_std5_aafaa_prg
};
static	int dec_std_sely5_p1_prg_i0_i[] = {
	0,	1
};
static	int dec_std_sely5_p1_prg_i0_o[] = {
	10
};
static	int dec_std_sely5_p1_prg_i1_i[] = {
	10,	3,	5,	7,	9
};
static	int dec_std_sely5_p1_prg_i1_o[] = {
	-1,	11,	12,	13,	14
};
static	int dec_std_sely5_p1_prg_i2_i[] = {
	6,	13
};
static	int dec_std_sely5_p1_prg_i3_i[] = {
	2,	11
};
static	int dec_std_sely5_p1_prg_i4_i[] = {
	4,	12
};
static	int dec_std_sely5_p1_prg_i5_i[] = {
	8,	14
};
static	int dec_std_sely5_p1_prg_i6_i[] = {
	10,	11,	12,	13,	14
};
static struct sc_deconstruct_prg_item dec_std_sely5_p1_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely5_p1_prg_i0_i,
		1,
		dec_std_sely5_p1_prg_i0_o,
		"std_in_set"
	}, {
		SCD_CONSTR,
		5,
		dec_std_sely5_p1_prg_i1_i,
		5,
		dec_std_sely5_p1_prg_i1_o,
		"std5_faaaa"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_p1_prg_i2_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_p1_prg_i3_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_p1_prg_i4_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely5_p1_prg_i5_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_RETURN,
		5,
		dec_std_sely5_p1_prg_i6_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely5_p1 = {
	10,
	5,
	7,
	dec_std_sely5_p1_prg
};
static	int dec_std5_faaaa_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std5_faaaa_prg_i0_o[] = {
	-1,	5,	6
};
static	int dec_std5_faaaa_prg_i1_i[] = {
	4,	3,	5
};
static	int dec_std5_faaaa_prg_i1_o[] = {
	8,	7
};
static	int dec_std5_faaaa_prg_i2_i[] = {
	0,	5,	6,	7,	8
};
static struct sc_deconstruct_prg_item dec_std5_faaaa_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std5_faaaa_prg_i0_i,
		3,
		dec_std5_faaaa_prg_i0_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		3,
		dec_std5_faaaa_prg_i1_i,
		2,
		dec_std5_faaaa_prg_i1_o,
		"std3_aaf"
	}, {
		SCD_RETURN,
		5,
		dec_std5_faaaa_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_faaaa = {
	5,
	4,
	3,
	dec_std5_faaaa_prg
};
static	int dec_std_in_set_prg_i0_o[] = {
	3
};
static	int dec_std_in_set_prg_i1_i[] = {
	0,	3,	1
};
static	int dec_std_in_set_prg_i1_o[] = {
	-1,	-1,	2
};
static	int dec_std_in_set_prg_i2_i[] = {
	2
};
static struct sc_deconstruct_prg_item dec_std_in_set_prg[] = {
	{
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_in_set_prg_i0_o,
		"get_cpa"
	}, {
		SCD_CONSTR,
		3,
		dec_std_in_set_prg_i1_i,
		3,
		dec_std_in_set_prg_i1_o,
		"std3_faa"
	}, {
		SCD_RETURN,
		1,
		dec_std_in_set_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_in_set = {
	2,
	2,
	3,
	dec_std_in_set_prg
};
static	int dec_std5_aaaaf_prg_i0_i[] = {
	4,	3,	1
};
static	int dec_std5_aaaaf_prg_i0_o[] = {
	-1,	8,	6
};
static	int dec_std5_aaaaf_prg_i1_i[] = {
	6
};
static	int dec_std5_aaaaf_prg_i1_o[] = {
	5
};
static	int dec_std5_aaaaf_prg_i2_i[] = {
	5,	0
};
static	int dec_std5_aaaaf_prg_i3_i[] = {
	6
};
static	int dec_std5_aaaaf_prg_i3_o[] = {
	7
};
static	int dec_std5_aaaaf_prg_i4_i[] = {
	7,	2
};
static	int dec_std5_aaaaf_prg_i5_i[] = {
	5,	6,	7,	8,	4
};
static struct sc_deconstruct_prg_item dec_std5_aaaaf_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std5_aaaaf_prg_i0_i,
		3,
		dec_std5_aaaaf_prg_i0_o,
		"std3_faa"
	}, {
		SCD_FUNC,
		1,
		dec_std5_aaaaf_prg_i1_i,
		1,
		dec_std5_aaaaf_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std5_aaaaf_prg_i2_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std5_aaaaf_prg_i3_i,
		1,
		dec_std5_aaaaf_prg_i3_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std5_aaaaf_prg_i4_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		5,
		dec_std5_aaaaf_prg_i5_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_aaaaf = {
	5,
	4,
	6,
	dec_std5_aaaaf_prg
};
static	int dec_std_ord_bin_conn1a_prg_i0_i[] = {
	2,	3,	4
};
static	int dec_std_ord_bin_conn1a_prg_i0_o[] = {
	10,	11
};
static	int dec_std_ord_bin_conn1a_prg_i1_i[] = {
	0,	1,	10
};
static	int dec_std_ord_bin_conn1a_prg_i1_o[] = {
	-1,	9
};
static	int dec_std_ord_bin_conn1a_prg_i2_i[] = {
	10,	5,	6,	8,	7
};
static	int dec_std_ord_bin_conn1a_prg_i2_o[] = {
	-1,	12,	13,	14
};
static	int dec_std_ord_bin_conn1a_prg_i3_i[] = {
	0,	9,	10,	11,	4,	12,	13,	7,	14
};
static struct sc_deconstruct_prg_item dec_std_ord_bin_conn1a_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std_ord_bin_conn1a_prg_i0_i,
		2,
		dec_std_ord_bin_conn1a_prg_i0_o,
		"std3_aaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std_ord_bin_conn1a_prg_i1_i,
		2,
		dec_std_ord_bin_conn1a_prg_i1_o,
		"std3_faf"
	}, {
		SCD_CONSTR,
		5,
		dec_std_ord_bin_conn1a_prg_i2_i,
		4,
		dec_std_ord_bin_conn1a_prg_i2_o,
		"std5_faaaf"
	}, {
		SCD_RETURN,
		9,
		dec_std_ord_bin_conn1a_prg_i3_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_ord_bin_conn1a = {
	9,
	6,
	4,
	dec_std_ord_bin_conn1a_prg
};
static	int dec_std3_dfd_prg_i0_i[] = {
	3,	4
};
static	int dec_std3_dfd_prg_i1_i[] = {
	3
};
static	int dec_std3_dfd_prg_i1_o[] = {
	8
};
static	int dec_std3_dfd_prg_i2_i[] = {
	0,	8,	1,	2
};
static	int dec_std3_dfd_prg_i3_i[] = {
	3
};
static	int dec_std3_dfd_prg_i3_o[] = {
	9
};
static	int dec_std3_dfd_prg_i4_i[] = {
	5,	9,	6,	7
};
static	int dec_std3_dfd_prg_i5_i[] = {
	8,	3,	9
};
static struct sc_deconstruct_prg_item dec_std3_dfd_prg[] = {
	{
		SCD_FILTER,
		2,
		dec_std3_dfd_prg_i0_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std3_dfd_prg_i1_i,
		1,
		dec_std3_dfd_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		4,
		dec_std3_dfd_prg_i2_i,
		0,
		0,
		"equal_with_d"
	}, {
		SCD_FUNC,
		1,
		dec_std3_dfd_prg_i3_i,
		1,
		dec_std3_dfd_prg_i3_o,
		"get_end"
	}, {
		SCD_FILTER,
		4,
		dec_std3_dfd_prg_i4_i,
		0,
		0,
		"equal_with_d"
	}, {
		SCD_RETURN,
		3,
		dec_std3_dfd_prg_i5_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std3_dfd = {
	8,
	2,
	6,
	dec_std3_dfd_prg
};
static	int dec_std_4ln5_faaaaaffa_prg_i0_o[] = {
	3
};
static	int dec_std_4ln5_faaaaaffa_prg_i1_o[] = {
	9
};
static	int dec_std_4ln5_faaaaaffa_prg_i2_o[] = {
	10
};
static	int dec_std_4ln5_faaaaaffa_prg_i3_i[] = {
	0,	3,	10
};
static	int dec_std_4ln5_faaaaaffa_prg_i3_o[] = {
	-1,	4,	5
};
static	int dec_std_4ln5_faaaaaffa_prg_i4_i[] = {
	5,	3,	10
};
static	int dec_std_4ln5_faaaaaffa_prg_i4_o[] = {
	-1,	6,	7
};
static	int dec_std_4ln5_faaaaaffa_prg_i5_i[] = {
	1,	9,	7
};
static	int dec_std_4ln5_faaaaaffa_prg_i5_o[] = {
	-1,	8
};
static	int dec_std_4ln5_faaaaaffa_prg_i6_i[] = {
	2,	6
};
static	int dec_std_4ln5_faaaaaffa_prg_i7_i[] = {
	0,	4,	5,	6,	7,	8,	1
};
static struct sc_deconstruct_prg_item dec_std_4ln5_faaaaaffa_prg[] = {
	{
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_4ln5_faaaaaffa_prg_i0_o,
		"get_cpa"
	}, {
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_4ln5_faaaaaffa_prg_i1_o,
		"get_cna"
	}, {
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_4ln5_faaaaaffa_prg_i2_o,
		"get_0"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4ln5_faaaaaffa_prg_i3_i,
		3,
		dec_std_4ln5_faaaaaffa_prg_i3_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4ln5_faaaaaffa_prg_i4_i,
		3,
		dec_std_4ln5_faaaaaffa_prg_i4_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		3,
		dec_std_4ln5_faaaaaffa_prg_i5_i,
		2,
		dec_std_4ln5_faaaaaffa_prg_i5_o,
		"std3_faf"
	}, {
		SCD_FILTER,
		2,
		dec_std_4ln5_faaaaaffa_prg_i6_i,
		0,
		0,
		"not_in_set"
	}, {
		SCD_RETURN,
		7,
		dec_std_4ln5_faaaaaffa_prg_i7_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_4ln5_faaaaaffa = {
	3,
	8,
	8,
	dec_std_4ln5_faaaaaffa_prg
};
static	int dec_std_sely3_u1p2_prg_i0_i[] = {
	1,	2
};
static	int dec_std_sely3_u1p2_prg_i0_o[] = {
	6
};
static	int dec_std_sely3_u1p2_prg_i1_i[] = {
	6
};
static	int dec_std_sely3_u1p2_prg_i1_o[] = {
	5
};
static	int dec_std_sely3_u1p2_prg_i2_i[] = {
	5,	0
};
static	int dec_std_sely3_u1p2_prg_i3_i[] = {
	6
};
static	int dec_std_sely3_u1p2_prg_i3_o[] = {
	7
};
static	int dec_std_sely3_u1p2_prg_i4_i[] = {
	7,	4
};
static	int dec_std_sely3_u1p2_prg_i5_i[] = {
	3,	7
};
static	int dec_std_sely3_u1p2_prg_i6_i[] = {
	5,	6,	7
};
static struct sc_deconstruct_prg_item dec_std_sely3_u1p2_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely3_u1p2_prg_i0_i,
		1,
		dec_std_sely3_u1p2_prg_i0_o,
		"std_in_set"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely3_u1p2_prg_i1_i,
		1,
		dec_std_sely3_u1p2_prg_i1_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely3_u1p2_prg_i2_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std_sely3_u1p2_prg_i3_i,
		1,
		dec_std_sely3_u1p2_prg_i3_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely3_u1p2_prg_i4_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely3_u1p2_prg_i5_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_RETURN,
		3,
		dec_std_sely3_u1p2_prg_i6_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely3_u1p2 = {
	5,
	3,
	7,
	dec_std_sely3_u1p2_prg
};
static	int dec_std5_fafaa_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std5_fafaa_prg_i0_o[] = {
	-1,	5
};
static	int dec_std5_fafaa_prg_i1_i[] = {
	4,	3,	5
};
static	int dec_std5_fafaa_prg_i1_o[] = {
	7,	6
};
static	int dec_std5_fafaa_prg_i2_i[] = {
	0,	5,	2,	6,	7
};
static struct sc_deconstruct_prg_item dec_std5_fafaa_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std5_fafaa_prg_i0_i,
		2,
		dec_std5_fafaa_prg_i0_o,
		"std3_faf"
	}, {
		SCD_CONSTR,
		3,
		dec_std5_fafaa_prg_i1_i,
		2,
		dec_std5_fafaa_prg_i1_o,
		"std3_aaf"
	}, {
		SCD_RETURN,
		5,
		dec_std5_fafaa_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_fafaa = {
	5,
	3,
	3,
	dec_std5_fafaa_prg
};
static	int dec_std3l2_faaaf_prg_i0_i[] = {
	4
};
static	int dec_std3l2_faaaf_prg_i0_o[] = {
	-1,	7
};
static	int dec_std3l2_faaaf_prg_i1_i[] = {
	7
};
static	int dec_std3l2_faaaf_prg_i1_o[] = {
	6
};
static	int dec_std3l2_faaaf_prg_i2_i[] = {
	6
};
static	int dec_std3l2_faaaf_prg_i2_o[] = {
	-1,	5
};
static	int dec_std3l2_faaaf_prg_i3_i[] = {
	5
};
static	int dec_std3l2_faaaf_prg_i3_o[] = {
	8
};
static	int dec_std3l2_faaaf_prg_i4_i[] = {
	8,	0
};
static	int dec_std3l2_faaaf_prg_i5_i[] = {
	5,	1
};
static	int dec_std3l2_faaaf_prg_i6_i[] = {
	6,	2
};
static	int dec_std3l2_faaaf_prg_i7_i[] = {
	7,	3
};
static	int dec_std3l2_faaaf_prg_i8_i[] = {
	0,	5,	6,	7,	4
};
static struct sc_deconstruct_prg_item dec_std3l2_faaaf_prg[] = {
	{
		SCD_CONSTR,
		1,
		dec_std3l2_faaaf_prg_i0_i,
		2,
		dec_std3l2_faaaf_prg_i0_o,
		"all_input"
	}, {
		SCD_FUNC,
		1,
		dec_std3l2_faaaf_prg_i1_i,
		1,
		dec_std3l2_faaaf_prg_i1_o,
		"get_beg"
	}, {
		SCD_CONSTR,
		1,
		dec_std3l2_faaaf_prg_i2_i,
		2,
		dec_std3l2_faaaf_prg_i2_o,
		"all_input"
	}, {
		SCD_FUNC,
		1,
		dec_std3l2_faaaf_prg_i3_i,
		1,
		dec_std3l2_faaaf_prg_i3_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std3l2_faaaf_prg_i4_i,
		0,
		0,
		"equal2"
	}, {
		SCD_FILTER,
		2,
		dec_std3l2_faaaf_prg_i5_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FILTER,
		2,
		dec_std3l2_faaaf_prg_i6_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FILTER,
		2,
		dec_std3l2_faaaf_prg_i7_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		5,
		dec_std3l2_faaaf_prg_i8_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std3l2_faaaf = {
	5,
	4,
	9,
	dec_std3l2_faaaf_prg
};
static	int dec_arc_type_input_prg_i0_i[] = {
	1
};
static	int dec_arc_type_input_prg_i0_o[] = {
	-1,	2
};
static	int dec_arc_type_input_prg_i1_i[] = {
	2,	0
};
static	int dec_arc_type_input_prg_i2_i[] = {
	2,	1
};
static struct sc_deconstruct_prg_item dec_arc_type_input_prg[] = {
	{
		SCD_CONSTR,
		1,
		dec_arc_type_input_prg_i0_i,
		2,
		dec_arc_type_input_prg_i0_o,
		"all_input"
	}, {
		SCD_FILTER,
		2,
		dec_arc_type_input_prg_i1_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		2,
		dec_arc_type_input_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_arc_type_input = {
	2,
	1,
	3,
	dec_arc_type_input_prg
};
static	int dec_std_sely3_p1_prg_i0_i[] = {
	0,	1
};
static	int dec_std_sely3_p1_prg_i0_o[] = {
	6
};
static	int dec_std_sely3_p1_prg_i1_i[] = {
	6,	3,	5
};
static	int dec_std_sely3_p1_prg_i1_o[] = {
	-1,	7,	8
};
static	int dec_std_sely3_p1_prg_i2_i[] = {
	2,	7
};
static	int dec_std_sely3_p1_prg_i3_i[] = {
	4,	8
};
static	int dec_std_sely3_p1_prg_i4_i[] = {
	6,	7,	8
};
static struct sc_deconstruct_prg_item dec_std_sely3_p1_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std_sely3_p1_prg_i0_i,
		1,
		dec_std_sely3_p1_prg_i0_o,
		"std_in_set"
	}, {
		SCD_CONSTR,
		3,
		dec_std_sely3_p1_prg_i1_i,
		3,
		dec_std_sely3_p1_prg_i1_o,
		"std3_faa"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely3_p1_prg_i2_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_FILTER,
		2,
		dec_std_sely3_p1_prg_i3_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_RETURN,
		3,
		dec_std_sely3_p1_prg_i4_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_sely3_p1 = {
	6,
	3,
	5,
	dec_std_sely3_p1_prg
};
static	int dec_std_3l2_5faaaf_prg_i0_o[] = {
	5
};
static	int dec_std_3l2_5faaaf_prg_i1_o[] = {
	6
};
static	int dec_std_3l2_5faaaf_prg_i2_i[] = {
	5,	6,	0,	6,	1
};
static	int dec_std_3l2_5faaaf_prg_i2_o[] = {
	7
};
static	int dec_std_3l2_5faaaf_prg_i3_i[] = {
	5,	6,	7,	6,	2
};
static	int dec_std_3l2_5faaaf_prg_i3_o[] = {
	8
};
static	int dec_std_3l2_5faaaf_prg_i4_i[] = {
	3,	8
};
static	int dec_std_3l2_5faaaf_prg_i5_i[] = {
	8,	6,	5,	6,	4
};
static	int dec_std_3l2_5faaaf_prg_i5_o[] = {
	-1,	-1,	10
};
static	int dec_std_3l2_5faaaf_prg_i6_i[] = {
	10,	8,	7
};
static struct sc_deconstruct_prg_item dec_std_3l2_5faaaf_prg[] = {
	{
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_3l2_5faaaf_prg_i0_o,
		"get_0"
	}, {
		SCD_FUNC,
		0,
		0,
		1,
		dec_std_3l2_5faaaf_prg_i1_o,
		"get_cpa"
	}, {
		SCD_CONSTR,
		5,
		dec_std_3l2_5faaaf_prg_i2_i,
		1,
		dec_std_3l2_5faaaf_prg_i2_o,
		"std5_aafaf"
	}, {
		SCD_CONSTR,
		5,
		dec_std_3l2_5faaaf_prg_i3_i,
		1,
		dec_std_3l2_5faaaf_prg_i3_o,
		"std5_aafaf"
	}, {
		SCD_FILTER,
		2,
		dec_std_3l2_5faaaf_prg_i4_i,
		0,
		0,
		"is_in_set"
	}, {
		SCD_CONSTR,
		5,
		dec_std_3l2_5faaaf_prg_i5_i,
		3,
		dec_std_3l2_5faaaf_prg_i5_o,
		"std5_faaaf"
	}, {
		SCD_RETURN,
		3,
		dec_std_3l2_5faaaf_prg_i6_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_3l2_5faaaf = {
	5,
	6,
	7,
	dec_std_3l2_5faaaf_prg
};
static	int dec_std_ord_bin_conn2_prg_i0_i[] = {
	2,	3,	4,	7,	8
};
static	int dec_std_ord_bin_conn2_prg_i0_o[] = {
	12,	13,	-1,	15
};
static	int dec_std_ord_bin_conn2_prg_i1_i[] = {
	0,	1,	12
};
static	int dec_std_ord_bin_conn2_prg_i1_o[] = {
	-1,	11
};
static	int dec_std_ord_bin_conn2_prg_i2_i[] = {
	12,	5,	6,	9,	10
};
static	int dec_std_ord_bin_conn2_prg_i2_o[] = {
	-1,	14,	-1,	16
};
static	int dec_std_ord_bin_conn2_prg_i3_i[] = {
	0,	11,	12,	13,	4,	14,	6,	15,	8,
	16,	10
};
static struct sc_deconstruct_prg_item dec_std_ord_bin_conn2_prg[] = {
	{
		SCD_CONSTR,
		5,
		dec_std_ord_bin_conn2_prg_i0_i,
		4,
		dec_std_ord_bin_conn2_prg_i0_o,
		"std5_aafaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std_ord_bin_conn2_prg_i1_i,
		2,
		dec_std_ord_bin_conn2_prg_i1_o,
		"std3_faf"
	}, {
		SCD_CONSTR,
		5,
		dec_std_ord_bin_conn2_prg_i2_i,
		4,
		dec_std_ord_bin_conn2_prg_i2_o,
		"std5_fafaf"
	}, {
		SCD_RETURN,
		11,
		dec_std_ord_bin_conn2_prg_i3_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_ord_bin_conn2 = {
	11,
	6,
	4,
	dec_std_ord_bin_conn2_prg
};
static	int dec_std5_fafaf_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std5_fafaf_prg_i0_o[] = {
	-1,	5
};
static	int dec_std5_fafaf_prg_i1_i[] = {
	4,	3,	5
};
static	int dec_std5_fafaf_prg_i1_o[] = {
	-1,	6
};
static	int dec_std5_fafaf_prg_i2_i[] = {
	0,	5,	2,	6,	4
};
static struct sc_deconstruct_prg_item dec_std5_fafaf_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std5_fafaf_prg_i0_i,
		2,
		dec_std5_fafaf_prg_i0_o,
		"std3_faf"
	}, {
		SCD_CONSTR,
		3,
		dec_std5_fafaf_prg_i1_i,
		2,
		dec_std5_fafaf_prg_i1_o,
		"std3_faf"
	}, {
		SCD_RETURN,
		5,
		dec_std5_fafaf_prg_i2_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_fafaf = {
	5,
	2,
	3,
	dec_std5_fafaf_prg
};
static	int dec_std_ord_bin_conn1_def_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std_ord_bin_conn1_def_prg_i0_o[] = {
	-1,	9,	10
};
static	int dec_std_ord_bin_conn1_def_prg_i1_i[] = {
	10,	3,	4,	7,	8
};
static	int dec_std_ord_bin_conn1_def_prg_i1_o[] = {
	-1,	11,	-1,	14
};
static	int dec_std_ord_bin_conn1_def_prg_i2_i[] = {
	10,	5,	6
};
static	int dec_std_ord_bin_conn1_def_prg_i2_o[] = {
	-1,	12,	13
};
static	int dec_std_ord_bin_conn1_def_prg_i3_i[] = {
	0,	9,	10,	11,	4,	12,	13,	14,	8
};
static struct sc_deconstruct_prg_item dec_std_ord_bin_conn1_def_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std_ord_bin_conn1_def_prg_i0_i,
		3,
		dec_std_ord_bin_conn1_def_prg_i0_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		5,
		dec_std_ord_bin_conn1_def_prg_i1_i,
		4,
		dec_std_ord_bin_conn1_def_prg_i1_o,
		"std5_fafaf"
	}, {
		SCD_CONSTR,
		3,
		dec_std_ord_bin_conn1_def_prg_i2_i,
		3,
		dec_std_ord_bin_conn1_def_prg_i2_o,
		"std3_faa"
	}, {
		SCD_RETURN,
		9,
		dec_std_ord_bin_conn1_def_prg_i3_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_ord_bin_conn1_def = {
	9,
	6,
	4,
	dec_std_ord_bin_conn1_def_prg
};
static	int dec_std3_faa_prg_i0_i[] = {
	0,	1
};
static	int dec_std3_faa_prg_i0_o[] = {
	-1,	3
};
static	int dec_std3_faa_prg_i1_i[] = {
	3
};
static	int dec_std3_faa_prg_i1_o[] = {
	4
};
static	int dec_std3_faa_prg_i2_i[] = {
	4,	2
};
static	int dec_std3_faa_prg_i3_i[] = {
	0,	3,	4
};
static struct sc_deconstruct_prg_item dec_std3_faa_prg[] = {
	{
		SCD_CONSTR,
		2,
		dec_std3_faa_prg_i0_i,
		2,
		dec_std3_faa_prg_i0_o,
		"arc_type_output"
	}, {
		SCD_FUNC,
		1,
		dec_std3_faa_prg_i1_i,
		1,
		dec_std3_faa_prg_i1_o,
		"get_end"
	}, {
		SCD_FILTER,
		2,
		dec_std3_faa_prg_i2_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		3,
		dec_std3_faa_prg_i3_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std3_faa = {
	3,
	2,
	4,
	dec_std3_faa_prg
};
static	int dec_std_ord_bin_conn1_prg_i0_i[] = {
	0,	1,	2
};
static	int dec_std_ord_bin_conn1_prg_i0_o[] = {
	-1,	11,	12
};
static	int dec_std_ord_bin_conn1_prg_i1_i[] = {
	12,	3,	4,	7,	8
};
static	int dec_std_ord_bin_conn1_prg_i1_o[] = {
	-1,	13,	-1,	16
};
static	int dec_std_ord_bin_conn1_prg_i2_i[] = {
	12,	5,	6,	9,	10
};
static	int dec_std_ord_bin_conn1_prg_i2_o[] = {
	-1,	14,	15,	17
};
static	int dec_std_ord_bin_conn1_prg_i3_i[] = {
	0,	11,	12,	13,	4,	14,	15,	16,	8,
	17,	10
};
static struct sc_deconstruct_prg_item dec_std_ord_bin_conn1_prg[] = {
	{
		SCD_CONSTR,
		3,
		dec_std_ord_bin_conn1_prg_i0_i,
		3,
		dec_std_ord_bin_conn1_prg_i0_o,
		"std3_faa"
	}, {
		SCD_CONSTR,
		5,
		dec_std_ord_bin_conn1_prg_i1_i,
		4,
		dec_std_ord_bin_conn1_prg_i1_o,
		"std5_fafaf"
	}, {
		SCD_CONSTR,
		5,
		dec_std_ord_bin_conn1_prg_i2_i,
		4,
		dec_std_ord_bin_conn1_prg_i2_o,
		"std5_faaaf"
	}, {
		SCD_RETURN,
		11,
		dec_std_ord_bin_conn1_prg_i3_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std_ord_bin_conn1 = {
	11,
	7,
	4,
	dec_std_ord_bin_conn1_prg
};
static	int dec_std5_3_aaaafaf_prg_i0_i[] = {
	6
};
static	int dec_std5_3_aaaafaf_prg_i0_o[] = {
	-1,	12
};
static	int dec_std5_3_aaaafaf_prg_i1_i[] = {
	12,	5
};
static	int dec_std5_3_aaaafaf_prg_i2_i[] = {
	12
};
static	int dec_std5_3_aaaafaf_prg_i2_o[] = {
	9
};
static	int dec_std5_3_aaaafaf_prg_i3_i[] = {
	9,	2
};
static	int dec_std5_3_aaaafaf_prg_i4_i[] = {
	9
};
static	int dec_std5_3_aaaafaf_prg_i4_o[] = {
	-1,	8
};
static	int dec_std5_3_aaaafaf_prg_i5_i[] = {
	8,	1
};
static	int dec_std5_3_aaaafaf_prg_i6_i[] = {
	8,	7
};
static	int dec_std5_3_aaaafaf_prg_i7_i[] = {
	7,	0
};
static	int dec_std5_3_aaaafaf_prg_i8_i[] = {
	8
};
static	int dec_std5_3_aaaafaf_prg_i8_o[] = {
	-1,	10
};
static	int dec_std5_3_aaaafaf_prg_i9_i[] = {
	10
};
static	int dec_std5_3_aaaafaf_prg_i9_o[] = {
	11
};
static	int dec_std5_3_aaaafaf_prg_i10_i[] = {
	11,	4
};
static	int dec_std5_3_aaaafaf_prg_i11_i[] = {
	10,	3
};
static	int dec_std5_3_aaaafaf_prg_i12_i[] = {
	7,	8,	9,	10,	4,	12,	6
};
static struct sc_deconstruct_prg_item dec_std5_3_aaaafaf_prg[] = {
	{
		SCD_CONSTR,
		1,
		dec_std5_3_aaaafaf_prg_i0_i,
		2,
		dec_std5_3_aaaafaf_prg_i0_o,
		"all_input"
	}, {
		SCD_FILTER,
		2,
		dec_std5_3_aaaafaf_prg_i1_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		1,
		dec_std5_3_aaaafaf_prg_i2_i,
		1,
		dec_std5_3_aaaafaf_prg_i2_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std5_3_aaaafaf_prg_i3_i,
		0,
		0,
		"check_type"
	}, {
		SCD_CONSTR,
		1,
		dec_std5_3_aaaafaf_prg_i4_i,
		2,
		dec_std5_3_aaaafaf_prg_i4_o,
		"all_input"
	}, {
		SCD_FILTER,
		2,
		dec_std5_3_aaaafaf_prg_i5_i,
		0,
		0,
		"check_type"
	}, {
		SCD_FUNC,
		2,
		dec_std5_3_aaaafaf_prg_i6_i,
		0,
		0,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std5_3_aaaafaf_prg_i7_i,
		0,
		0,
		"check_type"
	}, {
		SCD_CONSTR,
		1,
		dec_std5_3_aaaafaf_prg_i8_i,
		2,
		dec_std5_3_aaaafaf_prg_i8_o,
		"all_input"
	}, {
		SCD_FUNC,
		1,
		dec_std5_3_aaaafaf_prg_i9_i,
		1,
		dec_std5_3_aaaafaf_prg_i9_o,
		"get_beg"
	}, {
		SCD_FILTER,
		2,
		dec_std5_3_aaaafaf_prg_i10_i,
		0,
		0,
		"equal2"
	}, {
		SCD_FILTER,
		2,
		dec_std5_3_aaaafaf_prg_i11_i,
		0,
		0,
		"check_type"
	}, {
		SCD_RETURN,
		7,
		dec_std5_3_aaaafaf_prg_i12_i,
		0,
		0,
		0
	}
};
static struct sc_deconstruct dec_std5_3_aaaafaf = {
	7,
	6,
	13,
	dec_std5_3_aaaafaf_prg
};

//typedef bool		(*sc_filter)(sc_param *regs,int cnt,int input[]);
//typedef sc_retval	(*sc_func)(sc_param *regs,int cnt,int input[],int output);

// standart filters and functions
static	bool		filter_check_type(sc_param *regs,int cnt,int input[]);
static	bool		filter_equal2(sc_param *regs,int cnt,int input[]);
static	bool		filter_equal_with_d(sc_param *regs,int cnt,int input[]);
static	bool		filter_is_in_set(sc_param *regs,int cnt,int input[]);
static  bool		filter_differ(sc_param *regs,int cnt,int input[]);
static  bool		filter_not_in_set(sc_param *regs,int cnt,int input[]);
static	sc_retval	func_get_beg(sc_param *regs,int cnt,int input[],int output);
static	sc_retval	func_get_end(sc_param *regs,int cnt,int input[],int output);
static	sc_retval	func_get_arc_pos_const(sc_param *regs,int cnt,int input[],int output);
static  sc_retval	func_get_arc_neg_const(sc_param *regs,int cnt,int input[],int output);
static	sc_retval	func_get_0(sc_param *regs,int cnt,int input[],int output);

static struct sc_filter_info
	__check_type = {0,"check_type",filter_check_type},
	__equal2 = {0,"equal2",filter_equal2},
	__equal_with_d = {0,"equal_with_d",filter_equal_with_d},
	__differ = {0,"differ",filter_differ},
	__is_in_set = {0,"is_in_set",filter_is_in_set},
	__not_in_set = {0,"not_in_set",filter_not_in_set};

static struct sc_func_info
	__get_beg = {0,"get_beg",func_get_beg},
	__get_end = {0,"get_end",func_get_end},
	__get_arc_pos_const = {0,"get_cpa",func_get_arc_pos_const},
	__get_arc_neg_const = {0,"get_cna",func_get_arc_neg_const},
	__get_0 = {0,"get_0",func_get_0};

bool	filter_differ(sc_param *regs,int cnt,int input[])
{
	sc_addr a1,a2;
	if (cnt<2)
		SC_ABORT(); // really bad choice
	a1 = regs[input[0]].addr;
	a2 = regs[input[1]].addr;
	return a1 != a2;
}

bool	filter_check_type(sc_param *regs,int cnt,int input[])
{
	sc_type mask;
	sc_addr	addr;
	if (cnt<2)
		SC_ABORT();
	mask = regs[input[1]].type;
	addr = regs[input[0]].addr;
	return check_sc_type(system_session->get_type(addr),mask);
}
// order is addr1,candidate,d,mask
bool	filter_equal_with_d(sc_param *regs,int cnt,int input[])
{
	int d;
	sc_type mask;
	sc_addr addr1,addr2;
	addr1 = regs[input[0]].addr;
	addr2 = regs[input[1]].addr;
	d = regs[input[2]].i;
	mask = regs[input[3]].type;
	if (d) {
		return addr1 == addr2;
	}
	return check_sc_type(addr2->seg->get_type(addr2->impl),mask);
}
bool	filter_equal2(sc_param *regs,int cnt,int input[])
{
	sc_addr a1,a2;
	if (cnt<2)
		SC_ABORT();
	a1 = regs[input[0]].addr;
	a2 = regs[input[1]].addr;
	return a1 == a2;
}
bool filter_not_in_set(sc_param *regs,int cnt,int input[])
{
	sc_addr set,element;
	set = regs[input[0]].addr;
	element = regs[input[1]].addr;
	if (!set)
		return true;
	sc_iterator *iter = system_session->create_iterator(sc_constraint_new(
				CONSTR_3_f_a_f,
				set,
				SC_A_CONST|SC_POS,
				element
			),true);
	if (iter->is_over()) {
		delete iter;
		return true;
	}
	delete iter;
	return false;
}
// params are set,element
bool	filter_is_in_set(sc_param *regs,int cnt,int input[])
{
	sc_addr set,element;
	set = regs[input[0]].addr;
	element = regs[input[1]].addr;
	if (!set)
		return true;
	sc_iterator *iter = system_session->create_iterator(sc_constraint_new(
				CONSTR_3_f_a_f,
				set,
				SC_A_CONST|SC_POS,
				element
			),true);
	if (iter->is_over()) {
		delete iter;
		return false;
	}
	delete iter;
	return true;
}

sc_retval	func_get_beg(sc_param *regs,int cnt,int input[],int output)
{
	sc_addr arc;
	arc = regs[input[0]].addr;
	if (!(regs[output].addr = system_session->get_beg(arc)))
		return RV_ERR_GEN;
	return RV_OK;
}
sc_retval	func_get_end(sc_param *regs,int cnt,int input[],int output)
{
	sc_addr arc;
	arc = regs[input[0]].addr;
	if (!(regs[output].addr = system_session->get_end(arc)))
		return RV_ERR_GEN;
	return RV_OK;
}
sc_retval	func_get_arc_pos_const(sc_param *regs,int cnt,int input[],int output)
{
	regs[output].type = SC_CONST|SC_POS|SC_ARC_ACCESSORY;
	return RV_OK;
}
sc_retval	func_get_arc_neg_const(sc_param *regs,int cnt,int input[],int output)
{
	regs[output].type = SC_CONST|SC_NEG|SC_ARC_ACCESSORY;
	return RV_OK;
}
sc_retval	func_get_0(sc_param *regs,int cnt,int input[],int output)
{
	regs[output].i = 0;
	return RV_OK;
}


#define REGISTER_CONSTRAINT(place,constraint,dec) \
	do { \
		constraint.deconstruct = &dec; \
		if (sc_constraint_register(&constraint)) \
			SC_ABORT(); \
		place = constraint.id; \
	} while(0)
#define REGISTER_FUNC(func) \
	do {\
		if (sc_constraint_register_function(&func)) \
			SC_ABORT(); \
	} while(0)
#define REGISTER_FILT(filt) \
	do {\
		if (sc_constraint_register_filter(&filt)) \
			SC_ABORT(); \
	} while(0)

#define DECONSTRUCT_NIL ((*(sc_deconstruct *)(0)))


void	__postinit_std_constraints(sc_session *system)
{
//	system_session = system;
}

void	__init_std_constraints()
{
	REGISTER_FUNC(__get_beg);
	REGISTER_FUNC(__get_end);
	REGISTER_FUNC(__get_arc_pos_const);
	REGISTER_FUNC(__get_arc_neg_const);
	REGISTER_FUNC(__get_0);
	REGISTER_FILT(__check_type);
	REGISTER_FILT(__equal2);
	REGISTER_FILT(__equal_with_d);
	REGISTER_FILT(__is_in_set);
	REGISTER_FILT(__not_in_set);
	REGISTER_FILT(__differ);
	REGISTER_CONSTRAINT(__constr_all_input,all_input,DECONSTRUCT_NIL);
	REGISTER_CONSTRAINT(__constr_all_output,all_output,DECONSTRUCT_NIL);
	REGISTER_CONSTRAINT(__constr_arc_type_input,arc_type_input,dec_arc_type_input);
	REGISTER_CONSTRAINT(__constr_arc_type_output,arc_type_output,dec_arc_type_output);
	REGISTER_CONSTRAINT(__constr_3faa,std3_faa,dec_std3_faa);
	REGISTER_CONSTRAINT(__constr_3faf,std3_faf,dec_std3_faf);

	REGISTER_CONSTRAINT(__constr_3aaf,std3_aaf,dec_std3_aaf);
	REGISTER_CONSTRAINT(__constr_3dfd,std3_dfd,dec_std3_dfd);
	REGISTER_CONSTRAINT(__constr_5faaaa,std5_faaaa,dec_std5_faaaa);

#if 0
	REGISTER_CONSTRAINT(__constr_5faaaf,std5_faaaf,dec_std5_faaaf);
#else
	std5_faaaf.factory = std5_f_a_a_a_f_factory;
	std5_faaaf.deconstruct = &dec_std5_faaaf;
	if (sc_constraint_register(&std5_faaaf))
		SC_ABORT();
	__constr_5faaaf = std5_faaaf.id;
#endif

	std_on_segment.factory = std_on_segment_factory;
	std_on_segment.deconstruct = 0;
	if (sc_constraint_register(&std_on_segment))
		SC_ABORT();
	__constr_on_segment = std_on_segment.id;

	REGISTER_CONSTRAINT(__constr_5fafaa,std5_fafaa,dec_std5_fafaa);
	REGISTER_CONSTRAINT(__constr_5fafaf,std5_fafaf,dec_std5_fafaf);
	REGISTER_CONSTRAINT(__constr_5aaaaf,std5_aaaaf,dec_std5_aaaaf);
	REGISTER_CONSTRAINT(__constr_5aafaa,std5_aafaa,dec_std5_aafaa);
	REGISTER_CONSTRAINT(__constr_5aafaf,std5_aafaf,dec_std5_aafaf);
	REGISTER_CONSTRAINT(__constr_ord_bin_conn1,std_ord_bin_conn1,dec_std_ord_bin_conn1);
	REGISTER_CONSTRAINT(__constr_ord_bin_conn1_def,std_ord_bin_conn1_def,dec_std_ord_bin_conn1_def);
	REGISTER_CONSTRAINT(__constr_ord_bin_conn2,std_ord_bin_conn2,dec_std_ord_bin_conn2);
	REGISTER_CONSTRAINT(__constr_3l2_faaaf,std3l2_faaaf,dec_std3l2_faaaf);
	REGISTER_CONSTRAINT(__constr_5_3_aaaafaf,std5_3_aaaafaf,dec_std5_3_aaaafaf);
	REGISTER_CONSTRAINT(__constr_in_set,std_in_set,dec_std_in_set);
	REGISTER_CONSTRAINT(__constr_sely3_p1,std_sely3_p1,dec_std_sely3_p1);
	REGISTER_CONSTRAINT(__constr_sely3_u1p2,std_sely3_u1p2,dec_std_sely3_u1p2);
	REGISTER_CONSTRAINT(__constr_sely3_u1u2p3,std_sely3_u1u2p3,dec_std_sely3_u1u2p3);
	REGISTER_CONSTRAINT(__constr_sely5_p1,std_sely5_p1,dec_std_sely5_p1);
	REGISTER_CONSTRAINT(__constr_sely5_u1p2,std_sely5_u1p2,dec_std_sely5_u1p2);
	REGISTER_CONSTRAINT(__constr_sely5_u1u2p3,std_sely5_u1u2p3,dec_std_sely5_u1u2p3);
	REGISTER_CONSTRAINT(__constr_sely5_u1u2u3p4,std_sely5_u1u2u3p4,dec_std_sely5_u1u2u3p4);
	REGISTER_CONSTRAINT(__constr_sely5_u1u2u3u4p5,std_sely5_u1u2u3u4p5,dec_std_sely5_u1u2u3u4p5);
	REGISTER_CONSTRAINT(__constr_intersect2,std_intersect2,dec_std_intersect2);
	REGISTER_CONSTRAINT(__constr_bin_conn_unord1,std_bin_conn_unord1,dec_std_bin_conn_unord1);
	REGISTER_CONSTRAINT(__constr_3l2_5faaaf,std_3l2_5faaaf,dec_std_3l2_5faaaf);
	REGISTER_CONSTRAINT(__constr_4ln5_faaaaaffa,std_4ln5_faaaaaffa,dec_std_4ln5_faaaaaffa);
	REGISTER_CONSTRAINT(__constr_4l5_faaaaaffa,std_4l5_faaaaaffa,dec_std_4l5_faaaaaffa);
	REGISTER_CONSTRAINT(__constr_ord_bin_conn1a,std_ord_bin_conn1a,dec_std_ord_bin_conn1a);
}
