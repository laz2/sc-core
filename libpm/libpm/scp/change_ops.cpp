
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
 * @file change_ops.cpp
 * @brief Implementation SCP-operators for changing SC-elements.
 * @ingroup libpm
 */

#include "precompiled_p.h"

SCP_OPERATOR_IMPL(scp_oper_varAssign)
{
	SCP_OPND_REQ_VAR(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	return scp_set_variable(info, opnd1.opnd, opnd2.value);
}

SCP_OPERATOR_IMPL(scp_oper_varErase)
{
	SCP_OPND_REQ_VAR(info, 1, opnd1);
	scp_clear_variable(info, opnd1.opnd);
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_contAssign)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	Content cont2 = info->session->get_content(opnd2.value);
	return info->session->set_content(opnd1.value, cont2);
}

SCP_OPERATOR_IMPL(scp_oper_contErase)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	return info->session->erase_content(opnd1.value);
}

SCP_OPERATOR_IMPL(scp_oper_add)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_FIXED(info, 3, opnd3);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);
	Content cont3 = info->session->get_content(opnd3.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		switch (cont3.type()) {
		case TCINT:
			_cont1.i = Cont(cont2).i + Cont(cont3).i;
			cont_type = TCINT;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).i + Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCREAL:
		switch (cont3.type()) {
		case TCINT:
			_cont1.r = Cont(cont2).r + Cont(cont3).i;
			cont_type = TCREAL;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).r + Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCSTRING:
		if (cont3.type() != TCSTRING)
			return RV_ERR_GEN;
		_cont1.d.size = Cont(cont2).d.size + Cont(cont3).d.size-1;
		if (NULL == (_cont1.d.ptr = new char[_cont1.d.size]))
			return RV_ERR_GEN;
		memcpy(_cont1.d.ptr, Cont(cont2).d.ptr, Cont(cont2).d.size-1);
		memcpy(_cont1.d.ptr + Cont(cont2).d.size-1, Cont(cont3).d.ptr, Cont(cont3).d.size);
		cont_type = TCSTRING;
		break;
	default:
		return RV_ERR_GEN;
	}

	return info->session->set_content(result, Content(_cont1, cont_type));
}

SCP_OPERATOR_IMPL(scp_oper_sub)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_FIXED(info, 3, opnd3);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);
	Content cont3 = info->session->get_content(opnd3.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		switch (cont3.type()) {
		case TCINT:
			_cont1.i = Cont(cont2).i - Cont(cont3).i;
			cont_type = TCINT;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).i - Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCREAL:
		switch (cont3.type()) {
		case TCINT:
			_cont1.r = Cont(cont2).r - Cont(cont3).i;
			cont_type = TCREAL;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).r - Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	default:
		return RV_ERR_GEN;
	}

	return info->session->set_content(result, Content(_cont1, cont_type));
}

