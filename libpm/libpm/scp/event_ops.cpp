
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
 * @file event_ops.cpp
 * @brief Implementation SCP-operators for SC-event handling.
 * @ingroup libpm
 **/

#include "precompiled_p.h"

// this file contains complete hack

inline
sc_addr	__ref(sc_addr a)
{
	return a?a->ref():a;
}

class ev_handler : public sc_wait {
protected:
	sc_addr proc;
	sc_addr elem;
	sc_addr descr;
	sc_session *session;
public:
	ev_handler(sc_addr _proc, sc_addr _elem)
		: proc(__ref(_proc)), elem(_elem->ref())
	{
		descr = 0;
		session = libsc_login();
		session->open_segment(proc->seg->get_full_uri());
		session->open_segment(elem->seg->get_full_uri());
	}
	// easy way
	void set_descr(sc_addr _descr)
	{
		if (_descr == descr)
			return;
		session->open_segment(_descr->seg->get_full_uri());
		if (descr) {
			session->open_segment(descr->seg->get_full_uri());
			descr->unref();
		}
		descr = _descr->ref();
	}
	sc_addr get_descr() {return descr;}
	~ev_handler()
	{
		proc->unref();
		session->close_segment(proc->seg);
		if (descr) {
			descr->unref();
			session->close_segment(descr->seg);
		}
		elem->unref();
		session->close_segment(elem->seg);
		session->close();
	}
};

class wakeup_activity : public sc_activity {
	sc_addr towake;
	bool *res;
	sc_addr *father;
	sc_addr var;
public:
	wakeup_activity(sc_addr _towake,bool *_res,sc_addr *_father,sc_addr _var)
		: towake(_towake), res(_res), father(_father), var(_var)
	{
	}
	sc_retval init(sc_addr _this) {return RV_OK;}
	void done() {}
	sc_retval activate(sc_session *s,sc_addr _this,sc_addr param1,sc_addr prm2,sc_addr prm3)
	{
		scp_process_info info;
		scp_fill_process_info(*father,&info);
		sc_addr value = scp_get_var_value(&info,var);
		if (!value) {
			fprintf(stderr,"wakeup_activity: cannot get variable value\n");
			SC_ABORT();
		}
		*res = (value == EV_ALLOW) ? false : true;
		pm_sched_wakeup(towake);
		return RV_OK;
	}
};

class pre_ev_handler : public ev_handler {
public:
	pre_ev_handler(sc_addr _proc, sc_addr _elem)
		: ev_handler(_proc ,_elem)
	{
	}
	bool activate(sc_wait_type type,sc_param *prm,int len)
	{
		sc_addr towake = pm_sched_get_current();
		bool result;
		sc_addr var;
		sc_addr father;
		assert(towake);
		sc_segment *seg = create_unique_segment(system_session,
						"/tmp/pre_ev_handler");
		var = system_session->create_el(seg,SC_N_CONST);
		sc_addr ondeath = system_session->create_el(scp_segment,SC_N_CONST);
		system_session->reimplement(ondeath,
				new wakeup_activity(towake,&result,&father,var));
		sc_addr *prms = new sc_addr[len+2];
		bool	*consts = new bool[len+2];
		prms[1] = var;
		consts[1] = false;
		prms[0] = descr;
		consts[0] = true;
		for (int i=2;i<len+2;i++) {
			consts[i] = true;
			prms[i] = prm[i-2].addr;
			if (prms[i]->seg->ring_0)
				// just skip events in ring_0
				return false;
		}
		if (descr->dead) {
			descr->unref();
			sc_segment *seg = create_unique_segment(system_session,"/tmp/bugfix");
			descr = system_session->create_el(seg,SC_N_CONST);
			descr->ref();
			prms[0] = descr;
		}
		// hack (?)
		system_session->detach_wait(this);
		sc_addr son = create_orphaned_son(libsc_login(), proc, ondeath, &father, prms, consts, len + 2);
		if (!son) {
			fprintf(stderr,"Failed to start event handler. Event skiped\n");
			goto out;
		}
		if (pm_sched_add_agent(son,SCHED_CLASS_NORMAL))
			SC_ABORT();
	out:
		system_session->attach_wait(type,prm,len,this);
		pm_sched_put_to_sleep_and_switch(towake);
		return result;
	}
};

class post_ev_handler : public ev_handler
{
public:
	post_ev_handler(sc_addr _proc, sc_addr _elem)
		: ev_handler(_proc, _elem)
	{
	}

	bool activate(sc_wait_type type,sc_param *prm,int len)
	{
		boost::scoped_array<sc_addr> prms(new sc_addr[len+1]);
		boost::scoped_array<bool>    consts(new bool[len+1]);

		prms[0] = descr;
		consts[0] = true;

		for (int i = 1;i < len + 1; ++i) {
			consts[i] = true;
			prms[i] = prm[i-1].addr;
			if (prms[i]->seg->ring_0)
				// just skip events in ring_0
				return false;
		}

		if (descr->dead) {
			descr->unref();
			sc_segment *seg = create_unique_segment(system_session,"/tmp/bugfix");
			descr = system_session->create_el(seg,SC_N_CONST);
			descr->ref();
			prms[0] = descr;
		}

		sc_addr son = create_orphaned_son(libsc_login(), proc, 0, 0, prms.get(), consts.get(), len + 1);
		if (!son) {
			fprintf(stderr,"Failed to start event handler. Event skiped\n");
			return false;
		}

		if (pm_sched_add_agent(son,SCHED_CLASS_NORMAL))
			SC_ABORT();

		return false;
	}
};

