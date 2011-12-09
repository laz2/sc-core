
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
 * @file sc_constraint.cpp
 * @brief Constraint system implementation.
 * @author Alexey Kondratenko
 */

#include "precompiled_p.h"

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

struct holder {
	sc_deconstruct_item_type type;
	union {
		sc_constraint_info *constr;
		sc_func_info *func;
		sc_filter_info *filt;
	} u;
};

// yet maximum number of registered entities is limited
#define MAX_REG_ITEMS	8192
static 	holder	registry[MAX_REG_ITEMS];
int	reg_cnt;

static	bool ca_free=true;
// int to get allignment
static
int	const_alloc_buf[1024];

static sc_deconstruct *inline_deconstruct(sc_deconstruct *dec);

struct constraint_less
{
	bool operator()(char * const &_X, char * const &_Y) const
	{
		return strcmp(_X,_Y) < 0;
	}
};

typedef std::pair<char *, holder *> my_pair_t;
typedef std::map<char *, holder *, constraint_less> my_hash_t;

static my_hash_t my_hash;

void sc_constraint_init()
{
	memset(registry,0,sizeof(registry));
	reg_cnt = 0;
}

void sc_constraint_done()
{
	my_hash.clear();
	for (int i = 0; i < reg_cnt; ++i)
		if (registry[i].type == SCD_CONSTR) {
			sc_deconstruct *deconstr = registry[i].u.constr->deconstruct;

			if (deconstr) {
				sc_deconstruct_prg_item *commands = deconstr->commands;

				for (int j = 0; j < deconstr->commands_len; ++j) {
					delete[] commands[j].params_in;
					delete[] commands[j].params_out;
				}

				delete[] commands;
				delete deconstr;
			}
		}

	// FIXME: i don't free constraint infos from #set_constraint_registry.
}

// do not delete this comment
// it's comment only to not confuse gcc with unused static function
/*
static
void	dump_holder_names()
{
	my_hash_t::iterator iter = my_hash.begin();
	while (iter != my_hash.end()) {
		printf("%s\n",(*iter).first);
		iter++;
	}
}*/

static
int find_holder_id_by_name(char *name)
{
	int id;
	my_hash_t::iterator iter = my_hash.find(name);
	if (iter == my_hash.end())
		return -1;
	id = (*iter).second - registry;
	if (id<0 || id>=MAX_REG_ITEMS)
		return -1;
	return id;
}

static
sc_retval sc_deconstruct_realize(sc_deconstruct *dec)
{
	if (!dec)
		return RV_OK;
	int i = dec->commands_len-1;
	//dump_holder_names();
	for (;i>=0;i--) {
		if (dec->commands[i].type != SCD_RETURN) {
			int id = find_holder_id_by_name(dec->commands[i].name);
			if (id<0)
				return RV_ERR_GEN;
			dec->commands[i].id = id;
		}
	}
	return RV_OK;
}

static
sc_retval __sc_constraint_register(sc_constraint_info *info)
{
	registry[reg_cnt].type = SCD_CONSTR;
	registry[reg_cnt].u.constr = info;
	if (info->name && !my_hash.insert(my_pair_t(info->name,registry+reg_cnt)).second)
		return RV_ERR_GEN;
	info->id = reg_cnt++;
	return RV_OK;
}

sc_retval sc_constraint_register(sc_constraint_info *info)
{
	if (info->deconstruct) {
		if (sc_deconstruct_realize(info->deconstruct) != RV_OK)
			return RV_ERR_GEN;
		info->deconstruct = inline_deconstruct(info->deconstruct);
	}
	if (find_holder_id_by_name(info->name)>=0)
		return RV_ERR_GEN;
	return __sc_constraint_register(info);
}

sc_retval sc_constraint_register_filter(sc_filter_info *info)
{
	if (find_holder_id_by_name(info->name)>=0)
		return RV_ERR_GEN;
	registry[reg_cnt].type = SCD_FILTER;
	registry[reg_cnt].u.filt = info;
	if (!my_hash.insert(my_pair_t(info->name,registry+reg_cnt)).second)
		return RV_ERR_GEN;
	info->id = reg_cnt++;
	return RV_OK;
}

sc_retval sc_constraint_register_function(sc_func_info *info)
{
	if (find_holder_id_by_name(info->name)>=0)
		return RV_ERR_GEN;
	registry[reg_cnt].type = SCD_FUNC;
	registry[reg_cnt].u.func = info;
	if (!my_hash.insert(my_pair_t(info->name,registry+reg_cnt)).second)
		return RV_ERR_GEN;
	info->id = reg_cnt++;
	return RV_OK;
}

