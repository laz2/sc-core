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

void scp_opnd_info::form_name() const
{
	std::stringstream out;

	if (set) {
		out << system_session->get_idtf(pm_num_set_attrs[index]);
	} else {
		out << system_session->get_idtf(pm_numattr[index]);
	}
	
	if (opnd) {
		out << ": ";

		if (fixed)
			out << system_session->get_idtf(FIXED_) << ": ";

		if (assign)
			out << system_session->get_idtf(ASSIGN_) << ": ";

		if (var)
			out << "variable ";

		out << "\"";

		sc_string idtf = system_session->get_idtf(opnd);
		if (is_system_id(idtf)) {
			out << "<SYSTEM-IDTF>";
		} else {
			out << idtf;
		}
		out << "\"";
	}

	std::string result = out.str();
	name = new char[result.size() + 1];
	std::copy(result.begin(), result.end(), name);
	name[result.size()] = 0;
}

void scp_opnd_info::unpack_arc_flags()
{
	static sc_addr opnd_arc_flags[] = { 
		CONST_, VAR_, METAVAR_, POS_, FUZ_, NEG_, ARC_, NODE_, UNDF_,
		PERMANENT_, TEMPORARY_, ASSIGN_, FIXED_
	};
	static const int opnd_arc_flags_count = sizeof(opnd_arc_flags) / sizeof(sc_addr);

	bool flags[opnd_arc_flags_count];
	sc_set::unpack(system_session, opnd_arc, opnd_arc_flags, flags, opnd_arc_flags_count);

	// Unpack type mask
	#define OR_IF(expr,val,mask) ((expr)?((val)|(mask)):(val))
	type_mask = OR_IF(flags[0],  type_mask, SC_CONST);
	type_mask = OR_IF(flags[1],  type_mask, SC_VAR);
	type_mask = OR_IF(flags[2],  type_mask, SC_METAVAR);
	type_mask = OR_IF(flags[3],  type_mask, SC_POS);
	type_mask = OR_IF(flags[4],  type_mask, SC_FUZ);
	type_mask = OR_IF(flags[5],  type_mask, SC_NEG);
	type_mask = OR_IF(flags[6],  type_mask, SC_ARC_MAIN);
	type_mask = OR_IF(flags[7],  type_mask, SC_NODE);
	type_mask = OR_IF(flags[8],  type_mask, SC_UNDF);
	type_mask = OR_IF(flags[9],  type_mask, SC_PERMANENT);
	type_mask = OR_IF(flags[10], type_mask, SC_TEMPORARY);
	#undef OR_IF

	// Unpack assign_/fixed_
	assign = flags[11];
	fixed  = flags[12];
}

void scp_opnd_info::retrieve()
{
	if (assign) {
		// TODO: think about clearing variables
		if (var)
			scp_clear_variable(pi, opnd);
	} else {
		if (var) {
			fixed = get_var_value(pi, opnd, 0, &value) == RV_OK;
		} else {
			value = opnd;
			fixed = 1;
		}
	}
}

bool scp_opnd_info::unpack()
{
	if (opnd_arc) {
		var = is_variable(pi, opnd);
		unpack_arc_flags();
		retrieve();
		return true;
	} else {
		return false;
	}
}

bool scp_opnd_info::search_ord(scp_process_info *pi_, int index_)
{
	pi = pi_;
	index = index_;
	opnd = sc_tup::at(system_session, pi->active_op, pm_numattr[index_], &opnd_arc);
	return unpack();
}

bool scp_opnd_info::search_set(scp_process_info *pi_, int index_)
{
	pi = pi_;
	index = index_;
	set = true;
	opnd = sc_tup::at(system_session, pi->active_op, pm_num_set_attrs[index_], &opnd_arc);
	return unpack();
}

bool scp_opnd_info::search_ord(scp_process_info *pi_, int index_, sc_addr opnd_arc_)
{
	pi = pi_;
	index = index_;
	opnd_arc = opnd_arc_;
	opnd = system_session->get_end(opnd_arc);
	return unpack();
}

