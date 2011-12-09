
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
 * @file pattern_ops.cpp
 * @brief Implementation SCP-operators for working with pattern search and generating.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

/**
 * @brief Merge element @p main with @p other. Delete @p other.
 */
sc_retval simple_merge_elements(sc_session* s, sc_addr main, sc_addr other)
{
	sc_iterator *it = system_session->create_iterator(sc_constraint_new(CONSTR_ALL_INPUT, other), true);
	sc_for_each(it) system_session->set_end(it->value(1), main);

	it = system_session->create_iterator(sc_constraint_new(CONSTR_ALL_OUTPUT, other), true);
	sc_for_each(it) system_session->set_beg(it->value(1), main);

	if (s->get_type(other) == s->get_type(main) && s->get_type(other) & SC_ARC) {
		if (!s->get_beg(main))
			s->set_beg(main, s->get_beg(other));
		if (!s->get_end(main))
			s->set_end(main, s->get_end(other));
	}

	s->erase_el(other);

	return RV_OK;
}


class pattern
{
public:
	sc_smart_addr pattern_sign;
	sc_smart_addr results;				// фиксированное множество пар соответствия возращаемых значений
	sc_smart_addr all_results;			// возвращаемое множество всех найденных/сгенерированных элементов
	sc_smart_addr setPairResult;			// возвращаемое множество пар соответствия
	sc_smart_addr setFilterResult;			// множество элементов для которого создаем пары соответствия
	std::set<sc_addr> var_els;			// множество переменных шаблона
	std::set<sc_addr> const_els;		// множество констант шаблона
	std::set<sc_addr> vars_with_set;
	std::list<sc_addr> result_els;
	std::set<sc_addr> fixed_els;
	addr2addr_map mapParam;
	addr2addr_map mapResult;
	sc_segment* segment;
	sc_session* session;

	addr2addr_map original2copy;
	addr2addr_map copy2original;

	pattern(sc_session* s)
	{
		session = s;
		pattern_sign		= 0;
		results	= 0;
		segment		= 0;
		all_results = 0;
		setPairResult = 0;
		setFilterResult = 0;
	}

	~pattern()
	{
		if (segment)
			session->unlink(segment->get_full_uri());
		//TODO: надо чистить мусор, а что будет мусором?
		//TODO: где создаются пары результата?
		//session->erase_el(setResult);
		//if (segment) {
		//	sc_string _id = segment->get_full_uri();
		//	session->close_segment(segment);
		//	session->unlink(_id);
		//} else {
		//	std::list<sc_addr>::iterator it = resultElmn.begin();
		//	for (; it != resultElmn.end(); ++it) {
		//		sc_addr elmn = *it;
		//		if (elmn) session->erase_el(elmn);
		//	}
		//}
	}
};


/**
 * @brief Copy context for #cb_copy_pattern.
 */
struct context_copy_pattern
{
	sc_addr new_pattern;
	addr2addr_map original2copy;
	addr2addr_map copy2original;
};

/**
 * @brief Callback for copying patterns vars/metavars.
 * @see #sc_set.copy
 */
void cb_copy_pattern(sc_addr original, sc_addr copy, void *cb_data)
{
	context_copy_pattern *context = (context_copy_pattern *) cb_data;
	system_session->gen3_f_a_f(context->new_pattern, 0, copy->seg, SC_A_CONST, copy);
	context->original2copy.insert(addr2addr_map::value_type(original, copy));
	context->copy2original.insert(addr2addr_map::value_type(copy, original));
}


/**
 * @brief Parse params or results pairs set for sys_search or sys_gen.
 * @note Do not modify sc-memory.
 * @param info scp-process info
 * @param pairs_set raw pairs set.
 * @param[out] pairs_map .
 * @param[out] vars_with_set second pair elements with attr set_.
 * @param[out] fixed_els second pair elements with attr fixed_.
 * @param is_param_parse if true, then parse params pairs set, else results pairs set.
 * @return sc-return value.
 */