SCP_OPERATOR_IMPL(scp_oper_mult)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_FIXED(info, 3, opnd3);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);
	Content cont3 = info->session->get_content(opnd3.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		switch (cont3.type()) {
		case TCINT:
			_cont1.i = Cont(cont2).i * Cont(cont3).i;
			cont_type = TCINT;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).i * Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCREAL:
		switch (cont3.type()) {
		case TCINT:
			_cont1.r = Cont(cont2).r * Cont(cont3).i;
			cont_type = TCREAL;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).r * Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCSTRING:
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

SCP_OPERATOR_IMPL(scp_oper_div)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_FIXED(info, 3, opnd3);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);
	Content cont3 = info->session->get_content(opnd3.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		switch (cont3.type()) {
		case TCINT:
			if (Cont(cont3).i == 0)
				return RV_ERR_GEN;
			_cont1.i = Cont(cont2).i / Cont(cont3).i;
			cont_type = TCINT;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).i / Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCREAL:
		switch (cont3.type()) {
		case TCINT:
			_cont1.r = Cont(cont2).r / Cont(cont3).i;
			cont_type = TCREAL;
			break;
		case TCREAL:
			_cont1.r = Cont(cont2).r / Cont(cont3).r;
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCSTRING:
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

SCP_OPERATOR_IMPL(scp_oper_sin)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		_cont1.r = sin(double(Cont(cont2).i));
		cont_type = TCREAL;
		break;
	case TCREAL:
		_cont1.r = sin(Cont(cont2).r);
		cont_type = TCREAL;
		break;
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

SCP_OPERATOR_IMPL(scp_oper_asin)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		if((Cont(cont2).i>1)||(Cont(cont2).i<-1))
			return RV_ERR_GEN;
		_cont1.r = asin(double(Cont(cont2).i));
		cont_type = TCREAL;
		break;
	case TCREAL:
		if((Cont(cont2).r>1)||(Cont(cont2).r<-1))
			return RV_ERR_GEN;
		_cont1.r = asin(Cont(cont2).r);
		cont_type = TCREAL;
		break;
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

SCP_OPERATOR_IMPL(scp_oper_cos)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		_cont1.r = cos(double(Cont(cont2).i));
		cont_type = TCREAL;
		break;
	case TCREAL:
		_cont1.r = cos(Cont(cont2).r);
		cont_type = TCREAL;
		break;
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

SCP_OPERATOR_IMPL(scp_oper_acos)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		if((Cont(cont2).i>1)||(Cont(cont2).i<-1))
			return RV_ERR_GEN;
		_cont1.r = acos(double(Cont(cont2).i));
		cont_type = TCREAL;
		break;
	case TCREAL:
		if((Cont(cont2).r>1)||(Cont(cont2).r<-1))
				return RV_ERR_GEN;
		_cont1.r = acos(Cont(cont2).r);
		cont_type = TCREAL;
		break;
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

SCP_OPERATOR_IMPL(scp_oper_pow)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_FIXED(info, 3, opnd3);

	sc_addr result = result_opnd.value;

	Content cont2 = info->session->get_content(opnd2.value);
	Content cont3 = info->session->get_content(opnd3.value);

	Cont _cont1;
	TCont cont_type;

	switch (cont2.type()) {
	case TCINT:
		switch (cont3.type()) {
		case TCINT:
			if ((Cont(cont2).i == 0) && (Cont(cont3).i == 0))
				return RV_ERR_GEN;
			_cont1.r = pow((double)Cont(cont2).i, Cont(cont3).i);
			cont_type = TCREAL;
			break;
		case TCREAL:
			if ((Cont(cont2).i == 0) && (Cont(cont3).r == 0.0))
				return RV_ERR_GEN;
			_cont1.r = pow((double)Cont(cont2).i, Cont(cont3).r);
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCREAL:
		switch (cont3.type()) {
		case TCINT:
			if ((Cont(cont2).r == 0.0) && (Cont(cont3).i == 0))
				return RV_ERR_GEN;
			_cont1.r = pow(Cont(cont2).r, Cont(cont3).i);
			cont_type = TCREAL;
			break;
		case TCREAL:
			if ((Cont(cont2).r == 0.0)
				&& (Cont(cont3).r == 0.0)) return RV_ERR_GEN;
			_cont1.r = pow(Cont(cont2).r, Cont(cont3).r);
			cont_type = TCREAL;
			break;
		default:
			return RV_ERR_GEN;
		}
		break;
	case TCSTRING:
	default:
		return RV_ERR_GEN;
	}

	return (info->session->set_content(result, Content(_cont1, cont_type)));
}

/**
 * @brief Реализация scp-оператора floor, который выполняет округления числового контента вниз.
 * @code
 * floor([ 1_: fixed_: number ])
 * @endcode
 **/
SCP_OPERATOR_IMPL(scp_oper_floor)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	Content cont = info->session->get_content(opnd1.value);

	if (cont.type() == TCINT) {
		return RV_OK;
	} else if (cont.type() == TCREAL) {
		Cont c;
		c.r = (int) floor(Cont(cont).r);
		Content cont2(c, TCREAL);
		if (info->session->set_content(opnd1.value, cont2)) {
			return RV_ERR_GEN;
		}

		scp_set_state(info->process, SCP_STATE_THEN);

		return RV_OK;
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);

		return RV_OK;
	}
}