bool scp_opnd_info::search_set(scp_process_info *pi_, int index_, sc_addr opnd_arc_)
{
	pi = pi_;
	index = index_;
	opnd_arc = opnd_arc_;
	set = true;
	opnd = system_session->get_end(opnd_arc);
	return unpack();
}

sc_retval scp_opnd_info::create_element()
{
	sc_segment *seg;
	if (get_single_segment(pi, index, &seg) == RV_OK) {
		value = pi->session->create_el(seg, type_mask);

		if (value) {
			scp_set_variable(pi, opnd, value);
			return RV_OK;
		} else {
			SCP_ERROR_F(pi->process, "Cannot create element with type %s in segment \"%s\" for operand %s", \
				get_type_info(type_mask).c_str(), seg->get_full_uri().c_str(), get_name()); \
		}
	}

	return RV_ERR_GEN;
}

sc_retval get_single_segc(scp_process_info *pi, int index, sc_addr *segc)
{
	sc_addr constr;
	if (search_oneshot(system_session,sc_constraint_new(CONSTR_5_f_a_a_a_f,
				pi->active_op,
				0,
				0,
				0,
				pm_num_segc_attrs[index]
			), true, 1, 2, &constr))
		return RV_ELSE_GEN;
	if (get_var_value(pi, constr, 0, segc))
		return RV_ERR_GEN;
	return RV_OK;
}

sc_retval get_single_segment(scp_process_info *pi, int index, sc_segment **segment) 
{
	sc_addr seg_sign;
	if (get_single_segc(pi, index, &seg_sign)) {
		seg_sign = pi->defaultseg_sign;
	}

	*segment = is_segment(pi->session, seg_sign);
	if (!*segment) {
		SCP_ERROR(pi->process, "cannot retrieve segment for element creating of operand " << index << "_");
		return RV_ERR_GEN;
	}

	return RV_OK;
}

class multi_segc_filter : public sc_single_filter 
{
public:
	typedef std::list<sc_segment *> segc_set_t;

private:
	segc_set_t segc_set;

public:
	multi_segc_filter() 
	{
	}

	void add_segment(sc_segment *s)
	{
		segc_set.push_back(s);
	}

	bool check_value(sc_addr val)
	{
		return std::find(segc_set.begin(), segc_set.end(), val->seg) != segc_set.end();
	}

	~multi_segc_filter() {}
};

sc_retval get_multi_segc_filter(scp_process_info *pi, int index, sc_single_filter **filter)
{
	sc_addr segc_constr;

	sc_retval rv = search_oneshot(pi->session, sc_constraint_new(
				CONSTR_5_f_a_a_a_f,
				pi->active_op,
				SC_A_CONST|SC_POS,
				SC_N_CONST,
				SC_A_CONST|SC_POS,
				pm_num_segc_attrs[index]
			),true, 1, 2, &segc_constr);
	
	multi_segc_filter *segc_filter = new multi_segc_filter();

	if (rv) {
		return RV_ELSE_GEN;
	}

	std::auto_ptr<sc_iterator> iter(
		pi->session->create_iterator(sc_constraint_new(
			CONSTR_3_f_a_a, 
			segc_constr,
			SC_A_CONST|SC_POS, 
			SC_N_CONST), true));
	
	for (; !iter->is_over(); iter->next()) {
		sc_addr	segvar = iter->value(3);
	
		sc_addr direntry;
		if (get_var_value(pi, segvar, 0, &direntry)) {
			continue;
		}

		sc_segment *segment = sign2seg(pi->session, direntry);

		if (!segment) {
			continue;
		}

		segc_filter->add_segment(segment);
	}

	return RV_OK;
}



//
//  For backward compatability
//

bool cache_scp_operands = false;

typedef std::map<sc_addr, scp_opnd_info> opnd2info_cache_map_t;
typedef std::map<sc_string, opnd2info_cache_map_t> op2opnd_cache_map_t;

op2opnd_cache_map_t op_cache; /// Operands cache map.