sc_retval parse_pair(scp_process_info *info, sc_addr pairs_set, addr2addr_map &pairs_map, std::set<sc_addr> &vars_with_set,
					 std::set<sc_addr> &fixed_els, bool is_param_parse = false)
{
	sc_session *s = info->session;

	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, pairs_set, 0, SC_NODE), true);
	sc_for_each(it) {
		sc_addr pair = it->value(2);

		sc_addr first_el = sc_tup::at(s, pair, N1_);
		if (!first_el) {
			if (is_param_parse) {
				SCP_ERROR(info->process, "Not found 1_ component for entry '" << FULL_IDTF(pair) << "' in parameters pairs set");
			} else {
				SCP_ERROR(info->process, "Not found 1_ component for entry '" << FULL_IDTF(pair) << "' in results pairs set");
			}
		}

		// If first pair element is scp-variable, then set it value as first element.
		if (is_variable(info, first_el))
			first_el = scp_get_var_value(info, first_el);

		sc_addr arc_2_ = 0;
		sc_addr second_el = sc_tup::at(s, pair, N2_, &arc_2_);
		if (!second_el) {
			if (is_param_parse) {
				SCP_ERROR(info->process, "Not found 2_ component for 1_:'" << FULL_IDTF(first_el) << "' in parameters pairs set");
			} else {
				SCP_ERROR(info->process, "Not found 2_ component for 1_:'" << FULL_IDTF(first_el) << "' in results pairs set");
			}
		}

		if (is_param_parse) {
			if (is_variable(info, second_el))
				second_el = scp_get_var_value(info, second_el);
		} else {
			if (!is_variable(info, second_el))
				SCP_ERROR(info->process, "2_ component for 1_:'" << FULL_IDTF(first_el) << "' in results pairs set must program var");

			if (search_3_f_cpa_f(s, SET_, 0, arc_2_) == RV_OK)
				vars_with_set.insert(second_el);

			if (search_3_f_cpa_f(s, FIXED_, 0, arc_2_) == RV_OK)
				fixed_els.insert(second_el);
		}

		pairs_map.insert(addr2addr_map::value_type(first_el, second_el));
	}

	return RV_OK;
}

#include <iostream>

/**
 * @brief Parse pattern, copy it and substitute params.
 */
