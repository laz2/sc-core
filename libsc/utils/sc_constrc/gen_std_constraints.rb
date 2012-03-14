#--
# This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
# For the latest info, see http://www.ostis.net
#
# Copyright (c) 2012 OSTIS
#
# OSTIS is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OSTIS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
#++

require './dsl'

include SCConstrCompiler::DSL

header <<EOF
#include <stdlib.h>

#include <libsc/std_constraints.h>
#include <libsc/sc_constraint.h>
#include <libsc/sc_types.h>
#include <libsc/abort.h>
#include <libsc/custom_iters_p.h>
EOF

def_filter :check_type
def_filter :equal2
def_filter :equal_with_d
def_filter :is_in_set
def_filter :differ
def_filter :not_in_set

def_func :get_beg
def_func :get_end
def_func :get_arc_const_pos
def_func :get_arc_neg_const
def_func :get_0

def_constr :all_input, sc_addr!(:e1)
def_constr :all_output, sc_addr!(:e1)

def_constr :arc_type_input, sc_type(:t1), sc_addr!(:e2) do
  var :r1
  constr :all_output, [e2] => [_, r1]
  filter :check_type, r1, t1
  return_with r1, e2
end

#def_constr :arc_type_output, sc_addr!(:e1), sc_type(:t2) do
#  var :r2
#  constr :all_output, [:e1] => [:_, :r2]
#  filter :check_type, :r2, :t2
#  return_with :e1, :r2
#end
#
#def_constr :std3_faa, sc_addr!(:e1), sc_type(:t2), sc_type(:t3) do
#  var :e2, :e3
#  constr :all_output, [:e1, :t2] => [:_, :e2]
#  func :get_end, [:e2] => [:e3]
#  filter :check_type, :e3, :t3
#  return_with :e1, :e2, :e3
#end
#
#def_constr :std3_faf, sc_addr!(:e1), sc_type(:t2), sc_addr!(:e3) do
#  var :e2, :ce1
#  constr :all_input, [:e3] => [:_, :e2]
#  func :get_end, [:e2] => [:ce1]
#  filter :equal2, :e1, :ce1
#  filter :check_type, :e2, :t2
#  return_with :e1, :e2, :e3
#end

#SCConstraintCompiler.dump

footer <<EOF
// standart filters and functions
static bool filter_check_type(sc_param *regs,int cnt,int input[]);
static bool filter_equal2(sc_param *regs,int cnt,int input[]);
static bool filter_equal_with_d(sc_param *regs,int cnt,int input[]);
static bool filter_is_in_set(sc_param *regs,int cnt,int input[]);
static bool filter_differ(sc_param *regs,int cnt,int input[]);
static bool filter_not_in_set(sc_param *regs,int cnt,int input[]);
static sc_retval func_get_beg(sc_param *regs,int cnt,int input[],int output);
static sc_retval func_get_end(sc_param *regs,int cnt,int input[],int output);
static sc_retval func_get_arc_pos_const(sc_param *regs,int cnt,int input[],int output);
static sc_retval func_get_arc_neg_const(sc_param *regs,int cnt,int input[],int output);
static sc_retval func_get_0(sc_param *regs,int cnt,int input[],int output);

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

bool filter_differ(sc_param *regs,int cnt,int input[])
{
  sc_addr a1,a2;
  if (cnt<2)
    SC_ABORT(); // really bad choice
  a1 = regs[input[0]].addr;
  a2 = regs[input[1]].addr;
  return a1 != a2;
}

bool filter_check_type(sc_param *regs,int cnt,int input[])
{
  sc_type mask;
  sc_addr  addr;
  if (cnt<2)
    SC_ABORT();
  mask = regs[input[1]].type;
  addr = regs[input[0]].addr;
  return check_sc_type(system_session->get_type(addr),mask);
}

// order is addr1,candidate,d,mask
bool filter_equal_with_d(sc_param *regs,int cnt,int input[])
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

bool filter_equal2(sc_param *regs,int cnt,int input[])
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
bool filter_is_in_set(sc_param *regs,int cnt,int input[])
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

sc_retval func_get_beg(sc_param *regs,int cnt,int input[],int output)
{
  sc_addr arc;
  arc = regs[input[0]].addr;
  if (!(regs[output].addr = system_session->get_beg(arc)))
    return RV_ERR_GEN;
  return RV_OK;
}

sc_retval func_get_end(sc_param *regs,int cnt,int input[],int output)
{
  sc_addr arc;
  arc = regs[input[0]].addr;
  if (!(regs[output].addr = system_session->get_end(arc)))
    return RV_ERR_GEN;
  return RV_OK;
}

sc_retval func_get_arc_pos_const(sc_param *regs,int cnt,int input[],int output)
{
  regs[output].type = SC_CONST|SC_POS|SC_ARC_ACCESSORY;
  return RV_OK;
}

sc_retval func_get_arc_neg_const(sc_param *regs,int cnt,int input[],int output)
{
  regs[output].type = SC_CONST|SC_NEG|SC_ARC_ACCESSORY;
  return RV_OK;
}

sc_retval  func_get_0(sc_param *regs,int cnt,int input[],int output)
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


void __postinit_std_constraints(sc_session *system)
{
  // system_session = system;
}

void  __init_std_constraints()
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
EOF

require './generator'

puts SCConstrCompiler::Generator.process(:Structs)