sc_constraint_info *sc_constraint_get_info(sc_uint id)
{
	if (id < MAX_REG_ITEMS && registry[id].type == SCD_CONSTR)
		return registry[id].u.constr;
	return 0;
}

sc_constraint *__sc_constraint_new(sc_constraint_info *info)
{
	sc_constraint *ptr;
	size_t size = sizeof(sc_constraint)+sizeof(sc_param)*(info->input_cnt-1);
	if (ca_free && size<=sizeof(const_alloc_buf)) {
		ca_free = false;
		ptr = (sc_constraint *)const_alloc_buf;
	} else {
		ptr = (sc_constraint *)malloc(size);
		if (!ptr)
			return 0;
	}
	ptr->id = info->id;
	return ptr;
}

sc_constraint *sc_constraint_alloc(sc_uint id)
{
	sc_constraint_info *info;
	if (!(info = sc_constraint_get_info(id)))
		return 0;
	return __sc_constraint_new(info);
}

sc_constraint *sc_constraint_new_valist(sc_uint id, va_list list)
{
	sc_constraint_info *info = sc_constraint_get_info(id);
	if (!info)
		return 0;

	sc_constraint *constr = __sc_constraint_new(info);
	if (!constr)
		return 0;

	for (int i = 0 ; i < info->input_cnt; ++i) {
		switch (info->input_spec[i]) {
		case SC_PD_TYPE:
			constr->params[i].type = va_arg(list, int);
			break;
		case SC_PD_ADDR:
		case SC_PD_ADDR_0:
			constr->params[i].addr = va_arg(list, sc_addr);
			break;
		case SC_PD_INT:
			constr->params[i].i = va_arg(list, int);
			break;
		case SC_PD_SEGMENT:
			constr->params[i].seg = va_arg(list, sc_segment *);
			break;
		case SC_PD_BOOLEAN:
			constr->params[i].flag = va_arg(list, bool);
			break;
		default:
			sc_constraint_free(constr);
			return 0;
		}
	}
	va_end(list);
	return constr;
}

sc_constraint *sc_constraint_new(sc_uint id,...)
{
	va_list list;
	va_start(list,id);
	return sc_constraint_new_valist(id,list);
}

sc_constraint *sc_constraint_new_by_name(char *name,...)
{
	va_list list;
	int id = find_holder_id_by_name(name);
	if (id<0)
		return 0;
	va_start(list,name);
	return sc_constraint_new_valist(id,list);
}

void	sc_constraint_free(sc_constraint *c)
{
	if (c==(sc_constraint *)const_alloc_buf)
		ca_free = true;
	else
		free(c);
}

sc_constraint *sc_constraint_clone(sc_constraint *c)
{
	int size = registry[c->id].u.constr->input_cnt-1+sizeof(sc_constraint);
	sc_constraint *ptr;
	ptr = (sc_constraint *)malloc(size);
	if (!ptr)
		return 0;
	memcpy(ptr,c,size);
	return ptr;
}

sc_constraint *sc_constraint_newv(sc_uint id,sc_param *regs,int cnt,int input[])
{
	sc_constraint_info *info = sc_constraint_get_info(id);
	sc_constraint *constr;
	if (!info)
		return 0;
	constr = __sc_constraint_new(info);
	if (!constr)
		return 0;
	if (info->input_cnt<cnt)
		cnt = info->input_cnt;
	while (--cnt>=0)
		constr->params[cnt] = regs[input[cnt]];
	return constr;
}

sc_constraint * sc_constraint_newa(sc_uint id,sc_param *regs,int cnt)
{
	sc_constraint_info *info = sc_constraint_get_info(id);
	sc_constraint *constr;
	if (!info)
		return 0;
	constr = __sc_constraint_new(info);
	if (!constr)
		return 0;
	if (info->input_cnt<cnt)
		cnt = info->input_cnt;
	memcpy(constr->params,regs,cnt*sizeof(sc_param));
	return constr;
}

int	sc_constraint_get_id_by_name(char *name)
{
	int id = find_holder_id_by_name(name);
	if (id<0)
		return -1;
	return registry[id].type == SCD_CONSTR ? id : -1;
}

int	sc_constraint_get_filter_id_by_name(char *name)
{
	int id = find_holder_id_by_name(name);
	if (id<0)
		return -1;
	return registry[id].type == SCD_FILTER ? id : -1;
}

int	sc_constraint_get_function_id_by_name(char *name)
{
	int id = find_holder_id_by_name(name);
	if (id<0)
		return -1;
	return registry[id].type == SCD_FUNC ? id : -1;
}

