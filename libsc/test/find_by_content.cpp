/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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

#define BOOST_TEST_MODULE test_sc_session

#include <boost/test/unit_test.hpp>

#include <libsc/libsc.h>
#include <libsc/pm_keynodes.h>
#include <libsc/segment_utils.h>
#include <libsc/sc_utils.h>
#include <libsc/stl_utils.h>

BOOST_AUTO_TEST_SUITE(testsuite_find_by_content)

struct fixture
{
	fixture()
	{
		libsc_init();

		s = libsc_login();
		seg = create_unique_segment(s, "/tmp/test");

		cont_str = Content::string("Hello!");
		cont_int = Content::integer(5);
		cont_real = Content::real(5.0);

		el1 = s->create_el(seg, SC_N_CONST);
		s->set_content(el1, cont_str);

		el2 = s->create_el(seg, SC_N_CONST);
		s->set_content(el2, cont_str);

		el3 = s->create_el(seg, SC_N_CONST);
		s->set_content(el3, cont_int);

		el4 = s->create_el(seg, SC_N_CONST);
		s->set_content(el4, cont_real);
	}

	~fixture()
	{
		s->unlink(seg->get_full_uri());
		s->close();

		libsc_deinit();
	}

	Content cont_str, cont_int, cont_real;

	sc_addr el1, el2, el3, el4;
	sc_session *s;
	sc_segment *seg;

	addr_list result;
};

BOOST_FIXTURE_TEST_CASE(test_find_string, fixture)
{
	s->find_by_content(cont_str, result);
	BOOST_CHECK_EQUAL(result.size(), 2);
	BOOST_CHECK(contains(result, el1));
	BOOST_CHECK(contains(result, el2));
}

BOOST_FIXTURE_TEST_CASE(test_find_int, fixture)
{
	s->find_by_content(cont_int, result);
	BOOST_CHECK_EQUAL(result.size(), 1);
	BOOST_CHECK(contains(result, el3));
}

BOOST_FIXTURE_TEST_CASE(test_find_real, fixture)
{
	s->find_by_content(cont_real, result);
	BOOST_CHECK_EQUAL(result.size(), 1);
	BOOST_CHECK(contains(result, el4));
}

BOOST_FIXTURE_TEST_CASE(test_find_string_with_erase, fixture)
{
	s->erase_el(el1);
	s->find_by_content(cont_str, result);
	BOOST_CHECK_EQUAL(result.size(), 1);
	BOOST_CHECK(contains(result, el2));
}

BOOST_FIXTURE_TEST_CASE(test_find_no_result, fixture)
{
	s->find_by_content(Content::string("Hello, dude!"), result);
	BOOST_CHECK(result.empty());
}

BOOST_AUTO_TEST_SUITE_END()
