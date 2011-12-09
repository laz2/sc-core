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
 * @file rgp_connection.cpp
 * @brief Implementation of client socket RGP-connection. Uses boost::asio
 * @author Dmitry Lazurkin
 * @ingroup librgp
 */

/// @addtogroup librgp
/// @{

#include "precompiled_p.h"

#include "rgp_connection_p.h"

using namespace RGP;
using boost::asio::ip::tcp;

#define RGP_CMD_HANDLER_NAME(command_id) handler_ ## command_id

#define RGP_DEFINE_CMD_HANDLER(command_id) \
	void RGP_CMD_HANDLER_NAME(command_id)(rgp_connection &connection, sc_session *&work_session, const rgp_command &request, rgp_command &reply)

RGP_DEFINE_CMD_HANDLER(RGP_REQ_LOGIN) {
	connection.set_system(request.args()[0]->value<bool>());

	if (connection.is_system()) {
		work_session = system_session;
	} else {
		work_session = libsc_login();
	}

	reply << ((work_session) ? RV_OK : RV_ERR_GEN);
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_CLOSE) {
	reply << RV_OK;
	connection.close();
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_CREATE_SEGMENT) {
	sc_string uri = request.args()[0]->value<sc_string>();
	sc_segment *seg = work_session->create_segment(uri);

	if (seg) {
		reply << RV_OK << seg << seg2sign(work_session, seg);
	} else {
		reply << work_session->get_error();
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_OPEN_SEGMENT) {
	sc_string uri = request.args()[0]->value<sc_string>();
	sc_segment *seg = work_session->open_segment(uri);

	if (seg) {
		reply << RV_OK << seg << seg2sign(work_session, seg);
	} else {
		reply << work_session->get_error();
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_IDTF) {
	reply << RV_OK << work_session->get_idtf(request.args()[0]->value_not_nil<sc_addr>());
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_SET_IDTF) {
	reply << work_session->set_idtf(
		request.args()[0]->value_not_nil<sc_addr>(),
		request.args()[1]->value<sc_string>()
	);
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_ERASE_IDTF) {
	reply << work_session->erase_idtf(request.args()[0]->value_not_nil<sc_addr>());
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_FIND_BY_IDTF) {
	sc_string idtf = request.args()[0]->value<sc_string>();
	sc_segment *seg = request.args()[1]->value_not_nil<sc_segment *>();

	sc_addr addr = work_session->find_by_idtf(idtf, seg);

	if (addr) {
		reply << RV_OK << addr;
	} else {
		reply << RV_ELSE_GEN;
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_CREATE_EL) {
	sc_segment *seg = request.args()[0]->value_not_nil<sc_segment *>();
	sc_type type = request.args()[1]->value<sc_type>();

	sc_addr el = work_session->create_el(seg, type);

	if (el) {
		reply << RV_OK << el;
	} else {
		reply << work_session->get_error();
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GEN3_F_A_F) {
	sc_addr arc2 = 0;
	sc_retval rv = work_session->gen3_f_a_f(
		request.args()[0]->value_not_nil<sc_addr>(),
		&arc2,
		request.args()[1]->value_not_nil<sc_segment *>(),
		request.args()[2]->value<sc_type>(),
		request.args()[3]->value_not_nil<sc_addr>()
	);

	reply << rv;

	if (rv == RV_OK)
		reply << arc2;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GEN5_F_A_F_A_F) {
	sc_addr arc2 = 0;
	sc_addr arc4 = 0;
	sc_retval rv = work_session->gen5_f_a_f_a_f(
		request.args()[0]->value_not_nil<sc_addr>(),
		&arc2,
		request.args()[1]->value_not_nil<sc_segment *>(),
		request.args()[2]->value<sc_type>(),
		request.args()[3]->value_not_nil<sc_addr>(),
		&arc4,
		request.args()[4]->value_not_nil<sc_segment *>(),
		request.args()[5]->value<sc_type>(),
		request.args()[6]->value_not_nil<sc_addr>()
	);

	reply << rv;

	if (rv == RV_OK)
		reply << arc2 << arc4;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_ERASE_EL) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	work_session->erase_el(el);
	reply << work_session->get_error();
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_MERGE_EL) {
	sc_addr main  = request.args()[0]->value_not_nil<sc_addr>();
	sc_addr other = request.args()[1]->value_not_nil<sc_addr>();
	reply << sc_merge_elements(work_session, main, other);
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_SET_EL_BEGIN) {
	sc_addr el  = request.args()[0]->value_not_nil<sc_addr>();
	sc_addr beg = request.args()[1]->value_not_nil<sc_addr>();
	sc_retval rv = work_session->set_beg(el, beg);
	reply << rv;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_EL_BEGIN) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	sc_addr beg = work_session->get_beg(el);

	if (beg) {
		reply << RV_OK << beg;
	} else {
		reply << RV_ERR_GEN;
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_SET_EL_END) {
	sc_addr el  = request.args()[0]->value_not_nil<sc_addr>();
	sc_addr end = request.args()[1]->value_not_nil<sc_addr>();

	sc_retval rv = work_session->set_end(el, end);
	reply << rv;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_EL_END) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	sc_addr end = work_session->get_end(el);

	if (end) {
		reply << RV_OK << end;
	} else {
		reply << RV_ERR_GEN;
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_EL_TYPE) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	sc_type type = work_session->get_type(el);
	reply << RV_OK << type;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_CHANGE_EL_TYPE) {
	sc_addr el   = request.args()[0]->value_not_nil<sc_addr>();
	sc_type type = request.args()[1]->value<sc_type>();
	sc_retval rv = work_session->change_type(el, type);
	reply << rv;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_EL_CONTENT) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	const Content* content = work_session->get_content_const(el);
	reply << RV_OK << content;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_SET_EL_CONTENT) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	const Content *c = request.args()[1]->value_not_nil<Content *>();
	reply << work_session->set_content(el, *c);
}

