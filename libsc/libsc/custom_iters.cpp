
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

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

class std5_f_a_a_a_f_iter: public sc_iterator
{
public:
	sc_addr regs[5];
#define e1 (regs[0])
#define e2 (regs[1])
#define e3 (regs[2])
#define e4 (regs[3])
#define e5 (regs[4])
	sc_type t2,t3,t4;
	sc_iterator *e1_output,*e2_input;
	sc_session *s;
	bool over;

	bool e2_loop()
	{
		for (;!e2_input->is_over();e2_input->next()) {
			e4 = e2_input->value(1);
			sc_segment *e4seg = e4->seg;
			if (!__check_sc_type(e4seg->get_type(e4->impl),t4))
				continue;
			sc_addr_ll ll = e4seg->get_beg(e4->impl);
			if (!ll)
				continue;
			if (e4seg->get_element_addr(ll) == e5)
				return true;
		}
		return false;
	}
	void e1_loop()
	{
		for (;!e1_output->is_over();e1_output->next()) {
			e2 = e1_output->value(1);
			sc_segment *e2seg = e2->seg;
			if (!__check_sc_type(e2seg->get_type(e2->impl),t2))
				continue;
			sc_addr_ll ll = e2seg->get_end(e2->impl);
			if (!ll)
				continue;
			e3 = e2seg->get_element_addr(ll);
			if (!e3 || !s->is_segment_opened(e3->seg))
				continue;
			if (!__check_sc_type(e3->seg->get_type(e3->impl),t3))
				continue;
			e3->ref();
			e2_input = create_input_iterator(s,e2);
			if (e2_loop())
				return;
			delete e2_input;
			e2_input = 0;
			e3->unref();
		}
		over = true;
	}

	std5_f_a_a_a_f_iter(
			sc_session *_s,
			sc_addr _e1,sc_addr _e5,
			sc_type _t2,sc_type _t3,sc_type _t4
	) {
		s = _s;
		over = false;
		e1 = _e1;
		e5 = _e5->ref();
		t2 = sc_type_extend_mask(_t2);
		t3 = sc_type_extend_mask(_t3);
		t4 = sc_type_extend_mask(_t4);

		e2_input = 0;
		e1_output = create_output_iterator(s,e1);
		e1_loop();
	}
	~std5_f_a_a_a_f_iter()
	{
		if (e2_input) {
			e3->unref();
			delete e2_input;
		}
		delete e1_output;
		e5->unref();
	}
	bool is_over()
	{
		return over;
	}
	sc_addr value(sc_uint num)
	{
		if (over || num>4)
			return 0;
		return regs[num];
	}
	sc_retval next()
	{
		if (over)
			return RV_ELSE_GEN;
		e2_input->next();
		if (e2_loop())
			return RV_OK;
		delete e2_input; e2_input = 0;
		e3->unref();
		e1_output->next();
		e1_loop();
		return over?RV_ELSE_GEN:RV_OK;
	}
#undef e1
#undef e2
#undef e3
#undef e4
#undef e5
};

sc_iterator *std5_f_a_a_a_f_factory(sc_session *s,sc_constraint *constr)
{
	return new std5_f_a_a_a_f_iter(s,
		constr->params[0].addr, // e1
		constr->params[4].addr, // e5
		constr->params[1].type, // t2
		constr->params[2].type, // t3
		constr->params[3].type // t4
	);
}

class std_on_segment_iter : public sc_iterator
{
public:
	std_on_segment_iter(sc_segment::iterator *_iter, bool _foreign, sc_session *_s, sc_segment *_seg)
		: iter(_iter), foreign(_foreign), s(_s->ref()), val(0), seg(_seg->ref())
	{
		next();
	}

	~std_on_segment_iter()
	{
		if (!is_over())
			val->unref();
		s->unref();
		delete iter;
		seg->unref();
	}

	sc_retval next()
	{
		if (val)
			val->unref();
		do {
			val = 0;
			if (iter->is_over())
				return RV_ELSE_GEN;
			sc_addr_ll addr = iter->next();
			if (!foreign && seg->is_link(addr))
				continue;
			val = seg->get_element_addr(addr);
		} while (!val || val->is_dead());
		assert(foreign || val->seg == seg);
		val->ref();
		return RV_OK;
	}

	sc_addr value(sc_uint num)
	{
		if (num)
			return 0;
		return val;
	}

	bool is_over() { return val == 0; }

private:
	sc_segment::iterator *iter;
	bool foreign;
	sc_session *s;
	sc_addr val;
	sc_segment *seg;
};

sc_iterator* std_on_segment_factory(sc_session *s, sc_constraint *constr)
{
	sc_segment *seg = constr->params[0].seg;
	if (s->is_segment_opened(seg)) {
		bool foreign = constr->params[1].flag;
		sc_segment::iterator *iter = seg->create_iterator_on_elements();
		return new std_on_segment_iter(iter, foreign, s, seg);
	} else {
		return 0;
	}
}