struct scp2sc_event
{
	sc_addr *kn;
	sc_wait_type wt;
};

static scp2sc_event scp2sc_event_map[] = {
	{&WAIT_GEN_OUTPUT_ARC,     SC_WAIT_HACK_SET_MEMBER},
	{&WAIT_GEN_INPUT_ARC,      SC_WAIT_HACK_IN_SET},
	{&WAIT_ERASE_ELEMENT,      SC_WAIT_DIE},
	{&WAIT_GEN_INPUT_ARC_PRE,  SC_WAIT_HACK_IN_SET_PRE},
	{&WAIT_GEN_OUTPUT_ARC_PRE, SC_WAIT_HACK_SET_MEMBER_PRE},
	{&WAIT_ERASE_ELEMENT_PRE,  SC_WAIT_DIE_PRE_REAL}
};
static const int scp2sc_event_map_pre_index = 3;

SCP_OPERATOR_IMPL(scp_oper_setevhandler)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd_evtype);
	SCP_OPND_REQ_FIXED(info, 2, opnd_proc);
	SCP_OPND_REQ_FIXED(info, 3, opnd_tuple);
	SCP_OPND_OPT_VAR(info, 4, opnd_descrinfo);

	sc_addr descrseg;
	if (!opnd_descrinfo.opnd || get_single_segc(info, 4, &descrseg))
		descrseg = info->defaultseg_sign;

	sc_addr event_param = sc_tup::at(system_session, opnd_tuple.value, N1_);
	if (event_param == SCADDR_NIL) {
		SCP_ERROR_F(info->process, "Operand %s must be tuple, which has component with attribute 1_.",
			opnd_tuple.get_name());
		return RV_ERR_GEN;
	}

	if (is_variable(info, event_param)) {
		if (get_var_value(info, event_param, 0, &event_param)) {
			SCP_ERROR_F(info->process, "Component with attribute 1_ from value of operand %s must be a variable, which has value.",
				opnd_tuple.get_name());
			return RV_ERR_GEN;
		}
	}

	ev_handler *handler;
	int index;
	for (index = sizeof(scp2sc_event_map) / sizeof(scp2sc_event) - 1; index >= 0; --index) {
		if (*(scp2sc_event_map[index].kn) == opnd_evtype.value)
			break;
	}

	if (index < 0) {
		SCP_ERROR_F(info->process, "Not found event type \"%s\" from value of operand %s.",
			FULL_IDTF(opnd_evtype.value).c_str(), opnd_tuple.get_name());
		return RV_ERR_GEN;
	}

	if (index < scp2sc_event_map_pre_index) {
		handler = new post_ev_handler(opnd_proc.value, event_param);
	} else {
		handler = new pre_ev_handler(opnd_proc.value, event_param);
	}

	system_session->attach_wait(scp2sc_event_map[index].wt, &SC_ADDR_AS_PARAM(event_param), 1, handler);
	sc_addr descr = system_session->create_el(is_segment(system_session, descrseg), SC_N_CONST);
	handler->set_descr(descr);

	if (opnd_descrinfo.opnd)
		scp_set_variable(info,opnd_descrinfo.opnd,descr);

	{
		Cont c;
		c.i = reinterpret_cast<cint>(handler);
		system_session->set_content(descr, Content(c, TCINT));
	}

	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_delevhandler)
{
	SCP_OPND_REQ_FIXED(info, 1, descr);

	Content c = info->session->get_content(descr.value);
	if (c.type() != TCINT)
		return RV_ERR_GEN;
	info->session->detach_wait((sc_wait *)(Cont(c).i));
	return RV_OK;
}

SCP_OPERATOR_IMPL(scp_oper_sys_wait)
{
	SCP_OPND_REQ_FIXED(info, 1, opnd_evtype);
	SCP_OPND_REQ_FIXED(info, 2, opnd_tuple);

	sc_addr event_param = sc_tup::at(system_session, opnd_tuple.value, N1_);
	if (!event_param) {
		SCP_ERROR_F(info->process, "Operand %s must be tuple, which has component with attribute 1_.",
			opnd_tuple.get_name());
		return RV_ERR_GEN;
	}

	if (is_variable(info, event_param)) {
		if (get_var_value(info, event_param, 0, &event_param)) {
			SCP_ERROR_F(info->process, "Component with attribute 1_ from value of operand %s must be a variable, which has value.",
				opnd_tuple.get_name());
			return RV_ERR_GEN;
		}
	}

	int index;
	for (index = sizeof(scp2sc_event_map) / sizeof(scp2sc_event) - 1; index >= 0; --index) {
		if (*(scp2sc_event_map[index].kn) == opnd_evtype.value)
			break;
	}

	if (index < 0) {
		SCP_ERROR_F(info->process, "Not found event type \"%s\" from value of operand %s.",
			FULL_IDTF(opnd_evtype.value).c_str(), opnd_tuple.get_name());
		return RV_ERR_GEN;
	}

	info->session->attach_good_wait(scp2sc_event_map[index].wt, &SC_ADDR_AS_PARAM(event_param), 1, 
		new sleep_until_event_wait(system_session, info->process, scp2sc_event_map[index].wt, event_param));
	pm_sched_put_to_sleep_and_switch(info->process);

	return RV_OK;
}