/// Encode in RGP-command @p reply sc-iterator values.
/// Encode only values with indexes from @p need_values.
static
void encode_sc_iterator_values(sc_iterator *it, rgp_command &reply, std::vector<sc_uint> &need_values) {
	size_t sended = 0;
	while (sended < RGP_SC_ITERATOR_VALUE_SEND_COUNT && !it->is_over()) {
		for (size_t i = 0; i < need_values.size(); ++i)
			reply << it->value(need_values[i]);
		it->next();
		++sended;
	}
	reply << it->is_over();
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_CREATE_ITERATOR) {
	rgp_constraint_t constr_type = request.args()[0]->value<rgp_constraint_t>();

	sc_constraint_info *info = sc_constraint_get_info(rgp_constraint_infos[constr_type].sc_id);
	sc_constraint *constr = __sc_constraint_new(info);

	std::vector<sc_uint> need_values;

	for (int i = 0; i < info->input_cnt; ++i) {
		rgp_command_arg *arg = request.args()[i + 1];
		switch (info->input_spec[i]) {
		case SC_PD_TYPE:
			need_values.push_back(i);
			constr->params[i].type = arg->value<sc_type>();
			break;
		case SC_PD_ADDR:
		case SC_PD_ADDR_0:
			constr->params[i].addr = arg->value_not_nil<sc_addr>();
			break;
		case SC_PD_INT:
			constr->params[i].i = arg->value<sc_int>();
			break;
		case SC_PD_SEGMENT:
			constr->params[i].seg = arg->value_not_nil<sc_segment *>();
			break;
		case SC_PD_BOOLEAN:
			constr->params[i].flag = arg->value<bool>();
			break;
		default:
			sc_constraint_free(constr);
			reply << RV_ERR_GEN;
			return;
		}
	}

	sc_iterator *it = work_session->create_iterator(constr, true);

	if (constr_type == RGP_CONSTR_ON_SEGMENT)
		need_values.push_back(0);

	reply << RV_OK << it;
	encode_sc_iterator_values(it, reply, need_values);
}