/**
 * @brief Реализация scp-оператора ceil, который выполняет округления числового контента вверх.
 * @code
 * ceil([ 1_: fixed_: number ])
 * @endcode
 **/
SCP_OPERATOR_IMPL(scp_oper_ceil)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);

	Content cont = info->session->get_content(opnd1.value);

	if (cont.type() == TCINT) {
		return RV_OK;
	} else if (cont.type() == TCREAL) {
		Cont c;
		c.r = (int) ceil(Cont(cont).r);
		Content cont2(c, TCREAL);
		if (info->session->set_content(opnd1.value, cont2)) {
			return RV_ERR_GEN;
		}

		scp_set_state(info->process, SCP_STATE_THEN);

		return RV_OK;
	} else {
		scp_set_state(info->process, SCP_STATE_ELSE);
		return RV_OK;
	}
}

/**
 * @brief Реализация scp-оператора to_str, который выполняет преобразование контента в строковый контент.
 * @code
 * to_str([ 1_: (assign_:|fixed_:) result, 2_: fixed_: content_element ])
 * @endcode
 **/
SCP_OPERATOR_IMPL(scp_oper_to_str)
{
	SCP_OPND_REQ_ASSIGN_FIXED_CREATE(info, 1, result_opnd);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);

	Content c = info->session->get_content(opnd2.value);
	std::ostringstream os;
	sc_retval rv = RV_THEN;

	if (info->session->error)
		return RV_ERR_GEN;
	switch (c.type()) {
	case TCSTRING:
		c = Content(Cont::string((char *)c),TCSTRING);
		break;
	case TCDATA:
		return RV_ERR_GEN;
	case TCEMPTY:
		c = Content(Cont::string(""),TCSTRING);
		rv = RV_ELSE_GEN;
		break;
	case TCINT:
		os << Cont(c).i;
		c = Content(Cont::string(os.str().c_str()),TCSTRING);
		break;
	case TCREAL:
		os << Cont(c).r;
		c = Content(Cont::string(os.str().c_str()),TCSTRING);
		break;
	default:
		return RV_ERR_GEN;
	}
	if (info->session->set_content(result_opnd.value, c))
		return RV_ERR_GEN;
	return rv;
}

/**
 * @brief Реализация scp-оператора gsub, который выполняет замену в строковом содержимом первого операнда всех
 * вхождений строкового содержимого второго операнда на строковое содержимое третьего операнда.
 * @code
 * gsub([ 1_: fixed_: result_string, 2_: fixed_: find_string, 3_: fixed_: replace_string ])
 * @endcode
 **/
SCP_OPERATOR_IMPL(scp_oper_gsub)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd1);
	SCP_OPND_REQ_FIXED(info, 2, opnd2);
	SCP_OPND_REQ_FIXED(info, 3, opnd3);

	Content cont1 = info->session->get_content(opnd1.value);
	Content cont2 = info->session->get_content(opnd2.value);
	Content cont3 = info->session->get_content(opnd3.value);

	if (cont1.type() != TCSTRING || cont2.type() != TCSTRING || cont3.type() != TCSTRING) {
		return RV_ERR_GEN;
	}

	sc_string result_string = cont1;
	sc_string find_string = cont2;
	sc_string replace_string = cont3;

	for(size_t index = 0; index = result_string.find(find_string, index), index != std::string::npos;)
	{
		result_string.replace(index, find_string.length(), replace_string);
		index += replace_string.length();
	}

	info->session->set_content(opnd1.value, result_string);

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_sys_merge_element)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd_dst);
	SCP_OPND_REQ_FIXED(info, 2, opnd_src);

	sc_retval rv = sc_is_mergeable(info->session, opnd_dst.value, opnd_src.value);
	if (rv == RV_OK)
		rv = sc_merge_elements(info->session, opnd_dst.value, opnd_src.value);

	return rv;
}