sc_retval prepare_pattern(scp_process_info* info, pattern& pattern_context, sc_addr param_pairs_set, sc_addr result_pairs_set)
{
	// TODO: use transaction
	sc_session* s = info->session;

	LOCK_EVENT(); // Block event handlers during pattern coping.

	// Collect vars/metavars and consts pattern elements in two lists.
	sc_iterator* it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, pattern_context.pattern_sign.get(), 0, 0), true);
	sc_for_each(it) {
		sc_addr pattern_el = it->value(2);

		if (s->get_type(pattern_el) & (SC_VAR|SC_METAVAR)) {
			pattern_context.var_els.insert(pattern_el);
		} else {
			pattern_context.const_els.insert(pattern_el);
		}
	}

	if (pattern_context.var_els.empty())
		SCP_ERROR(info->process, "Pattern must have at least one var element");

	if (result_pairs_set)
		if (parse_pair(info, result_pairs_set, pattern_context.mapResult,
				pattern_context.vars_with_set, pattern_context.fixed_els) != RV_OK)
			return RV_ERR_GEN;

	if (param_pairs_set) {
		if (parse_pair(info, param_pairs_set, pattern_context.mapParam,
				pattern_context.vars_with_set, pattern_context.fixed_els, true) != RV_OK)
			return RV_ERR_GEN;

		//
		// Copy pattern in tmp segment. Do not copy constants.
		//
		pattern_context.segment = create_unique_segment(s, "/tmp");
		context_copy_pattern copy_context;
		copy_context.new_pattern = s->create_el(is_segment(s, info->defaultseg_sign), SC_N_CONST);
		sc_set::copy(s, pattern_context.segment, pattern_context.var_els, cb_copy_pattern, &copy_context);
		std::set<sc_addr>::iterator it_consts = pattern_context.const_els.begin();
		for (; it_consts != pattern_context.const_els.end(); it_consts++)
			s->gen3_f_a_f(copy_context.new_pattern, 0, pattern_context.segment, SC_A_CONST, *it_consts);

		//
		// Substitute params in pattern copy.
		//
		addr2addr_map::iterator params_it = pattern_context.mapParam.begin();
		for (; params_it != pattern_context.mapParam.end(); ++params_it) {
			sc_addr param = params_it->first;
			sc_addr val = params_it->second;

			addr2addr_map::iterator copies_it = copy_context.original2copy.find(param);

			if (copies_it == copy_context.original2copy.end()) {
				SCP_ERROR(info->process, "Pattern hasn't parameter " << SC_URI(param));
				return RV_ERR_GEN;
			}

			sc_addr param_copy = copies_it->second;
			simple_merge_elements(s, val, param_copy);
			copy_context.original2copy.erase(copies_it);
			copy_context.original2copy.insert(addr2addr_map::value_type(param, val));
		}

		//
		// Substitute pattern sign and result pairs.
		//
		pattern_context.pattern_sign = copy_context.new_pattern;
		addr2addr_map::iterator results_it = pattern_context.mapResult.begin();
		while (results_it != pattern_context.mapResult.end()) {
			sc_addr result_param = results_it->first;

			addr2addr_map::iterator itmp2 = copy_context.original2copy.find(result_param);
			if (itmp2 == copy_context.original2copy.end()) {

				//
				// May be result_param is already substituted param, then skip its substituting.
				// result_param must be in temporary pattern segment. Check it.
				//

				if (pattern_context.segment != result_param->seg) {
					SCP_ERROR(info->process, "Pattern hasn't result parameter " << get_addr_full_uri(result_param));
					return RV_ERR_GEN;
				}

				++results_it;
				continue;
			}

			sc_addr result_out = results_it->second;
			pattern_context.mapResult.erase(results_it);
			pattern_context.mapResult.insert(addr2addr_map::value_type(itmp2->second, result_out));
			results_it = pattern_context.mapResult.begin();
		}

		pattern_context.original2copy = copy_context.original2copy;
		pattern_context.copy2original = copy_context.copy2original;
	}

	return RV_OK;
}