sc_type get_type_mask(sc_addr opnd_arc)
{
	static sc_addr opnd_arc_flags[] = { 
		CONST_, VAR_, METAVAR_, POS_, FUZ_, NEG_, ARC_, NODE_,
		UNDF_, PERMANENT_, TEMPORARY_
	};
	static const int opnd_arc_flags_count = sizeof(opnd_arc_flags) / sizeof(sc_addr);

	sc_type type = 0;
	bool flags[opnd_arc_flags_count];
	sc_set::unpack(system_session, opnd_arc, opnd_arc_flags, flags, opnd_arc_flags_count);

#define OR_IF(expr,val,mask) ((expr)?((val)|(mask)):(val))
	type = OR_IF(flags[0], type, SC_CONST);
	type = OR_IF(flags[1], type, SC_VAR);
	type = OR_IF(flags[2], type, SC_METAVAR);
	type = OR_IF(flags[3], type, SC_POS);
	type = OR_IF(flags[4], type, SC_FUZ);
	type = OR_IF(flags[5], type, SC_NEG);
	type = OR_IF(flags[6], type, SC_ARC_MAIN);
	type = OR_IF(flags[7], type, SC_NODE);
	type = OR_IF(flags[8], type, SC_UNDF);
	type = OR_IF(flags[9], type, SC_PERMANENT);
	type = OR_IF(flags[10], type, SC_TEMPORARY);

	return type;
#undef OR_IF
}

inline bool search_opnd(scp_process_info *pi, int index, scp_opnd_info *info)
{
	if (!search_5_f_cpa_a_cpa_f(system_session, pi->active_op, &info->opnd_arc, &info->opnd, 0, pm_numattr[index])) {
		info->index = index;
		info->type_mask = get_type_mask(info->opnd_arc);
		info->var = 0;
		info->assign = 0;
		info->fixed = 1;
		info->set = 0;
		return true;
	} else {
		return false;
	}
}

inline bool search_set_opnd(scp_process_info *pi, int index, scp_opnd_info *info)
{
	if (!search_5_f_cpa_a_cpa_f(system_session, pi->active_op, &info->opnd_arc, &info->opnd, 0, pm_num_set_attrs[index])) {
		info->index = index;
		info->type_mask = get_type_mask(info->opnd_arc);
		info->var = 0;
		info->assign = 0;
		info->fixed = 1;
		info->set = 0;
		return true;
	} else {
		return false;
	}
}

inline bool search_set_attr(scp_opnd_info *info)
{
	// Поиск атрибутивного отношения set_ у дуги, соединяющей оператор с операндом 
	if (!search_3_f_cpa_f(system_session, SET_, 0, info->opnd_arc)) {
		info->set = 1;
		return true;
	} else {
		return false;
	}
}

inline bool search_assign_attr(scp_opnd_info *info)
{
	// Проверка наличия дуги, соединяющей оператор с операндом,
	// атрибутивного отношения assign_
	if (!search_3_f_cpa_f(system_session, ASSIGN_, 0, info->opnd_arc)) {
		info->value = 0;
		info->assign = 1;
		return true;
	} else {
		return false;
	}
}

inline bool search_fixed_attr(scp_opnd_info *info)
{
	if (!search_3_f_cpa_f(system_session, FIXED_, 0, info->opnd_arc)) {
		info->fixed = 1;
		info->assign = 0;
		return true;
	} else {
		return false;
	}
}

inline bool search_var(scp_process_info *pi, scp_opnd_info *info)
{
	// Проверка, является ли операнд переменной
	if (is_variable(pi, info->opnd)) {
		info->var = 1;
		info->fixed = 0;
		return true;
	} else {
		return false;
	}
}

inline bool is_const(scp_process_info *pi, scp_opnd_info *info)
{
	return search_3_f_cpa_f(system_session, pi->consts, 0, info->opnd) == RV_OK;
}

/**
 * @brief Returns cached operands map for operator @p active_op. 
 */
opnd2info_cache_map_t& get_cached_op(sc_addr active_op)
{
	sc_string full_idtf = FULL_IDTF(active_op);
	op2opnd_cache_map_t::iterator cached_op_it = op_cache.find(full_idtf);
	if (cached_op_it != op_cache.end()) {
		return cached_op_it->second;
	} else {
		op_cache[full_idtf] = opnd2info_cache_map_t();
		return op_cache[full_idtf];
	}
}

