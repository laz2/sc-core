
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
 * @file sc_operation.cpp
 * @author Lazurkin Dmitriy
 * @brief Реализация подсистемы sc-операций.
 */

#include "libpm_precompiled_p.h"

sc_segment *operationsSeg;
sc_addr operationsSet;

sc_retval init_sc_operations(sc_session *s)
{
	operationsSeg = s->create_segment(OPERATIONS_SEGURI);
	operationsSet = create_el(s, operationsSeg, SC_N_CONST);
	return RV_OK;
}

sc_addr ScOperation::registerOperation()
{
	operation = create_el(system_session, operationsSeg, SC_N_CONST);
	
	system_session->set_idtf(operation, name);
	
	sc_include_in_set(system_session, operationsSeg, operationsSet, operation);

	sc_generator gen(system_session, operationsSeg);
	gen.element(operation);
	gen.arc();
	gen.attr(VAR_VALUE_);
	actCondParams = gen.element("");

	actCond = gen.enter_system("");
	genActCond(gen);
	gen.leave_system();
	gen.finish();
	return actCond;
}

void ScOperation::bindParam(sc_addr param, sc_addr value)
{
	sc_rel::add_ord_tuple(system_session, actCondParams, param, value);
}

sc_segment *ScOperation::createTmpSegment()
{
	return create_unique_segment(system_session, "/tmp/" + name);
}

bool ScOperation::activate(sc_wait_type type, sc_param *prm, int len)
{
	sc_segment *tmpSeg = createTmpSegment();
	
	sc_addr paramSystem = create_el(system_session, tmpSeg, SC_N_CONST);
	for (int i = 0; i < len; i++) {
		sc_include_in_set(system_session, tmpSeg, paramSystem, prm[i].addr);
	}

	return activateImpl(paramSystem, tmpSeg);
}