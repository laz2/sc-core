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
 * @file rgp_connection_p.h
 * @brief Functionality which serve RGP-client
 * @author Dmitry Lazurkin
 * @ingroup librgp
 */

#ifndef __LIBRGP_RGP_CONNECTION_H_INCLUDED__
#define __LIBRGP_RGP_CONNECTION_H_INCLUDED__

/// @addtogroup librgp
/// @{

#include "rgp_objs_registry_p.h"
#include "rgp_command_p.h"

namespace RGP {

namespace ba=boost::asio;
namespace bs=boost::system;

#define RGP_SC_ITERATOR_VALUE_SEND_COUNT 10

class rgp_connection {
public:
	/// Initialize connection
	rgp_connection(boost::asio::io_service &io_service);

	~rgp_connection();

	/** 
	 * @brief Return socket, associated with this connection. This socket used in accept operation
	 * @return reference to socket
	 */
	ba::ip::tcp::socket& control_socket() {
		return control_socket_;
	}

	ba::ip::tcp::socket& event_socket() { return event_socket_; }

	rgp_objs_registry& registry() { return registry_; }

	/// Start input/output chain with reading RGP command_id
	void start();

	void start_async_reading();

	void handle_data_readed(const bs::error_code &error, size_t bytes_transferred);

	void executing_response();

	sc_activity* new_proxy_activity();

	sc_wait* new_proxy_wait();

	void close();

	bool is_closed() const { return closed; }

	void set_system(bool is_system) { is_system_ = is_system; }

	bool is_system() const { return is_system_; }

	sc_session*& work_session() { return work_session_; }

	sc_retval event_communication();

private:
	ba::ip::tcp::socket control_socket_;     ///< socket, associated with RGP-connection
	ba::ip::tcp::socket event_socket_;

	ba::streambuf       request_buf; ///< buffer for request data

	sc_session *work_session_;
	bool is_system_;

	bool closed;

	rgp_objs_registry registry_;
	rgp_command *request_;
};

} // namespace RGP

/// @}

#endif // __LIBRGP_RGP_CONNECTION_H_INCLUDED__