sc_retval pack_result_pattern(scp_process_info *info, pattern &pattern_context)
{
	sc_session *s = system_session;
	sc_segment *seg = is_segment(s, info->defaultseg_sign);
	addr2addr_map::iterator itmp;

	//
	// Collect all NSM results in result_els.
	//
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, pattern_context.results.get(), 0, SC_NODE), true);
	sc_for_each(it) pattern_context.result_els.push_back(it->value(2));

	if (pattern_context.result_els.empty())
		return RV_ERR_GEN;

	for (itmp = pattern_context.mapResult.begin(); itmp != pattern_context.mapResult.end(); ++itmp) {
		sc_addr pattern_el = itmp->first; // element in pattern copy
		sc_addr scp_var = itmp->second;   // scp-variable for outputing result

		// Check if result scp-variable has set_ attr.
		bool is_set = false;
		if (pattern_context.vars_with_set.find(scp_var) != pattern_context.vars_with_set.end())
			is_set = true;

		sc_addr setValues = 0;
		std::list<sc_addr>::iterator itRes = pattern_context.result_els.begin();
		for (; itRes != pattern_context.result_els.end(); itRes++) {
			sc_addr result_el = *itRes;

			// Search result pair (in pattern copy) for pattern_el.
			sc_addr result_pair = 0;
			if (search_3l2_f_cpa_cna_cpa_f(s, result_el, 0, &result_pair, 0, pattern_el))
				continue;

			sc_addr found_el = 0;
			sc_iterator *it2 = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, result_pair, 0, 0), true);
			sc_for_each(it2) {
				sc_addr el = it2->value(2);
				if (el == pattern_el) {
					continue;
				} else {
					found_el = el;
					break;
				}
			}

			if (found_el == 0)
				break;

			if (is_set) {
				sc_addr arc1;
				if (setValues == 0) {
					if (pattern_context.fixed_els.find(scp_var) != pattern_context.fixed_els.end()) {
						setValues = scp_get_var_value(info, scp_var);
					} else {
						setValues = s->create_el(seg, SC_N_CONST);
					}
				}
				s->gen3_f_a_f(setValues, &arc1, seg, SC_A_CONST, found_el);
			} else {
				scp_set_variable(info, scp_var, found_el);
				break;
			}
		}

		if (is_set && setValues != 0) {
			scp_set_variable(info, scp_var, setValues);
			setValues = 0;
		}
	}

	//
	// Add all results elements in all_results.
	//
	if (pattern_context.all_results) {
		std::list<sc_addr>::iterator result_it = pattern_context.result_els.begin();
		for (; result_it != pattern_context.result_els.end(); ++result_it) {
			sc_addr result_pair = *result_it;

			sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, result_pair, 0, 0), true);
			sc_for_each(it) {
				sc_addr found_el = sc_tup::at(s, it->value(2), N2_);
				if (found_el)
					s->gen3_f_a_f(pattern_context.all_results, 0, seg, SC_A_CONST, found_el);
			}
		}
	}

	if (pattern_context.setPairResult) {
		// Add filter elements to filtred_els
		std::set<sc_addr> filter_els;
		if (pattern_context.setFilterResult) {
		  sc_iterator* it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, pattern_context.setFilterResult.get(), 0, 0), true);
			sc_for_each(it) filter_els.insert(it->value(2));
		}

		std::list<sc_addr>::iterator itRes = pattern_context.result_els.begin();
		for (; itRes != pattern_context.result_els.end(); ++itRes) {
			sc_addr result_el = *itRes;

			sc_iterator* it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, result_el, 0, 0), true);
			sc_for_each(it) {
				sc_addr comp_arc = 0;
				sc_addr _pe = sc_tup::at(s, it->value(2), N1_, &comp_arc);

				if (pattern_context.setFilterResult) {
					if (filter_els.find(_pe) == filter_els.end()) {
						s->erase_el(it->value(2));
						continue;
					}
				}


				addr2addr_map::iterator c2o_it = pattern_context.copy2original.find(_pe);
				if (c2o_it != pattern_context.copy2original.end()) {
					s->set_end(comp_arc, c2o_it->second);
					s->erase_el(_pe);
				}
			}

			s->gen3_f_a_f(pattern_context.setPairResult, 0, seg, SC_A_CONST, result_el);
		}
	}

	return RV_OK;
}

/**
 * @brief SCP-operator for pattern searching.
 * @code
 * sys_search -> {
 *      1_: fixed_: <pattern>,
 *      [ 2_: (fixed_|assign_): { {1_: <pattern_el>, 2_: <scp-variable> }, ... } ], // results pairs set
 *      [ 3_: fixed_: { {1_: <pattern_el>, 2_: <scp-variable> }, ... } ], // params pairs set
 *      [ 4_: (assign_|fixed_): <scp-variable> ],                         // all results set
 *      [ 5_: fixed_: <filter_set> ]                                      // filter set
 * };
 * @endcode
 */