sc_filter	sc_constraint_get_filter(sc_uint id)
{
	if (registry[id].type != SCD_FILTER)
		return 0;
	return registry[id].u.filt->filter;
}

sc_filter_info *sc_constraint_get_filter_info(sc_uint id)
{
	if (registry[id].type != SCD_FILTER)
		return 0;
	return registry[id].u.filt;
}

sc_func		sc_constraint_get_function(sc_uint id)
{
	if (registry[id].type != SCD_FUNC)
		return 0;
	return registry[id].u.func->func;
}

sc_func_info	*sc_constraint_get_function_info(sc_uint id)
{
	if (registry[id].type != SCD_FUNC)
		return 0;
	return registry[id].u.func;
}

#ifdef CONSTRAINT_CHECK_PARAMS
static
int check_params(sc_constraint *constr,sc_constraint_info *info)
{
	int i;
	for (i=info->input_cnt-1;i>=0;i--) {
		if (info->input_spec[i]==SC_PD_TYPE) {
			if (!(constr->params[i].type & ~(SC_FUZZYNESS_MASK|SC_SYNTACTIC_MASK|SC_CONSTANCY_MASK)))
				continue;
		}
		if ((info->input_spec[i]==SC_PD_ADDR_0
					&& !constr->params[i].addr)
				|| info->input_spec[i]!=SC_PD_ADDR)
			continue;
		if ((unsigned long)(constr->params[i].addr)>SC_ARCH_PAGESIZE)
			continue;
		SC_ABORT_MESSAGE("Incorrect constraint params detected! Check backtrace");
	}
	return 1;
}
#endif

sc_iterator *sc_constraint_create_iterator(sc_session *s,sc_constraint *constr,bool sink)
{
	sc_constraint_info *info = sc_constraint_get_info(constr->id);
	if (!info)
		return 0;
#	ifdef CONSTRAINT_CHECK_PARAMS
	{
		if (!check_params(constr,info))
			return 0;
	}
#	endif
	{
		unsigned set_param=0;
		for (int i=0;i<info->input_cnt;i++) {
			if (info->input_spec[i] == SC_PD_ADDR && constr->params[i].addr->seg == SC_SET_SEG)
				set_param |= (1U<<i);
		}
		if (set_param) {
			sc_constraint_info *set_info = sc_constraint_find_set_constraint(info, set_param);
			if (!set_info) {
				set_info = sc_constraint_compile_set_constraint(info, set_param);
				if (__sc_constraint_register(set_info))
					SC_ABORT();
				sc_constraint_register_set_constraint(info, set_info, set_param);
			}
			return new sc_fallback_iterator(s,constr,set_info->deconstruct,sink);
		}
	}
	if (info->factory) {
		sc_iterator *iter = info->factory(s,constr);
		if (sink)
			sc_constraint_free(constr);
		return iter;
	}
	return new sc_fallback_iterator(s,constr,info->deconstruct,sink);
}

// reimplement this better later

#define MAX_REG_CNT 512
#define MAX_COMMANDS_LEN 4096

static
void __inline_dec(sc_deconstruct *top, sc_deconstruct *idec)
{
	int	regmap[MAX_REG_CNT];
	int	i,k;

	// par is parent deconstruct_item which called this
	sc_deconstruct_prg_item *par = top->commands + top->commands_len - 1;
	sc_deconstruct_prg_item *tmp;

	assert(par->type == SCD_CONSTR);

	// set regmap for input regs
	for (i = 0; i < par->in_cnt; i++)
		regmap[i] = par->params_in[i];

	// we initialize with -1 only part of regmap which is not input regs
	for (;i<idec->reg_cnt+idec->input_cnt;i++)
		regmap[i] = -1;

	// tmp is return of idec deconstruct
	tmp = idec->commands+idec->commands_len-1;
	assert(tmp->type == SCD_RETURN);

	// set regmap for output regs
	for (i=0;i<tmp->in_cnt && i<par->out_cnt;i++) {
		if (par->params_out[i]>=0)
			regmap[tmp->params_in[i]] = par->params_out[i];
	}

	// setup rest of regmap allocating new registers
	for (i=idec->input_cnt;i<idec->reg_cnt+idec->input_cnt;i++) {
		if (regmap[i]>=0)
			continue;
		regmap[i] = top->reg_cnt++ + top->input_cnt;
	}

	// new free parent
	top->commands_len--;
	delete [] par->params_in;
	delete [] par->params_out;

	for (i=0;i<idec->commands_len-1;i++) {
		sc_deconstruct_prg_item *item = idec->commands+i;
		tmp = top->commands+top->commands_len;
		tmp->in_cnt = item->in_cnt;
		tmp->out_cnt = item->out_cnt;
		tmp->name = item->name;
		tmp->id = item->id;
		tmp->type = item->type;
		tmp->params_in = new int[tmp->in_cnt];
		tmp->params_out = new int[tmp->out_cnt];

		for (k=0;k<tmp->in_cnt;k++)
			tmp->params_in[k] = regmap[item->params_in[k]];

		for (k=0;k<tmp->out_cnt;k++)
			if (item->params_out[k]>=0)
				tmp->params_out[k] = regmap[item->params_out[k]];
			else
				tmp->params_out[k] = -1;

		if (top->commands_len >= MAX_COMMANDS_LEN)
			SC_ABORT();

		top->commands[top->commands_len++] = *tmp;

		if (tmp->type == SCD_CONSTR
				&& strcmp(tmp->name,"all_input")
				&& strcmp(tmp->name,"all_output"))
			__inline_dec(top,registry[tmp->id].u.constr->deconstruct);
	}
}

