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
 * @file main.cpp
 * @brief Tool for checking sc-segment equality.
 * @author Dmitry Lazurkin
 */

#include <libpm/pm.h>

#include <libsc/sc_utils.h>
#include <libsc/segment_utils.h>

#include <libsc/tgf_io.h>
#include <libpm/pm_sched.h>

#include <libpm/nsm/nsm.h>
#include <libpm/nsm/nsm_utils.h>
#include <libpm/nsm/nsm_pattern.h>
#include <libpm/nsm/nsm_keynodes.h>

#include <tclap/CmdLine.h>

#include <iostream>

static void sync_streams()
{
	// FIXME: Usage of std::cout/std::cerr.
	fflush(stdout);
	fflush(stderr);

	libsc_deinit();
}

sc_session *s;

static bool merge_els(sc_addr el1, sc_addr el2) 
{
	int out_qnt = s->get_out_qnt(el1);
	int in_qnt = s->get_in_qnt(el1);

	bool rv = true;

	if (out_qnt != s->get_out_qnt(el2)) {
		std::cout << "out count diff : ";
		std::cout << s->get_idtf(el1) << " ( " << s->get_out_qnt(el1) << " ) != " << s->get_idtf(el2) << " ( " << s->get_out_qnt(el2) << " )\n";
		rv = false;
	}
	
	if (in_qnt != s->get_in_qnt(el2)) {
		std::cout << "in count diff : ";
		std::cout << s->get_idtf(el1) << " ( " << s->get_in_qnt(el1) << " ) != " << s->get_idtf(el2) << " ( " << s->get_in_qnt(el2) << " )\n";
		rv = false;
	}

	sc_type t = s->get_type(el1);
	if (t != s->get_type(el2)) {
		std::cout << "type diff : ";
		std::cout << s->get_idtf(el1) << " ( " << s->get_type(el1) << " ) != " << s->get_idtf(el2) << " ( " << s->get_type(el2) << " )\n";
		rv = false;
	}

	Content content1 = s->get_content(el1);
	Content content2 = s->get_content(el2);
	
	if (content1 != content2) {
		std::cout << "content diff : ";
		std::cout << s->get_idtf(el1) << " ( " << s->get_content(el1) << " ) != " << s->get_idtf(el2) << " ( " << s->get_content(el2) << " )\n";
		rv = false;
	}
	
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_ALL_INPUT, el2), true);
	sc_for_each(it) s->set_end(it->value(1), el1);

	it = s->create_iterator(sc_constraint_new(CONSTR_ALL_OUTPUT, el2), true);
	sc_for_each(it) s->set_beg(it->value(1), el1);
	
	s->erase_el(el2);

	return rv;
}

static bool merge_els_from_segments(sc_segment *seg1, sc_segment *seg2)
{
	int count_el_1 = 0;
	int count_el_2 = 0;
	
	bool rv = true;

	addr_list seg1_els;
	sc_iterator *iter1 = system_session->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, seg1, false), true);
	sc_for_each(iter1) {
		count_el_1++;
		seg1_els.push_back(iter1->value(0));
	}

	addr_list::iterator seg1_it = seg1_els.begin();
	for (; seg1_it != seg1_els.end(); seg1_it++) {
		sc_addr el1 = *seg1_it;

		sc_string idtf1 = system_session->get_idtf(el1);
		if (!is_system_id(idtf1)) {
			sc_addr el2 = system_session->find_by_idtf(idtf1, seg2);

			if (!el2 || !merge_els(el1, el2)) {
				rv = false;
			}

			count_el_2++;
		}
	}

	sc_iterator *seg_it2 = system_session->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, seg2, false), true);
	sc_for_each(seg_it2) {
		count_el_2++;
		sc_addr el2 = seg_it2->value(0);
		if (!el2->is_dead()) {
			sc_string idtf2 = system_session->get_idtf(el2);
			if (!is_system_id(idtf2)) {
				rv = false;
			}
		}
	}

	if (count_el_1 != count_el_2) {
		rv = false;
	}

	return rv;
}

static sc_addr create_pattern_el(sc_addr pattern_sign, sc_segment *pattern_seg, sc_addr original_el)
{
	sc_string idtf = s->get_idtf(original_el);

	if (is_system_id(idtf)) {
		sc_type original_type = s->get_type(original_el);

		sc_type pattern_type;
		if (original_type & SC_CONST) {
			pattern_type = (original_type ^ SC_CONST) | SC_VAR;
		} else if (original_type & SC_VAR) {
			pattern_type = (original_type ^ SC_VAR) | SC_METAVAR;
		} else if (original_type & SC_METAVAR) {
			pattern_type = original_type;
		}

		sc_addr pattern_el = s->create_el(pattern_seg, pattern_type);
		s->gen3_f_a_f(pattern_sign, 0, pattern_seg, SC_A_CONST|SC_POS, pattern_el);
		return pattern_el;
	} else {
		s->gen3_f_a_f(pattern_sign, 0, pattern_seg, SC_A_CONST|SC_POS, original_el);
		return original_el;
	}
}