sc_retval scp_fill_opndinfo(scp_process_info *pi, int index, scp_opnd_info *opnd_info, unsigned flags)
{
	memset(opnd_info, 0, sizeof(scp_opnd_info));
	
	// Find operand in cache
	bool from_cache = false;
	if (cache_scp_operands) {
		opnd2info_cache_map_t& cached_op = get_cached_op(pi->active_op);
		opnd2info_cache_map_t::iterator cached_opnd_it = cached_op.find(pm_numattr[index]);
		if (cached_opnd_it != cached_op.end()) {
			*opnd_info = cached_opnd_it->second;
			opnd_info->opnd = 0;
			opnd_info->opnd_arc = 0;
			opnd_info->value = 0;
			search_5_f_cpa_a_cpa_f(system_session, pi->active_op, &opnd_info->opnd_arc, 
				&opnd_info->opnd, 0, pm_numattr[index]);
			from_cache = true;
		}
	}

	if (!from_cache) {
		if (!search_opnd(pi, index, opnd_info))
			return RV_ELSE_GEN;

		search_set_attr(opnd_info);
		search_var(pi, opnd_info);
	}

	if ((from_cache && opnd_info->assign) || search_assign_attr(opnd_info)) {
		if (opnd_info->var) {
			scp_clear_variable(pi, opnd_info->opnd);
		} else {
			SCP_ERROR_F(pi->process, "Operand %d_ \"%s\" has assign_ attribute, but it isn't variable",
				index, SC_IDc(opnd_info->opnd));
			return RV_ERR_GEN;
		}
	} else {
		if (opnd_info->var) {
			if (get_var_value(pi, opnd_info->opnd, 0, &opnd_info->value) == RV_OK) {
				opnd_info->fixed = 1;
			}
		} else {
			opnd_info->value = opnd_info->opnd;
			opnd_info->fixed = 1;
		}
	}

	if (cache_scp_operands && !from_cache) {
		opnd2info_cache_map_t& cached_op = get_cached_op(pi->active_op);
		cached_op[pm_numattr[index]] = *opnd_info;
	}

	return RV_OK;
}

sc_retval scp_fill_set_opndinfo(scp_process_info *pi, int index, scp_opnd_info *opnd_info)
{
	memset(opnd_info, 0, sizeof(scp_opnd_info));

	// Find operand in cache
	bool from_cache = false;
	if (cache_scp_operands) {
		opnd2info_cache_map_t& cached_op = get_cached_op(pi->active_op);
		opnd2info_cache_map_t::iterator cached_opnd_it = cached_op.find(pm_num_set_attrs[index]);
		if (cached_opnd_it != cached_op.end()) {
			*opnd_info = cached_opnd_it->second;
			opnd_info->opnd = 0;
			opnd_info->opnd_arc = 0;
			opnd_info->value = 0;
			search_5_f_cpa_a_cpa_f(system_session, pi->active_op, &opnd_info->opnd_arc, 
				&opnd_info->opnd, 0, pm_num_set_attrs[index]);
			from_cache = true;
		}
	}

	if (!from_cache) {
		if (!search_set_opnd(pi, index, opnd_info)) {
			return RV_ELSE_GEN;
		}
	}

	if (opnd_info->var || search_var(pi, opnd_info)) {
		if (opnd_info->assign || search_assign_attr(opnd_info)) {
			scp_clear_variable(pi, opnd_info->opnd);
		} else if (opnd_info->fixed || search_fixed_attr(opnd_info)) {
			if (get_var_value(pi, opnd_info->opnd, 0, &opnd_info->value)) {
				dprintf("fixed_ operand haven't value");
				return RV_ERR_GEN;
			}
		} else {
			SCP_ERROR(pi->process, "set" << index << "_ operand must be fixed_ or assign_");
			return RV_ERR_GEN;
		}
	} else {
		opnd_info->value = opnd_info->opnd;
		opnd_info->fixed = 1;
	}

	if (cache_scp_operands && !from_cache) {
		opnd2info_cache_map_t& cached_op = get_cached_op(pi->active_op);
		cached_op[pm_num_set_attrs[index]] = *opnd_info;
	}

	return RV_OK;
}
