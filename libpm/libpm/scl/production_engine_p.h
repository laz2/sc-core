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
 * @addtogroup libpm
 * @{
 */

/**
 * @file production_engine.h
 * @brief Interface of production engine subsystem.
 * @author Dmitry Lazurkin
 */

#ifndef __PRODUCTIN_ENGINE_H__
#define __PRODUCTIN_ENGINE_H__

#include <libpm/pm.h>
#include <libpm/sc_operation.h>
#include "scl_keynodes.h"

namespace scl 
{
	sc_retval init_productions_engine(sc_session *s);

	/**
	 * @brief SC-operation "Production Engine"
	 * Actiovation condition:
	 * @code
	 *   "production_engine" ->> _productions
	 * @endcode
	 * 
	 * Create process #scl.productions_activity for productions set "_productions". 
	 */    
	class productions_operation : public ScOperationActSetMember
	{
	public:
		productions_operation(sc_session *_s) : ScOperationActSetMember("Production Engine", PRODUCTION_ENGINE), s(_s) {}

		virtual bool activateImpl(sc_addr paramSystem, sc_segment *tmpSegment);
	private:
		sc_session *s;
	};

	/// Process which evaluate productions.
	class productions_activity : public basic_advanced_activity
	{
	public:
		
		productions_activity(sc_session *_s, sc_segment *_seg, sc_addr _productions);

		virtual ~productions_activity();

		virtual sc_retval activate(sc_session *s, sc_addr _this, sc_addr prm1, sc_addr prm2, sc_addr prm3);

	private:
		sc_session *s;
		sc_segment *segment;
		int count_not_applied; ///< Count of productions which not applied to knowledge database.
		addr_vector productions; 
		int cur_index; /// Index of current production in #productions.
	};
}

#endif // __PRODUCTIN_ENGINE_H__

/**@}*/
