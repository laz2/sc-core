
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

#include "scp_cache_p.h"

typedef std::map<sc_addr, scp_cache *> cache_map_t;

cache_map_t cache_map;

void dispose_cache(sc_addr var_value) 
{
	cache_map_t::iterator iter = cache_map.find(var_value);
	if (iter == cache_map.end()) {
		return;
	} 
	delete iter->second;
	cache_map.erase(iter);
}

void init_cache(sc_addr var_value) 
{
	scp_cache *ptr = new scp_cache;
	cache_map[var_value] = ptr;
}

static scp_cache *find_cache(sc_addr var_value) 
{
	cache_map_t::iterator i = cache_map.find(var_value);
	if (i == cache_map.end()) {
		SC_ABORT();
	} 
	return i->second;
}

sc_retval scp_vars_init(sc_session *system) 
{
	return RV_THEN;
}

bool is_variable(scp_process_info *info, sc_addr addr)
{
	return search_3_f_cpa_f(system_session, info->vars, 0, addr) == RV_OK;
}

sc_retval __get_var_value(sc_addr var_value, sc_addr var, sc_addr *conn, sc_addr *value, sc_addr *valueArc)
{
	scp_cache *cache = find_cache(var_value);
	if (cache->get(var, value, conn, valueArc)) {
		return RV_OK;
	} else if (!cache->replaces) {
		return RV_ELSE_GEN;
	} else {
		sc_addr _conn, _value, _valueArc;
		if (search_oneshot(system_session, sc_constraint_new(
			  CONSTR_ORD_BIN_CONN1,
			  var_value,
			  SC_A_CONST|SC_POS,
			  SC_N_CONST,  /* conn */
			  SC_A_CONST|SC_POS,
			  var,
			  SC_A_CONST|SC_POS,
			  0,  /* value */
			  SC_A_CONST|SC_POS,
			  N1_,
			  SC_A_CONST|SC_POS,
			  N2_), true, 3, 2, &_conn, 5, &_valueArc, 6, &_value)) {
				return RV_ELSE_GEN;
		}

		cache->put(var, _value, _conn, _valueArc);

		if (conn)
			*conn = _conn;

		if (value)
			*value = _value;

		if (valueArc)
			*valueArc = _valueArc;

		return RV_OK;
	}
}

sc_retval get_var_value(scp_process_info *info, sc_addr var, sc_addr *conn, sc_addr *value, sc_addr *valueArc)
{
	if (!is_variable(info, var)) {
		return RV_ELSE_GEN;
	}

	scp_cache *cache = find_cache(info->var_value);
	if (cache->get(var, value, conn, valueArc)) {
		return RV_OK;
	} else if (!cache->replaces) {
		return RV_ELSE_GEN;
	} else {
		sc_addr _conn, _value, _valueArc;
		if (search_oneshot(system_session, sc_constraint_new(
				CONSTR_ORD_BIN_CONN1,
				info->var_value,
				SC_A_CONST|SC_POS,
				SC_N_CONST,  /* conn */
				SC_A_CONST|SC_POS,
				var,
				SC_A_CONST|SC_POS,
				0,  /* value */
				SC_A_CONST|SC_POS,
				N1_,
				SC_A_CONST|SC_POS,
				N2_), true, 3, 2, &_conn, 5, &_valueArc, 6, &_value)) {
			return RV_ELSE_GEN;
		}

		cache->put(var, _value, _conn, _valueArc);

		if (conn) {
			*conn = _conn;
		}

		if (value) {
			*value = _value;
		}

		if (valueArc) {
			*valueArc = _valueArc;
		}

		return RV_OK;
	}
}

sc_addr scp_get_var_value(scp_process_info *pi, sc_addr var)
{
	sc_addr value;

	if (get_var_value(pi, var, 0, &value)) {
		return 0;
	}

	return value;
}

sc_retval __set_variable(sc_addr var_value, sc_addr var, sc_addr value)
{
	sc_addr valueArc;
	sc_addr conn = sc_rel::add_ord_tuple(system_session, var_value, var, value, 0, &valueArc);
	scp_cache *cache = find_cache(var_value);
	cache->put(var, value, conn, valueArc);
	return RV_OK;
}

sc_retval __set_variable(scp_process_info *info, sc_addr var, sc_addr value)
{
	return __set_variable(info->var_value, var, value);
}

sc_retval scp_set_variable(scp_process_info *info, sc_addr var, sc_addr value)
{
	sc_addr value_arc, conn;
	sc_retval rv;
	rv = get_var_value(info, var, &conn, 0, &value_arc);
	if (RV_IS_ERR(rv))
		return rv;

	if (rv)
		return __set_variable(info, var, value);

	system_session->set_end(value_arc, value);

	scp_cache *cache = find_cache(info->var_value);
	cache->put(var, value, conn, value_arc);

	return RV_OK;
}

void __clear_variable(sc_addr var_value, sc_addr var)
{
	sc_addr conn;
	if (__get_var_value(var_value, var, &conn, 0, 0))
		return;

	scp_cache *cache = find_cache(var_value);
	cache->clear(var);
	system_session->erase_el(conn);
}

void scp_clear_variable(scp_process_info *info, sc_addr var)
{
	__clear_variable(info->var_value, var);
}

void scp_clear_this_value_variables(sc_addr var_value, sc_addr value) {
	sc_iterator *iter = system_session->create_iterator(
		sc_constraint_new(
			CONSTR_ORD_BIN_CONN1,
			var_value,
			SC_A_CONST|SC_POS,
			SC_N_CONST, /* conn */
			SC_A_CONST|SC_POS,
			value,
			SC_A_CONST|SC_POS,
			0, /* var */
			SC_A_CONST|SC_POS,
			N2_,
			SC_A_CONST|SC_POS,
			N1_),
	true);
	auto_sc_iterator_ptr autoIter(iter);

	for (; !iter->is_over(); iter->next()) {
		sc_addr conn = iter->value(2);
		system_session->erase_el(conn);
	}
}