sc_deconstruct *inline_deconstruct(sc_deconstruct *idec)
{
	if (!idec)
		return 0;
	sc_deconstruct *odec = new sc_deconstruct;
	// to not cause serious memory fragmentation
	sc_deconstruct_prg_item commands[MAX_COMMANDS_LEN];
	int i,k;
	odec->reg_cnt = idec->reg_cnt;
	odec->input_cnt = idec->input_cnt;
	odec->commands_len = 0;
	odec->commands = commands;
	for (i=0;i<idec->commands_len;i++) {
		sc_deconstruct_prg_item *tmp;
		sc_deconstruct_prg_item *item = idec->commands+i;
		tmp = odec->commands + odec->commands_len++;
		// little code duplication
		tmp->in_cnt = item->in_cnt;
		tmp->out_cnt = item->out_cnt;
		tmp->name = item->name;
		tmp->id = item->id;
		tmp->type = item->type;
		tmp->params_in = new int[tmp->in_cnt];
		tmp->params_out = new int[tmp->out_cnt];
		for (k=0;k<tmp->in_cnt;k++)
			tmp->params_in[k] = item->params_in[k];
		for (k=0;k<tmp->out_cnt;k++)
			if (item->params_out[k]>=0)
				tmp->params_out[k] = item->params_out[k];
			else
				tmp->params_out[k] = -1;
		if (tmp->type == SCD_CONSTR
				&& strcmp(tmp->name,"all_input")
				&& strcmp(tmp->name,"all_output"))
			__inline_dec(odec,registry[tmp->id].u.constr->deconstruct);
	}
	sc_deconstruct_prg_item *real_commands = new sc_deconstruct_prg_item[odec->commands_len];
	memcpy(real_commands,commands,sizeof(sc_deconstruct_prg_item)*odec->commands_len);
	odec->commands = real_commands;
	return odec;
}

// for debugging purposes
void	LIBSC_API dump_deconstruct(const sc_deconstruct *dec)
{
	int i,j;
	sc_deconstruct_prg_item *item;
	printf("-1. input [0..%d)\n",dec->input_cnt);
	printf("-0.5 regs (%d,%d)\n",dec->input_cnt,dec->input_cnt+dec->reg_cnt);
	for (i=0;i<dec->commands_len;i++) {
		item = dec->commands+i;
		printf("%d. ",i);
		switch (item->type) {
		case SCD_CONSTR:
			printf("constr"); break;
		case SCD_FUNC:
			printf("func");break;
		case SCD_FILTER:
			printf("filter");break;
		case SCD_SET_EACH:
			printf("set_each");break;
		default:
			printf("return");
		}
		printf(" %s(",item->name);
		if (item->in_cnt) {
			printf("%d",item->params_in[0]);
			for (j=1;j<item->in_cnt;j++)
				printf(",%d",item->params_in[j]);
		}
		if (item->out_cnt) {
			printf(":%d",item->params_out[0]);
			for (j=1;j<item->out_cnt;j++)
				printf(",%d",item->params_out[j]);
		}
		printf(")\n");
	}
}