SCP_OPERATOR_IMPL(scp_oper_sys_search)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd_pattern);
	SCP_OPND_OPT_ASSIGN_FIXED_CREATE(info, 2, opnd_results);
	SCP_OPND_OPT_FIXED(info, 3, opnd_params);
	SCP_OPND_OPT_ASSIGN_FIXED_CREATE(info, 4, opnd_all_results);
	SCP_OPND_OPT_FIXED(info, 5, opnd_filter);

	sc_session *s = info->session;

	sc_addr pattern_sign = opnd_pattern.value;
	sc_addr param_pairs = opnd_params.value;
	sc_addr all_results = opnd_all_results.value;
	sc_addr filter = opnd_filter.value;

	sc_addr pair_set = 0;
	sc_addr result_pair_set = 0;
	if (opnd_results.opnd) {
		if (opnd_results.fixed) {
			pair_set = opnd_results.value;
		} else {
			result_pair_set = opnd_results.value;
		}
	}

	pattern pattern_context(s);
	pattern_context.pattern_sign = pattern_sign;
	pattern_context.all_results = all_results;
	pattern_context.setPairResult = result_pair_set;
	pattern_context.setFilterResult = filter;

	if (prepare_pattern(info, pattern_context, param_pairs, pair_set) != RV_OK) {
		return RV_ERR_GEN;
	}

	nsm_pattern cur_nsm_pattern(s, pattern_context.pattern_sign, is_segment(s, info->defaultseg_sign));
	if (cur_nsm_pattern.search()) {
		pattern_context.results = cur_nsm_pattern.get_results();
		pack_result_pattern(info, pattern_context);
		cur_nsm_pattern.set_need_clean(!pattern_context.setPairResult);
	}

	if (pattern_context.result_els.empty()) {
		scp_set_state(info->process, SCP_STATE_ELSE);
	} else {
		scp_set_state(info->process, SCP_STATE_THEN);
	}

	return RV_OK;
}

/**
 * @brief SCP-operator for generating sc-constructs from pattern.
 * @code
 * sys_gen -> {
 *      1_: fixed_: <pattern>,
 *      [ 2_: (fixed_|assign_): { {1_: <pattern_el>, 2_: <scp-variable> }, ... } ], // results pairs set
 *      [ 3_: fixed_: { {1_: <pattern_el>, 2_: <scp-variable> }, ... } ], // params pairs set
 *      [ 4_: (assign_|fixed_): <scp-variable> ]                         // all results set
 * };
 * @endcode
 */
SCP_OPERATOR_IMPL(scp_oper_sys_gen)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd_pattern);
	SCP_OPND_OPT_ASSIGN_FIXED_CREATE(info, 2, opnd_results);
	SCP_OPND_OPT_FIXED(info, 3, opnd_params);
	SCP_OPND_OPT_ASSIGN_FIXED_CREATE(info, 4, opnd_all_results);
	SCP_OPND_OPT_FIXED(info, 5, opnd_filter);

	sc_session *s = info->session;

	sc_addr pattern_sign = opnd_pattern.value;
	sc_addr param_pairs = opnd_params.value;
	sc_addr all_results = opnd_all_results.value;
	sc_addr filter = opnd_filter.value;

	sc_addr pair_set = 0;
	sc_addr result_pair_set = 0;
	if (opnd_results.opnd) {
		if (opnd_results.fixed) {
			pair_set = opnd_results.value;
		} else {
			result_pair_set = opnd_results.value;
		}
	}

	pattern pattern_context(s);
	pattern_context.pattern_sign = pattern_sign;
	pattern_context.all_results = all_results;
	pattern_context.setPairResult = result_pair_set;

	if (prepare_pattern(info, pattern_context, param_pairs, pair_set) != RV_OK) {
		return RV_ERR_GEN;
	}

	nsm_pattern cur_nsm_pattern(s, pattern_context.pattern_sign, is_segment(s, info->defaultseg_sign));
	if (cur_nsm_pattern.gen()) {
		pattern_context.results = cur_nsm_pattern.get_results();
		pack_result_pattern(info, pattern_context);
		cur_nsm_pattern.set_need_clean(!pattern_context.setPairResult);
	}

	return RV_OK;
}
