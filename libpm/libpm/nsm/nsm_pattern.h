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
 * @file nsm_pattern.h
 * @brief Helper for working with NSM patterns.
 * @author Dmitry Lazurkin
 */

#ifndef __NSM_PATTERN_H__
#define __NSM_PATTERN_H__

#include <libpm/pm.h>

#include <libsc/sc_smart_addr.h>

#include "nsm_keynodes.h"

#include <map>

/// Class for pattern searching and generating with NSM.
class LIBPM_API nsm_pattern
{
public:
	/**
	 * @param _s session for working.
	 * @param pattern_sign pattern system sign.
	 * @param _seg segment for working.
	 */
	nsm_pattern(sc_session *_s, sc_addr pattern_sign, sc_segment *_seg) : s(_s), sign(pattern_sign), 
		segment(_seg), results(0), need_clean(true) {}
	
	virtual ~nsm_pattern() { clean_if_need(); } 

	/**
	 * @brief Sync search with NSM.
	 * @note Switch context with pm_sched functionality.
	 */
	virtual bool search()
	{
		clean_if_need();
		return gen_goal_and_run(sign, NSM_SEARCH_, NSM_SEARCHED_);
	}

	/// Async search with NSM
	virtual sc_addr search_async()
	{
		clean_if_need();
		return gen_goal(sign, NSM_SEARCH_);
	}

	/**
	 * @brief Sync generate with NSM.
	 * @note Switch context with pm_sched functionality.
	 */
	virtual bool gen()
	{
		clean_if_need();
		return gen_goal_and_run(sign, NSM_GENERATE_, NSM_GENERATED_);
	}

	/// Async gen with NSM
	virtual sc_addr gen_async()
	{
		clean_if_need();
		return gen_goal(sign, NSM_GENERATE_);
	}

	sc_addr get_results() const { return results; }
	
	sc_addr get_sign() const { return sign; }

	void set_sign(sc_addr _sign) 
	{
		if (_sign != sign) {
			sign = _sign;
			clean_if_need();
		}
	}

	sc_session *get_session() const { return s; }
	
	void set_session(sc_session *_s) { s = _s; }

	sc_segment *get_segment() const { return segment; }
	
	void set_segment(sc_segment *_seg) { segment = _seg; }
	
	/// If true nsm_pattern must erase all temporary NSM structures.
	bool is_need_clean() const { return need_clean; }

	void set_need_clean(bool flag) { need_clean = flag; }
	
	/// Erase all temporary NSM structures.
	virtual void clean();

protected:
	/// Generate NSM-goal with attr @p goal_attr for pattern @p pattern_sign. Sleep until generate arc from @p result_attr.
	bool gen_goal_and_run(sc_addr pattern_sign, sc_addr goal_attr, sc_addr result_attr);
	
	/// Generate NSM-goal with attr @p goal_attr for pattern @p pattern_sign.
	sc_addr gen_goal(sc_addr pattern_sign, sc_addr goal_attr);

	void clean_if_need() 
	{
		if (need_clean)
			clean();
	}

protected:
	sc_addr sign; ///< Pattern sign.
	sc_session *s;
	sc_segment *segment;
	sc_addr goal; ///< NSM-goal sign.
	sc_addr results;
	bool need_clean; ///< If true nsm_pattern must erase all temporary NSM structures.
};

/// Class for parametrized pattern searching and generating with NSM.
class LIBPM_API nsm_pattern_with_params : public nsm_pattern
{
public:
	typedef addr2addr_map params_map;
	
	/**
	 * @param _s session for working.
	 * @param pattern_sign pattern system sign.
	 * @param _seg segment for working.
	 */
	nsm_pattern_with_params(sc_session *_s, sc_addr pattern_sign, sc_segment *_seg) : nsm_pattern(_s, pattern_sign, _seg), sign_with_params(0) {}

	params_map &get_params_map() { return params; }

	virtual bool search();

	virtual bool gen();
	
	virtual sc_addr search_async()
	{
		return gen_goal_for_async(NSM_SEARCH_, NSM_SEARCHED_);
	}

	virtual sc_addr gen_async()
	{
		return gen_goal_for_async(NSM_GENERATE_, NSM_GENERATED_);
	}

	virtual void clean();

protected:
	/// Generate goal for async searching or generating.
	sc_addr gen_goal_for_async(sc_addr goal_type, sc_addr result_attr);

	/// Copy pattern and substitute params.
	sc_addr substitute_params();

	/// Substitute in results copied params with original params.
	void substitute_in_results();

	/// Waits for end search or generating event and substitute in results.
	class substitute_in_results_waiter : public sc_wait
	{
	public:
		substitute_in_results_waiter(sc_session *_s, nsm_pattern_with_params& _p, sc_addr _cond) : 
		  s(_s), p(_p), cond(_cond) {}

		virtual bool activate(sc_wait_type type, sc_param *params, int len)
		{
			assert(type == SC_WAIT_HACK_IN_SET);
			
			if (params[0].addr == cond) {
				p.substitute_in_results();
				s->detach_wait(this);
				// TODO: Need delete this object? Think about this.
			}

			return true;
		}

	private:
		sc_smart_addr cond;
		nsm_pattern_with_params& p;
		sc_session *s;
	};

	friend class substitute_in_results_waiter;

	/// Attach waiter for call substitute_in_results.
	void attach_substitute_in_results_waiter(sc_addr arc_goal, sc_addr result_attr);

	/**
	 * @brief Callback for copying patterns vars/metavars.
	 * @see #sc_set.copy
	 */
	static void cb_copy_pattern(sc_addr original, sc_addr copy, void *cb_data);

protected:
	params_map params;
	sc_addr sign_with_params; ///< Copied pattern with substituted params.
	sc_segment *segment_for_copy; ///< Temporary segment for pattern copy.
	addr2addr_map original2copy;
	addr2addr_map copy2original;
};

#endif // __NSM_PATTERN_H__

/**@}*/