void fill_need_sc_iterator_values(const rgp_command &request, std::vector<sc_uint> &need_values) {
	for (size_t i = 1; i < request.args_count(); ++i)
		need_values.push_back(request.args()[i]->value<sc_int>());
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_NEXT_ITERATOR) {
	sc_iterator *it = request.args()[0]->value_not_nil<sc_iterator *>();

	sc_retval rv = it->next();
	reply << rv;
	
	if (rv == RV_OK) {
		std::vector<sc_uint> need_values;
		fill_need_sc_iterator_values(request, need_values);
		encode_sc_iterator_values(it, reply, need_values);
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_ERASE_ITERATOR) {
	sc_iterator *it = request.args()[0]->value_not_nil<sc_iterator *>();
	connection.registry().unregister(it);
	delete it;
	reply << RV_OK;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_REIMPLEMENT) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	sc_activity *activity = connection.new_proxy_activity();
	sc_retval rv = work_session->reimplement(el, activity);
	if (rv == RV_OK) {
		reply << rv << activity;
	} else {
		delete activity;
		reply << rv;
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_ATTACH_WAIT) {
	sc_wait_type wait_type = request.args()[0]->value<sc_wait_type>();
	size_t prms_cnt = request.args_count() - 1;
	boost::scoped_array<sc_param> prms(new sc_param[prms_cnt]);

	for (size_t i = 0; i < prms_cnt; ++i) {
		rgp_command_arg *arg = request.args()[i + 1];
		switch (arg->type()) {
		case RGP_ARG_SC_ADDR:
			prms[i].addr = arg->value_not_nil<sc_addr>();
			break;
		case RGP_ARG_SC_SEGMENT:
			prms[i].seg = arg->value_not_nil<sc_segment *>();
			break;
		case RGP_ARG_SC_TYPE:
			prms[i].type = arg->value<sc_type>();
			break;
		default:
			reply << RV_ERR_GEN;
			return;
		}
	}

	sc_wait *wait = connection.new_proxy_wait();
	sc_retval rv = work_session->attach_wait(wait_type, prms.get(), prms_cnt, wait);
	if (rv == RV_OK) {
		reply << rv << wait;
	} else {
		delete wait;
		reply << rv;
	}
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_DETACH_WAIT) {
	sc_wait *wait = request.args()[0]->value_not_nil<sc_wait *>();

	connection.registry().unregister(wait);
	delete wait;

	reply << RV_OK;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_ACTIVATE) {
	sc_addr _this = request.args()[0]->value_not_nil<sc_addr>();
	
	sc_addr prms[3];
	std::fill_n(prms, 3, SCADDR_NIL);
	for (unsigned i = 1; i < request.args_count(); ++i)
		prms[i - 1] = request.args()[i]->value<sc_addr>();

	sc_retval rv = work_session->activate(_this, prms[0], prms[1], prms[2]);
	reply << rv;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_SEG_INFO) {
	sc_segment *seg = request.args()[0]->value_not_nil<sc_segment *>();
	reply << RV_OK << seg->sign << seg->get_full_uri() << seg->dead;
}

RGP_DEFINE_CMD_HANDLER(RGP_REQ_GET_EL_INFO) {
	sc_addr el = request.args()[0]->value_not_nil<sc_addr>();
	reply << RV_OK << el->seg << el->dead;
}

#define RGP_DISPATCH_ENTRY(command_id) \
	{ command_id, &RGP_CMD_HANDLER_NAME(command_id) }

typedef void (*request_dispatch_handler)(rgp_connection &connection, sc_session *&work_session, const rgp_command &, rgp_command &);

static struct request_dispatch_entry {
	rgp_command_t id;
	request_dispatch_handler handler;
} request_dispatch_table[] = {
	RGP_DISPATCH_ENTRY(RGP_REQ_LOGIN),
	RGP_DISPATCH_ENTRY(RGP_REQ_CLOSE),

	RGP_DISPATCH_ENTRY(RGP_REQ_CREATE_SEGMENT),
	RGP_DISPATCH_ENTRY(RGP_REQ_OPEN_SEGMENT),

	RGP_DISPATCH_ENTRY(RGP_REQ_GET_IDTF),
	RGP_DISPATCH_ENTRY(RGP_REQ_SET_IDTF),
	RGP_DISPATCH_ENTRY(RGP_REQ_ERASE_IDTF),
	RGP_DISPATCH_ENTRY(RGP_REQ_FIND_BY_IDTF),

	RGP_DISPATCH_ENTRY(RGP_REQ_CREATE_EL),
	RGP_DISPATCH_ENTRY(RGP_REQ_GEN3_F_A_F),
	RGP_DISPATCH_ENTRY(RGP_REQ_GEN5_F_A_F_A_F),
	RGP_DISPATCH_ENTRY(RGP_REQ_ERASE_EL),

	RGP_DISPATCH_ENTRY(RGP_REQ_MERGE_EL),

	RGP_DISPATCH_ENTRY(RGP_REQ_SET_EL_BEGIN),
	RGP_DISPATCH_ENTRY(RGP_REQ_SET_EL_END),

	RGP_DISPATCH_ENTRY(RGP_REQ_GET_EL_BEGIN),
	RGP_DISPATCH_ENTRY(RGP_REQ_GET_EL_END),

	RGP_DISPATCH_ENTRY(RGP_REQ_GET_EL_TYPE),
	RGP_DISPATCH_ENTRY(RGP_REQ_CHANGE_EL_TYPE),

	RGP_DISPATCH_ENTRY(RGP_REQ_GET_EL_CONTENT),
	RGP_DISPATCH_ENTRY(RGP_REQ_SET_EL_CONTENT),

	RGP_DISPATCH_ENTRY(RGP_REQ_CREATE_ITERATOR),
	RGP_DISPATCH_ENTRY(RGP_REQ_NEXT_ITERATOR),
	RGP_DISPATCH_ENTRY(RGP_REQ_ERASE_ITERATOR),

	RGP_DISPATCH_ENTRY(RGP_REQ_ATTACH_WAIT),
	RGP_DISPATCH_ENTRY(RGP_REQ_DETACH_WAIT),
	
	RGP_DISPATCH_ENTRY(RGP_REQ_REIMPLEMENT),
	RGP_DISPATCH_ENTRY(RGP_REQ_ACTIVATE),

	RGP_DISPATCH_ENTRY(RGP_REQ_GET_SEG_INFO),
	RGP_DISPATCH_ENTRY(RGP_REQ_GET_EL_INFO)
};

/// Read in sync mode RGP-command @p command.
void read_rgp_command(const rgp_objs_registry &registry, ba::ip::tcp::socket &socket, rgp_command &command) {
	ba::streambuf reply_buf;
	size_t available_bytes = 0;

	while (!command.is_readed()) {
		ba::streambuf::mutable_buffers_type bufs = reply_buf.prepare(10);
		size_t bytes_transferred = socket.read_some(bufs);

		reply_buf.commit(bytes_transferred);
		available_bytes += bytes_transferred;

		std::istreambuf_iterator<char> request_it(&reply_buf);
		command.read(registry, request_it, available_bytes);
		reply_buf.consume(reply_buf.in_avail() - available_bytes);
	}
}

/// Proxy wait thats send activate request to client.
class rgp_proxy_wait : public sc_wait {
public:
	rgp_proxy_wait(rgp_connection &c) : connection(c) {}

	~rgp_proxy_wait() {
		connection.work_session()->detach_wait(this);
	}

	virtual bool activate(sc_wait_type type, sc_param *prms, int len) {
		//
		// Send RGP_REQ_ACTIVATE_WAIT to client
		//

		rgp_command request(RGP_REQ_ACTIVATE_WAIT);

		request << this << type;
		for (int i = 0; i < len; ++i)
			request << prms[i].addr;

		L_(debug) << "<--- " << request;
		request.write(connection.registry(), connection.event_socket());

		try {
			//
			// Receive RGP_REP_RETURN from client
			//

			rgp_command reply;
			read_rgp_command(connection.registry(), connection.event_socket(), reply);

			L_(debug) << "---> " << reply;

			if (reply.id() != RGP_REP_RETURN || reply.args()[0]->value<sc_retval>() != RV_OK) {
				return true;
			} else {
				return reply.args()[1]->value<bool>();
			}
		} catch (rgp_error &e) {
			L_(warning) << "Error occured: " << e.what();
			return true;
		}
	}

private:
	rgp_connection &connection;
};

/// Proxy activity thats send activate request to client.
class rgp_proxy_activity : public sc_activity {
public:
	rgp_proxy_activity(rgp_connection &c) : connection(c),
		destructor_called(false), this_(0) {}

	~rgp_proxy_activity() {
		destructor_called = true;
		connection.work_session()->reimplement(this_, 0);
	}

	virtual sc_retval init(sc_addr _this) {
		this_ = _this;
		return RV_OK;
	}

	virtual void done() {
		connection.registry().unregister(this);
		if (!destructor_called)
			delete this;
	}

	virtual sc_retval activate(sc_session*, sc_addr _this, sc_addr prm1, sc_addr prm2, sc_addr prm3) {
		//
		// Send RGP_REQ_ACTIVATE to client
		//

		rgp_command request(RGP_REQ_ACTIVATE);

		request << this << _this << prm1 << prm2 << prm3;

		L_(debug) << "[E] <--- " << request;

		request.write(connection.registry(), connection.event_socket());

		return connection.event_communication();
	}

private:
	bool destructor_called;
	rgp_connection &connection;
	sc_addr this_;
};

sc_retval rgp_connection::event_communication()
{
	try {
		//
		// Receive RGP_REP_RETURN from client
		//

		while (true) {
			rgp_command request_or_reply;
			read_rgp_command(registry_, event_socket_, request_or_reply);

			L_(debug) << "[E] ---> " << request_or_reply;

			if (request_or_reply.id() == RGP_REP_RETURN) {
				return request_or_reply.args()[0]->value<sc_retval>();
			} else {
				rgp_command reply(RGP_REP_RETURN);

				L_(debug) << "[E] Executing handler for " << rgp_command_infos[request_dispatch_table[request_or_reply.id()].id].name;

				request_dispatch_table[request_or_reply.id()].handler(
					*this, work_session_, request_or_reply, reply);

				L_(debug) << "[E] <--- " << reply;

				reply.write(registry_, event_socket_);
			}

			if (closed) {
				L_(debug) << "[E] Closed from " << event_socket_.remote_endpoint();
				delete this;
				return RV_OK;
			}
		}
	} catch (rgp_error &e) {
		L_(warning) << "[E] Error occured: " << e.what();
		return e.code();
	}
}

sc_activity* rgp_connection::new_proxy_activity() { return new rgp_proxy_activity(*this); }

sc_wait* rgp_connection::new_proxy_wait() { return new rgp_proxy_wait(*this); }

rgp_connection::rgp_connection(boost::asio::io_service &io_service) :
	control_socket_(io_service), event_socket_(io_service),
	request_(NULL), closed(false), is_system_(false) {
	}

template< typename T >
void delete_objs_from_registry(rgp_obj_registry<T>& registry) {
	typedef typename rgp_obj_registry<T>::obj2uid_map::value_type pair_t;
	typedef std::list<T> tmp_t;
	tmp_t tmp;
	std::transform(registry.begin(), registry.end(), std::back_inserter(tmp),
		boost::bind(&pair_t::first, boost::lambda::_1));
	typename tmp_t::const_iterator it = tmp.begin();
	for (; it != tmp.end(); ++it)
		delete *it;
}

rgp_connection::~rgp_connection() {
	control_socket_.close();
	event_socket_.close();

	delete_objs_from_registry(registry_.get_sc_iterators_registry());
	delete_objs_from_registry(registry_.get_sc_waits_registry());
	delete_objs_from_registry(registry_.get_sc_activities_registry());

	if (!is_system_)
		work_session_->close();
}

void rgp_connection::start() {
	L_(debug) << "[C] Connection from " << control_socket_.remote_endpoint();
	start_async_reading();
}

void rgp_connection::start_async_reading() {
	ba::async_read(control_socket_, request_buf,
		ba::transfer_at_least(1),
		boost::bind(&rgp_connection::handle_data_readed,
		this,
		ba::placeholders::error,
		ba::placeholders::bytes_transferred));
}

void rgp_connection::handle_data_readed(const bs::error_code& error,
										size_t bytes_transferred) {
	if (bytes_transferred != 0) { // FIXME: why boost::asio::transfer_at_least not working?
		//L_(debug) << "Bytes read " << bytes_transferred;
		try {
			if (request_ == NULL)
				request_ = new rgp_command();

			if (!request_->is_readed()) {
				size_t available_bytes = request_buf.in_avail();
				std::istreambuf_iterator<char> request_it(&request_buf);
				request_->read(registry_, request_it, available_bytes);
				request_buf.consume(request_buf.in_avail() - available_bytes);
			}

			if (request_->is_readed()) {
				L_(debug) << "[C] ---> " << *request_;
				executing_response();
			}
		} catch (rgp_error &e) {
			L_(warning) << "Error occurred: " << e.what();
			rgp_command error_reply(RGP_REP_RETURN);
			error_reply << e.code();
			L_(debug) << "[C] <--- " << error_reply;
			error_reply.write(registry_, control_socket_);
			request_buf.consume(request_buf.in_avail());
		}

		if (!closed) {
			start_async_reading();
		} else {
			L_(debug) << "[C] Closed from " << control_socket_.remote_endpoint();
			delete this;
		}
	}
}

void rgp_connection::executing_response() {
	rgp_command reply(RGP_REP_RETURN);
	L_(debug) << "[C] Executing handler for " << rgp_command_infos[request_->id()].name;
	request_dispatch_table[request_->id()].handler(*this, work_session_, *request_, reply);
	L_(debug) << "[C] <--- " << reply;
	reply.write(registry_, control_socket_);
	delete request_;
	request_ = NULL;
}

void rgp_connection::close() {
	closed = true;
}

/// @}