static sc_addr build_pattern(sc_segment *tmp_seg, sc_segment *seg1)
{
	addr2addr_map orig2pattern;
	
	sc_addr pattern_sign = s->create_el(tmp_seg, SC_N_CONST);

	sc_iterator *seg_it = s->create_iterator(sc_constraint_new(CONSTR_ON_SEGMENT, seg1, false), true);
	sc_for_each(seg_it) {
		sc_addr cur_el = seg_it->value(0);
		
		addr2addr_map::iterator it = orig2pattern.find(cur_el);
		if (it != orig2pattern.end())
			continue;
		
		sc_addr new_el = create_pattern_el(pattern_sign, tmp_seg, cur_el);
		orig2pattern.insert(addr2addr_map::value_type(cur_el, new_el));

		if (s->get_type(cur_el) & SC_ARC_MAIN) {
			{
				sc_addr new_beg = 0;
				addr2addr_map::iterator it = orig2pattern.find(s->get_beg(cur_el));
				if (it != orig2pattern.end()) {
					new_beg = it->second;
				} else {
					new_beg = create_pattern_el(pattern_sign, tmp_seg, s->get_beg(cur_el));
					orig2pattern.insert(addr2addr_map::value_type(s->get_beg(cur_el), new_beg));
				}
				s->set_beg(new_el, new_beg);
			}    

			{
				sc_addr new_end = 0;
				addr2addr_map::iterator it = orig2pattern.find(s->get_end(cur_el));
				if (it != orig2pattern.end()) {
					new_end = it->second;
				} else {
					new_end = create_pattern_el(pattern_sign, tmp_seg, s->get_end(cur_el));
					orig2pattern.insert(addr2addr_map::value_type(s->get_end(cur_el), new_end));
				}
				s->set_end(new_el, new_end);
			}
		}
	}

	return pattern_sign;
}

static bool check_equal_with_nsm(sc_segment *seg1, sc_segment *seg2) 
{
	sc_segment *tmp_seg = create_unique_segment(s, "/pattern_segment");
	sc_addr pattern_sign = build_pattern(tmp_seg, seg1);
	
	nsm_pattern seg_pattern(s, pattern_sign, tmp_seg);
	sc_addr arc_goal = seg_pattern.search_async();
	sc_addr results = 0;

	while (search_3_f_cpa_f(s, NSM_SEARCHED_, 0, arc_goal) != RV_OK) {
		if (!pm_sched_do_step()) {
			std::cerr << "Error while scheduling\n";
			exit(10);
		}
	}
	
	search_3l2_f_cpa_cna_cpa_f(s, NSM_RESULT, 0, &results, 0, arc_goal);

	if (!results)
		return false;

	return s->get_out_qnt(results) == 2;
}

static bool check_equal(sc_segment *seg1, sc_segment *seg2)
{
	if (merge_els_from_segments(seg1, seg2)) {
		if (check_equal_with_nsm(seg1, seg2))
			return true;
	}

	return false;
}

TCLAP::UnlabeledValueArg<std::string> arg_first_seg("first", "First TGF file for checking equality", true, "", "path to TGF file");
TCLAP::UnlabeledValueArg<std::string> arg_second_seg("second", "Second TGF file for checking equality", true, "", "path to TGF file");

int main(int argc, char **argv)
{
	atexit(sync_streams);

	setlocale(LC_ALL,".1251");

	try {
		TCLAP::CmdLine cmd("Tool for checking segment equality", ' ', "0.1");

		cmd.add(arg_first_seg);
		cmd.add(arg_second_seg);

		cmd.parse(argc, argv);
	} catch (TCLAP::ArgException &e) {
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	s = libsc_init(0);

	s->mkdir("/tmp");
	s->mkdir("/proc");
	s->mkdir("/proc/agents");
	
	pm_keynodes_init(s);
	pm_sched_init(s, true);
	nsm_pre_init();
	nsm_init();

	sc_string segfile1 = arg_first_seg.getValue();
	sc_string segfile2 = arg_second_seg.getValue();

	sc_segment *seg1 = do_read_tgf(segfile1);
	sc_segment *seg2 = do_read_tgf(segfile2);

	if (check_equal(seg1, seg2)) {
		std::cout << "Equals\n";
		return 0;
	} else {
		std::cout << "Not equals\n";
		return 1;
	}
}