sc_constraint_info *sc_constraint_compile_set_constraint(sc_constraint_info *info, unsigned set_params)
{
	if (!set_params)
		SC_ABORT_MESSAGE("this function cannot handle empty set_params bitset");
	//first check that each bit in set_params corresponds to SC_PD_ADDR param of original constraint
	int i,j;
	int p = set_params;
	int extra_regs_cnt;
	for (i=0;p && i < info->input_cnt; i++, p>>=1) {
		if (!(p & 1))
			continue;
		if (info->input_spec[i] != SC_PD_ADDR)
			SC_ABORT_MESSAGE("wrong set_params bitset!");
	}
	if (p)
		SC_ABORT_MESSAGE("somebody called sc_constraint_compile_set_constraint with wrong set_params");
	// allocate new constraint info and it's param spec
	sc_constraint_info *rv = new sc_constraint_info;
	rv->name = 0;
	rv->input_cnt = info->input_cnt;
	rv->input_spec = new sc_param_spec[info->input_cnt];
	rv->factory = 0;
	// fill input_spec of new constraint info
	memcpy(rv->input_spec, info->input_spec, sizeof(sc_param_spec)*info->input_cnt);
	extra_regs_cnt = 0;
	for (i=0, p = set_params; p; i++, p>>=1) {
		if (!(p&1))
			continue;
		rv->input_spec[i] = SC_PD_SET;
		extra_regs_cnt++;
	}
	// alloc new deconstruct
	sc_deconstruct *old_program = info->deconstruct;
	sc_deconstruct *program = rv->deconstruct = new sc_deconstruct;
	program->input_cnt = old_program->input_cnt;
	program->reg_cnt = old_program->reg_cnt+extra_regs_cnt;
	program->commands_len = old_program->commands_len+extra_regs_cnt;
	// register map will be used to map old input params to extra registers
	int *regmap = new int[program->input_cnt+program->reg_cnt];
	j = old_program->input_cnt+old_program->reg_cnt;
	for (i=0, p = set_params; i < program->input_cnt+program->reg_cnt; i++, p>>=1)
		if (p & 1)
			regmap[i] = j++;
		else
			regmap[i] = i;
	p = set_params;
	int current_command = 0;
	// alloc new program commands
	sc_deconstruct_prg_item *commands = program->commands = new sc_deconstruct_prg_item[program->commands_len];
	// fill new program commands
	for (i=0;i<old_program->commands_len;i++) {
		sc_deconstruct_prg_item *item = old_program->commands+i;
		sc_deconstruct_prg_item *new_item;
		// check command's input params
		for (j=0;j<item->in_cnt;j++) {
			int k = item->params_in[j];
			if (k < 0)
				continue;
			if (regmap[k] == k)
				continue;
			if (!(p & (1U<<k)))
				continue;
			// first usage of sc_set param was found
			p &= ~(1U<<k);
			// insert extra command
			new_item = commands+(current_command++);
			new_item->type = SCD_SET_EACH;
			new_item->in_cnt = 1;
			new_item->params_in = new int[1];
			new_item->params_in[0] = k;
			new_item->out_cnt = 1;
			new_item->params_out = new int[1];
			new_item->params_out[0] = regmap[k];
			new_item->name = 0;
			new_item->id = -1;
		}
		new_item = commands+(current_command++);
		// just copy item
		new_item->type = item->type;
		new_item->in_cnt = item->in_cnt;
		new_item->out_cnt = item->out_cnt;
		new_item->name = item->name;
		new_item->id = item->id;
		new_item->params_in = new int[item->in_cnt];
		for (j=0;j<item->in_cnt;j++) {
			int k = item->params_in[j];
			new_item->params_in[j] = k;
			if (k < 0)
				continue;
			new_item->params_in[j] = regmap[k];
		}
		new_item->params_out = new int[item->out_cnt];
		memcpy(new_item->params_out, item->params_out, sizeof(int)*item->out_cnt);
	}
	delete regmap;
	return rv;
}

struct set_constraint_key {
	int id;
	unsigned set_params;
	set_constraint_key(int _id, unsigned _set_params)
		: id(_id), set_params(_set_params)
	{
	}
	bool operator <(const set_constraint_key &other) const
	{
		if (id == other.id)
			return set_params < other.set_params;
		return id < other.id;
	}
};

typedef std::pair<set_constraint_key, int> set_constraint_pair;
typedef std::map<set_constraint_key, int> set_constraint_map;

static
set_constraint_map set_constraint_registry;


void sc_constraint_register_set_constraint(sc_constraint_info *original, sc_constraint_info *info, unsigned set_params)
{
	if (!set_constraint_registry.insert(std::make_pair(set_constraint_key(original->id, set_params), info->id)).second)
		SC_ABORT();
}

sc_constraint_info *sc_constraint_find_set_constraint(sc_constraint_info *orig_info, unsigned set_params)
{
	set_constraint_map::iterator iter = set_constraint_registry.find(set_constraint_key(orig_info->id, set_params));
	if (iter == set_constraint_registry.end())
		return 0;
	return sc_constraint_get_info((*iter).second);
}

