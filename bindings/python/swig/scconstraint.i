
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

%module scconstraint
%{
#include <libsc/sc_constraint.h>
%}
%include "carrays.i"


class sc_session;

/// Type of parameter for #sc_constraint.
enum sc_param_spec
{
	SC_PD_TYPE,
	SC_PD_ADDR,
	SC_PD_INT,
	SC_PD_ADDR_0, ///< Address which is not checked by constraint checker.
	SC_PD_SET,
	SC_PD_SEGMENT,
	SC_PD_BOOLEAN
};

%array_class(sc_param_spec, sc_param_specArray);
%array_class(sc_param, sc_paramArray);

struct	sc_constraint {
	sc_uint		id;
	sc_param	params[1];
	/*void params_set(sc_type type,sc_addr addr,int i,sc_segment *seg)
	{
		   params[0].type = type;
		   params[0].addr = addr;
		   params[0].i = i;
		   params[0].seg = seg;
	}*/
};

class sc_iterator;

typedef bool		(*sc_filter)(sc_param *regs,int cnt,int input[]);
typedef sc_retval	(*sc_func)(sc_param *regs,int cnt,int input[],int output);

enum sc_deconstruct_item_type {
	SCD_RETURN = 0,
	SCD_FILTER,
	SCD_CONSTR,
	SCD_FUNC,
	SCD_SET_EACH
};

// when deconstruct is realized field id gets filled.
struct sc_deconstruct_prg_item {
	sc_deconstruct_item_type type;
	int in_cnt;
	int *params_in;
	int out_cnt;
	int *params_out;
	char *name;
	int id;
};

// last item of every deconstruct must be return
struct	sc_deconstruct {
	int input_cnt;
	int reg_cnt;
	int commands_len;
	struct sc_deconstruct_prg_item *commands;
};

typedef	sc_iterator *(*custom_iter_factory)(sc_session *s,sc_constraint *);

struct	sc_constraint_info {
	sc_uint id;
	char *name;
	int input_cnt;

	sc_param_spec *input_spec;

	int fix_cnt;
	int *fix_params;
	custom_iter_factory factory;

	sc_deconstruct *deconstruct;
};

struct	sc_filter_info {
	sc_uint id;
	char *name;
	sc_filter filter;
};

struct	sc_func_info {
	sc_uint id;
	char *name;
	sc_func func;
};


LIBSC_API void		sc_constraint_init();

LIBSC_API void		sc_constraint_done();

LIBSC_API sc_retval	sc_constraint_register(sc_constraint_info *);

LIBSC_API sc_retval	sc_constraint_register_filter(sc_filter_info *);
LIBSC_API sc_retval	sc_constraint_register_function(sc_func_info *);

LIBSC_API sc_iterator	*sc_constraint_create_iterator(sc_session *s,sc_constraint *constr,bool sink);




LIBSC_API sc_constraint *	sc_constraint_alloc(sc_uint id);
LIBSC_API sc_constraint * sc_constraint_new(sc_uint id,...);
LIBSC_API sc_constraint * sc_constraint_new_by_name(char *name,...);
LIBSC_API sc_constraint * sc_constraint_new_valist(sc_uint id,va_list list);
LIBSC_API sc_constraint * sc_constraint_clone(sc_constraint *);
LIBSC_API sc_constraint * sc_constraint_newv(sc_uint id,sc_param *regs,int cnt,int input[]);
LIBSC_API sc_constraint * sc_constraint_newa(sc_uint id,sc_param *regs,int cnt);

LIBSC_API sc_constraint * __sc_constraint_new(sc_constraint_info * info);


LIBSC_API void 		sc_constraint_free(sc_constraint *);
LIBSC_API sc_constraint_info *sc_constraint_get_info(sc_uint id);
LIBSC_API int		sc_constraint_get_id_by_name(char *name);
LIBSC_API int		sc_constraint_get_filter_id_by_name(char *name);
LIBSC_API int		sc_constraint_get_function_id_by_name(char *name);
LIBSC_API sc_filter	sc_constraint_get_filter(sc_uint id);
LIBSC_API sc_filter_info *sc_constraint_get_filter_info(sc_uint id);
LIBSC_API sc_func		sc_constraint_get_function(sc_uint id);
LIBSC_API sc_func_info   *sc_constraint_get_function_info(sc_uint id);
LIBSC_API sc_constraint_info *sc_constraint_compile_set_constraint(sc_constraint_info *info, unsigned set_params);
LIBSC_API void sc_constraint_register_set_constraint(sc_constraint_info *original, sc_constraint_info *info, unsigned set_params);
LIBSC_API sc_constraint_info *sc_constraint_find_set_constraint(sc_constraint_info *orig_info, unsigned set_params);





