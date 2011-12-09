
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

DEFINE_FIXALLOCER(sc_fallback_iterator)


void sc_fallback_iterator::__next(int i)
{
	sc_deconstruct_prg_item *item;
	int k;
	if (i>0 && dec->commands[i].type != SCD_CONSTR)
		goto backward2;
	while (i>=0 && i<len-1) {
		item = dec->commands+i;
		if (item->type == SCD_SET_EACH) {
			sc_addr a_set;
			sc_plain_set &set_state = current[i].set_state;
			assert(item->in_cnt == 1 && item->out_cnt == 1);
			a_set = registers[item->params_in[0]].addr;
			assert(a_set && a_set->seg == SC_SET_SEG);
			if (!set_state)
				set_state = reinterpret_cast<sc_plain_set>(a_set->impl);
			else
				registers[item->params_out[0]].addr->unref();
			sc_addr addr = *set_state++;
			if (!addr) {
				set_state = 0;
				goto backward;
			}
			registers[item->params_out[0]].addr = addr->ref();
		} else if (item->type == SCD_CONSTR) {
			sc_iterator *iter;
			if (!current[i].iter) {
				sc_constraint *constr;
				constr = sc_constraint_newv(item->id,
					registers,item->in_cnt,
					item->params_in);
				if (!constr)
					SC_ABORT();
				iter = session->create_iterator(constr,true);
				current[i].iter = iter;
			} else
				(iter = current[i].iter)->next();
			if (iter->is_over()) {
				delete iter;
				current[i].iter = 0;
				goto backward;
			}
			for (k=0;k<item->out_cnt;k++) {
				if (item->params_out[k]<0)
					continue;
				registers[item->params_out[k]].addr=iter->value(k);
			}
		} else if (item->type == SCD_FILTER) {
			if (!current[i].filter(registers,item->in_cnt,item->params_in))
				goto backward;
		} else {
			registers[item->params_out[0]].addr = 0;
			if (current[i].func(registers,item->in_cnt,
					item->params_in,item->params_out[0])!=RV_OK)
				goto backward;
			sc_addr _t = registers[item->params_out[0]].addr;
			// have to make awfull hack
			if (((unsigned long)_t) >= SC_ARCH_PAGESIZE) {
				if (!session->is_segment_opened(_t->seg))
					goto backward;
				_t->ref();
			}
		}
		i++;
		continue;
	backward:
		i--;
	backward2:
		while (i>=0 && dec->commands[i].type != SCD_CONSTR && dec->commands[i].type != SCD_SET_EACH) {
			item = dec->commands+i;
			if (item->type == SCD_FUNC) {
				sc_addr _t = registers[item->params_out[0]].addr;
				// have to make awfull hack
				if (((unsigned long)_t) > SC_ARCH_PAGESIZE)
					_t->unref();
			}
			i--;
		}
	}
	if (i<0) {
		over = true;
		return;
	}
}

sc_fallback_iterator::sc_fallback_iterator
(
	sc_session *s,
	sc_constraint *_constr,
	const sc_deconstruct *_dec,
	bool _sink
)
{
	assert(sizeof(*this) == 32);
	int i;
	dec = _dec;
	session = s->ref();
	len = dec->commands_len;
	registers = new sc_param[dec->input_cnt+dec->reg_cnt];
	current = new iter_part[len];
	over = false;
	memcpy(registers,_constr->params,sizeof(sc_param)*dec->input_cnt);
	for (i=0;i<len;i++) {
		sc_deconstruct_prg_item *item = dec->commands+i;
		if (item->type == SCD_FILTER) {
			current[i].filter = sc_constraint_get_filter(item->id);
		} else if (item->type == SCD_FUNC) {
			current[i].func = sc_constraint_get_function(item->id);
		} else if (item->type != SCD_RETURN)
			current[i].iter = 0;
	}
	if (_sink)
		sc_constraint_free(_constr);
	__next(0);
}

sc_retval sc_fallback_iterator::next()
{
	int i = len-1;
	if (over)
		return RV_ELSE_GEN;
	__next(i);
	return over?RV_ELSE_GEN:RV_OK;
}

sc_fallback_iterator::~sc_fallback_iterator()
{
	for (int i=0;i<dec->commands_len;i++) {
		sc_deconstruct_prg_item *item = dec->commands+i;
		if (item->type == SCD_CONSTR)
			delete current[i].iter;
		else if (item->type == SCD_SET_EACH) {
			if (current[i].set_state)
				registers[item->params_out[0]].addr->unref();
		} else if (!over && item->type == SCD_FUNC) {
			sc_addr _t = registers[item->params_out[0]].addr;
			// have to make awfull hack
			if (((unsigned long)_t) > SC_ARCH_PAGESIZE)
				_t->unref();
		}
	}
	session->unref();
	delete [] current;
	delete [] registers;
}

sc_addr	sc_fallback_iterator::value(sc_uint num)
{
	sc_deconstruct_prg_item *ret = dec->commands+len-1;
	if (over)
		return 0;
	if (num>=(sc_uint)ret->in_cnt) {
		return 0;
	}
	return registers[ret->params_in[num]].addr;
}
