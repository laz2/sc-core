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
 * @file rgp_server.cpp
 * @brief Implementation of RGP-server. Uses boost::asio
 * @author Dmitry Lazurkin
 */

#include "precompiled_p.h"

#include "rgp_connection_p.h"
#include "rgp_server.h"

using namespace RGP;

using boost::asio::ip::tcp;

sc_segment *rgp_segment = 0;
sc_addr     server_sign = 0;
sc_session *rgp_session = 0;

class rgp_server_activity : public sc_activity {
public:
	explicit rgp_server_activity(int control_port=47805) :
		io_service(new ba::io_service),
		control_acceptor(*io_service, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), control_port)),
		event_acceptor(*io_service, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), control_port + 1)) {
		start_accept();
	}

	~rgp_server_activity() {
		done();
	}

	sc_retval init(sc_addr _this) {
		pm_sched_add_agent(_this, SCHED_CLASS_KERNEL);
		return RV_OK;
	}

	void done() {
		pm_sched_remove_agent(server_sign);
		delete this;
		rgp_session->unlink(rgp_segment->get_full_uri());
	}

	sc_retval activate(sc_session *_s, sc_addr _this, sc_addr __p1, sc_addr __p2, sc_addr __p3) {
		try {
			//
			// In Processing Module we have own scheduler then use poll_one
			// which haven't event processing loop.
			// Per scheduler quant execute only one handler.
			//
			io_service->poll_one();
			return RV_OK;
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return RV_ERR_GEN;
		}
	}

private:
	/// Start connection accepting in async mode
	void start_accept() {
		// create new connection
		rgp_connection *new_connection = new rgp_connection(*io_service);
		// start acceptor in async mode

		control_acceptor.async_accept(new_connection->control_socket(),
			boost::bind(&rgp_server_activity::handle_control_accept, this, new_connection, ba::placeholders::error));

	}

	/**
	 * @brief Run when new connection is accepted
	 * @param new_connection accepted connection
	 * @param error reference to error object
	 */
	void handle_control_accept(rgp_connection *new_connection, const bs::error_code& error)  {
		if (!error) {
			event_acceptor.async_accept(new_connection->event_socket(),
				boost::bind(&rgp_server_activity::handle_event_accept, this, new_connection, ba::placeholders::error));
		}
	}

	void handle_event_accept(rgp_connection *new_connection, const bs::error_code& error) {
		if (!error) {
			new_connection->start();
			start_accept();
		}
	}

	ba::io_service *io_service;
	ba::ip::tcp::acceptor control_acceptor; /// object, that accepts new control connections
	ba::ip::tcp::acceptor event_acceptor;   /// object, that accepts new event connections
};

#if 0
#include <boost/logging/format/named_write.hpp>

BOOST_DEFINE_LOG_FILTER(g_l_level, filter_type)
BOOST_DEFINE_LOG(g_l, logger_type)
#endif

void init_rgp_logs() {
	//g_l()->writer().write("[RGP] |\n", "cout file(out.txt)");
	//g_l()->mark_as_initialized();
	//g_l_level()->set_enabled(boost::logging::level::warning);
}

#define DEFINE_CONSTR(name) \
	rgp_constraint_infos[RGP_ ## name].sc_id = name;

sc_retval init_rgp_server(sc_session *init_session, int port) {
	DEFINE_CONSTR(CONSTR_ALL_INPUT);
	DEFINE_CONSTR(CONSTR_ALL_OUTPUT);
	DEFINE_CONSTR(CONSTR_3_f_a_a);
	DEFINE_CONSTR(CONSTR_3_f_a_f);
	DEFINE_CONSTR(CONSTR_3_a_a_f);
	DEFINE_CONSTR(CONSTR_5_f_a_a_a_a);
	DEFINE_CONSTR(CONSTR_5_f_a_a_a_f);
	DEFINE_CONSTR(CONSTR_5_f_a_f_a_a);
	DEFINE_CONSTR(CONSTR_5_f_a_f_a_f);
	DEFINE_CONSTR(CONSTR_5_a_a_a_a_f);
	DEFINE_CONSTR(CONSTR_5_a_a_f_a_a);
	DEFINE_CONSTR(CONSTR_5_a_a_f_a_f);
	DEFINE_CONSTR(CONSTR_3l2_f_a_a_a_f);
	DEFINE_CONSTR(CONSTR_ON_SEGMENT);
	DEFINE_CONSTR(CONSTR_ORD_BIN_CONN1);
	DEFINE_CONSTR(CONSTR_ORD_BIN_CONN2);

	init_rgp_logs();

	rgp_session = init_session;

	rgp_segment = rgp_session->create_segment("/tmp/rgp");
	server_sign = rgp_session->create_el(rgp_segment, SC_N_CONST);
	sc_activity *a = new rgp_server_activity(port);

	if (rgp_session->reimplement(server_sign, a)) {
		std::cerr << "Warning: cannot set listen activity" << std::endl;
		return RV_ERR_GEN;
	}

	return RV_OK;
}
