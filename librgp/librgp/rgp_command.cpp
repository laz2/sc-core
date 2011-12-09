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
 * @file rgp_command.h
 * @brief Implementation of RGP-command class and RGP-command arguments.
 * @author Dmitry Lazurkin
 * @ingroup librgp
 */

/// @addtogroup librgp
/// @{

#include "precompiled_p.h"

#include "rgp_command_p.h"

using namespace RGP;

#if defined(_MSC_VER)
// disable forcing value to bool
#pragma warning(disable:4800)
#endif

template< typename Type, rgp_command_arg_t ArgType, typename Size >
class int_command_arg : public rgp_command_arg {
public:
	int_command_arg() : rgp_command_arg(ArgType) {}

	virtual ~int_command_arg() {}

	virtual boost::any value() const { return value_; }

	virtual void set_value(const boost::any &v) { value_ = boost::any_cast<Type>(v); }

	virtual std::string to_string() const {
		std::ostringstream out;
		out << value_;
		return out.str();
	}

	virtual rgp_command_arg* clone() { return new int_command_arg(); }

	virtual size_t read(const rgp_objs_registry &registry, std::istreambuf_iterator< char > &in_it,
		size_t &available_bytes) throw (rgp_error) {
			size_t bytes_demand = sizeof(Size);

			if (available_bytes < bytes_demand)
				return bytes_demand;

			available_bytes -= bytes_demand;

			value_ = static_cast<Type>(read_int<Size>(in_it));

			return 0;
	}

	virtual void write(rgp_objs_registry &registry, std::ostreambuf_iterator< char > &out_it) throw (rgp_error) {
		write_int<Size>(value_, out_it);
	}

protected:
	Type value_;
};

typedef int_command_arg<sc_short, RGP_ARG_SC_INT16, int16_t> sc_int16_command_arg;
typedef int_command_arg<sc_int, RGP_ARG_SC_INT32, int32_t>   sc_int32_command_arg;
typedef int_command_arg<bool, RGP_ARG_SC_BOOLEAN, uint8_t>   sc_boolean_command_arg;
typedef int_command_arg<sc_retval, RGP_ARG_SC_RETVAL, rgp_sc_retval_id> sc_retval_command_arg;
typedef int_command_arg<sc_wait_type, RGP_ARG_SC_WAIT_TYPE, rgp_sc_wait_type_id> sc_wait_type_command_arg;

template< typename ObjectType, rgp_command_arg_t ArgType,
		  ObjectType (rgp_objs_registry::*GetObj)(rgp_object_id) const>
class object_command_arg : public rgp_command_arg {
public:
	object_command_arg() : rgp_command_arg(ArgType) {}

	virtual boost::any value() const { return value_; }

	virtual bool is_nil() const { return value_ == 0; }

	virtual void set_value(const boost::any &v) { value_ = boost::any_cast<ObjectType>(v); }

	virtual rgp_command_arg* clone() { return new object_command_arg(); }

	virtual std::string to_string() const {
		std::ostringstream out;
		if (value_ == 0) {
			out << "NIL";
		} else {
			out << value_;
		}
		return out.str();
	}

	virtual size_t read(const rgp_objs_registry &registry, std::istreambuf_iterator< char > &in_it,
		size_t &available_bytes) throw (rgp_error) {
			size_t bytes_demand = sizeof(uint32_t);

			if (available_bytes < bytes_demand)
				return bytes_demand;

			available_bytes -= bytes_demand;

			uint32_t id = read_uint32(in_it);
			value_ = (registry.*GetObj)(id);

			if (value_ == 0)
				L_(debug) << "Object of type " << rgp_command_arg_type_names[type()] << " not found for id " << id;

			return 0;
	}

	virtual void write(rgp_objs_registry &registry, std::ostreambuf_iterator< char > &out_it) throw (rgp_error) {
		write_uint32(registry.get_uid(value_), out_it);
	}

protected:
	ObjectType value_;
};

/// Command argument of RGP_ARG_SC_ITERATOR type.
typedef object_command_arg<sc_iterator *, RGP_ARG_SC_ITERATOR,
	&rgp_objs_registry::get_sc_iterator> sc_iterator_command_arg;
/// Command argument of RGP_ARG_SC_ACTIVITY type.
typedef object_command_arg<sc_activity *, RGP_ARG_SC_ACTIVITY,
	&rgp_objs_registry::get_sc_activity> sc_activity_command_arg;
/// Command argument of RGP_ARG_SC_WAIT type.
typedef object_command_arg<sc_wait *, RGP_ARG_SC_WAIT,
	&rgp_objs_registry::get_sc_wait> sc_wait_command_arg;

/// Command argument of RGP_ARG_SC_SEGMENT type.
class sc_segment_command_arg : public object_command_arg<sc_segment *, RGP_ARG_SC_SEGMENT,
	&rgp_objs_registry::get_sc_segment > {
public:
	virtual bool is_nil() const { 
		return value_ == 0 || value_->dead;
	}

	virtual std::string to_string() const {
		if (value_ == 0) {
			return "NIL";
		} else if (value_->dead) {
			return "DEAD";
		} else {
			return value_->get_full_uri();
		}
	}

	virtual rgp_command_arg* clone() { return new sc_segment_command_arg(); }
};

/// Command argument of RGP_ARG_SC_ADDR type.
class sc_addr_command_arg : public object_command_arg<sc_addr, RGP_ARG_SC_ADDR,
	&rgp_objs_registry::get_sc_addr > {
public:
	virtual bool is_nil() const {
		return value_ == 0 || value_->is_dead();
	}

	virtual std::string to_string() const {
		if (value_ == 0) {
			return "NIL";
		} else if (value_->is_dead()) {
			return "DEAD";
		} else {
			return get_full_uri(system_session, value_);
		}
	}

	virtual rgp_command_arg* clone() { return new sc_addr_command_arg(); }
};

/// Command argument of RGP_ARG_SC_TYPE type.
class sc_type_command_arg : public int_command_arg<sc_type, RGP_ARG_SC_TYPE, rgp_sc_type_id> {
public:
	virtual std::string to_string() const { return get_type_info(value_); }

	virtual rgp_command_arg* clone() { return new sc_type_command_arg(); }
};

/// Command argument of RGP_ARG_SC_CONSTRAINT type.
class sc_constraint_command_arg : public int_command_arg<rgp_constraint_t,
	RGP_ARG_SC_CONSTRAINT, rgp_sc_constraint_id> {
public:
	virtual std::string to_string() const {
		if (value_ == RGP_CONSTR_ON_SEGMENT) {
			return "CONSTR_ON_SEGMENT";
		} else {
			return sc_constraint_get_info(rgp_constraint_infos[value_].sc_id)->name;
		}
	}

	virtual rgp_command_arg* clone() { return new sc_constraint_command_arg(); }
};

/// Command argument of RGP_ARG_SC_STRING type.
class sc_string_command_arg : public rgp_command_arg {
public:
	sc_string_command_arg() :
	  rgp_command_arg(RGP_ARG_SC_STRING),
	  is_size_read(false) {}

	virtual ~sc_string_command_arg() {}

	virtual boost::any value() const { return value_; }

	virtual void set_value(const boost::any &v) { value_ = boost::any_cast<sc_string>(v); }

	virtual std::string to_string() const { return value_; }

	virtual rgp_command_arg* clone() { return new sc_string_command_arg(); }

	virtual size_t read(const rgp_objs_registry &registry, std::istreambuf_iterator< char > &in_it,
		size_t &available_bytes) throw (rgp_error) {
		//
		// If size of string isn't read then read size at first.
		//
		if (!is_size_read) {
			size_t bytes_demand = sizeof(uint32_t);

			if (available_bytes < bytes_demand)
				return bytes_demand;

			available_bytes -= bytes_demand;

			// Read size value
			bytes_demand = read_uint32(in_it);

			value_.resize(bytes_demand);
			is_size_read = true;
			out_it = value_.begin();
		}

		//
		// If we here then size is read.
		// Read string data bytes as many as possible (check available_bytes).
		//

		while (out_it != value_.end() && available_bytes != 0) {
			*out_it++ = read_int8(in_it);
			--available_bytes;
		}

		return std::distance(out_it, value_.end());
	}

	virtual void write(rgp_objs_registry &registry, std::ostreambuf_iterator< char > &out_it) throw (rgp_error) {
		write_uint32(value_.size(), out_it);
		std::copy(value_.begin(), value_.end(), out_it);
	}

private:
	bool is_size_read;
	sc_string::iterator out_it;
	sc_string value_;
};

/// Command argument of RGP_ARG_SC_CONTENT type.
class sc_content_command_arg : public rgp_command_arg {
public:
	sc_content_command_arg() :
	  rgp_command_arg(RGP_ARG_SC_CONTENT),
	  data_arg(0),
	  content_type(TCEMPTY),
	  owner(true),
	  value_(0) {}

	virtual ~sc_content_command_arg() {
		if (data_arg)
			delete data_arg;
		if (owner && value_)
			delete value_;
	}

	virtual boost::any value() const { return value_; }

	virtual void set_value(const boost::any &v) {
		owner = false;
		value_ = boost::any_cast<Content *>(v);
	}

	virtual std::string to_string() const {
		assert(value_);
		std::ostringstream out;
		out << *value_;
		return out.str();
	}

	virtual rgp_command_arg* clone() { return new sc_content_command_arg(); }

	virtual size_t read(const rgp_objs_registry &registry, std::istreambuf_iterator<char> &in_it,
		size_t &available_bytes) throw (rgp_error) {
		size_t bytes_demand = 0;

		//
		// If type of content isn't read (value_ is NULL) then read type at first.
		//
		if (!value_) {
			bytes_demand = sizeof(uint16_t);

			if (available_bytes < bytes_demand)
				return bytes_demand;

			available_bytes -= bytes_demand;

			value_ = new Content();
			value_->set_type(static_cast<TCont>(read_uint16(in_it))); // TODO: check correct type
		}

		//
		// If we here then type is read
		// Read content data bytes as many as possible (check available_bytes)
		//

		switch (value_->type()) {
		case TCSTRING:
			if (data_arg == 0) data_arg = new sc_string_command_arg;
			bytes_demand = data_arg->read(registry, in_it, available_bytes);
			if (bytes_demand == 0)
				*value_ = data_arg->value<sc_string>();
			break;
		case TCINT:
			if (data_arg == 0) data_arg = new sc_int32_command_arg;
			bytes_demand = data_arg->read(registry, in_it, available_bytes);
			if (bytes_demand == 0)
				*value_ = Content::integer(data_arg->value_not_nil<sc_int>());
			break;
		case TCREAL:
			throw rgp_error(RV_ERR_GEN, "Not support REAL content yet");
			break;
		case TCEMPTY:
			return 0;
		case TCDATA:
			throw rgp_error(RV_ERR_GEN, "Not support DATA content yet");
			break;
		}

		return bytes_demand;
	}

	virtual void write(rgp_objs_registry &registry, std::ostreambuf_iterator< char > &out_it) throw (rgp_error) {
		if (value_->type() == TCREAL) {
			// TODO: add handling for real content
			write_uint16(TCINT, out_it);
		} else {
			write_uint16(value_->type(), out_it);
		}

		const Cont &cont = *value_;
		switch (value_->type()) {
		case TCSTRING:
			write_uint32(cont.d.size - 1, out_it);
			std::copy(cont.d.ptr, cont.d.ptr + cont.d.size - 1, out_it);
			break;
		case TCINT:
			write_int32(cont.i, out_it);
			break;
		case TCREAL:
			write_int32(static_cast<int32_t>(cont.r), out_it);
			break;
		case TCEMPTY:
			break;
		case TCDATA:
			write_uint32(cont.d.size, out_it);
			std::copy(cont.d.ptr, cont.d.ptr + cont.d.size, out_it);
			break;
		}
	}

private:
	TCont content_type;
	rgp_command_arg *data_arg;
	bool owner;
	Content *value_;
	const Content *const_value_;
};

rgp_command_arg *command_arg_prototypes[] = {
	// RGP_ARG_UNKNOWN
	NULL,
	// RGP_ARG_SC_TYPE
	new sc_type_command_arg,
	// RGP_ARG_SC_SEGMENT
	new sc_segment_command_arg,
	// RGP_ARG_SC_ADDR
	new sc_addr_command_arg,
	// RGP_ARG_SC_ITERATOR
	new sc_iterator_command_arg,
	// RGP_ARG_SC_CONTENT
	new sc_content_command_arg,
	// RGP_ARG_SC_ACTIVITY
	new sc_activity_command_arg,
	// RGP_ARG_SC_WAIT_TYPE
	new sc_wait_type_command_arg,
	// RGP_ARG_SC_WAIT
	new sc_wait_command_arg,
	// RGP_ARG_SC_RETVAL
	new sc_retval_command_arg,
	// RGP_ARG_SC_CONSTRAINT
	new sc_constraint_command_arg,
	// RGP_ARG_SC_INT16
	new sc_int16_command_arg,
	// RGP_ARG_SC_INT32
	new sc_int32_command_arg,
	// RGP_ARG_SC_BOOLEAN
	new sc_boolean_command_arg,
	// RGP_ARG_SC_STRING
	new sc_string_command_arg
};

rgp_command::~rgp_command() {
	std::for_each(args_.begin(), args_.end(),
		boost::checked_deleter<rgp_command_arg>());
}

rgp_command& rgp_command::operator<<(sc_addr o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_ADDR]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_segment *o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_SEGMENT]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_iterator *o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_ITERATOR]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_activity *o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_ACTIVITY]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_wait *o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_WAIT]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_type o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_TYPE]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_wait_type o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_WAIT_TYPE]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(sc_retval o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_RETVAL]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(bool o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_BOOLEAN]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(const sc_string &o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_STRING]->clone();
	a->set_value(o);
	args_.push_back(a);
	++args_count_;
	return *this;
}

rgp_command& rgp_command::operator<<(const Content *o) {
	rgp_command_arg *a = command_arg_prototypes[RGP_ARG_SC_CONTENT]->clone();
	a->set_value(const_cast<Content *>(o));
	args_.push_back(a);
	++args_count_;
	return *this;
}

size_t rgp_command::read(const rgp_objs_registry &registry, std::istreambuf_iterator< char > &in_it,
						 size_t &available_bytes) throw (rgp_error) {
	for (;;) {
		size_t bytes_demand = 0;

		switch (reading_state) {
		case STATE_READ_CMD_ID: {
			bytes_demand = sizeof(rgp_command_id);
			if (available_bytes >= bytes_demand) {
				available_bytes -= bytes_demand;
				bytes_demand = 0;

				id_ = static_cast< rgp_command_t >(read_uint8(in_it));

				if (id_ >= RGP_COMMAND_T_FIRST && id_ <= RGP_COMMAND_T_LAST) {
					reading_state = STATE_READ_ARG_COUNT;
				} else {
					THROW_RGP_ERROR(RV_RGP_ERR_CMD, "Recieved bad command id " << id_);
				}
			}
			break;
		}
		case STATE_READ_ARG_COUNT: {
			bytes_demand = sizeof(uint8_t);
			if (available_bytes >= bytes_demand) {
				available_bytes -= bytes_demand;
				bytes_demand = 0;

				args_count_ = read_uint8(in_it);

				if (info().args_count == args_count_ || 
					(info().read_args_rest && 
					 (info().max_rest_count == -1 || (args_count_ <= (info().args_count + info().max_rest_count))))) {
					if (info().args_count == 0 && !info().read_args_rest) {
						reading_state = STATE_READED;
					} else {
						reading_state = STATE_READ_ARG_TYPE;
					}
				} else {
					THROW_RGP_ERROR(RV_RGP_ERR_ARG_CNT,
						"For " << info().name << " expects " << info().args_count << 
						" count of arguments, but recieved " << args_count_ << " count of arguments");
				}
			}
			break;
		}
		case STATE_READ_ARG_TYPE:
			bytes_demand = sizeof(uint16_t);
			if (available_bytes >= bytes_demand) {
				available_bytes -= bytes_demand;
				bytes_demand = 0;

				rgp_command_arg_t arg_type = static_cast< rgp_command_arg_t >(read_uint16(in_it));

				if (arg_type >= RGP_COMMAND_ARG_T_FIRST && arg_type <= RGP_COMMAND_ARG_T_LAST) {
					// TODO: check expected type
					args_.push_back(command_arg_prototypes[arg_type]->clone());
					reading_state = STATE_READ_ARG;
				} else {
					THROW_RGP_ERROR(RV_RGP_ERR_TYPE,
						"Recieved bad type id " << arg_type << " for command argument " << args_.size() + 1);
				}
			}
			break;
		case STATE_READ_ARG:
			bytes_demand = args_.back()->read(registry, in_it, available_bytes);
			if (bytes_demand == 0) {
				if (args_.size() == args_count_) {
					reading_state = STATE_READED;
				} else {
					reading_state = STATE_READ_ARG_TYPE;
				}
			}
			break;
		case STATE_READED:
			return 0; // command was readed successful
		default:
			assert(false);
		}

		if (bytes_demand != 0)
			return bytes_demand; // at least need bytes_demand for continue reading
	}
}

bool rgp_command::write(rgp_objs_registry &registry, ba::ip::tcp::socket &socket) throw (rgp_error) {
	ba::streambuf out_buf;
	std::ostreambuf_iterator< char > out_it(&out_buf);
	write_uint8(id_, out_it);
	write_uint8(args_count_, out_it);
	BOOST_FOREACH (rgp_command_arg *arg, args_) {
		write_uint16(arg->type(), out_it);
		arg->write(registry, out_it);
	}
	ba::write(socket, out_buf);
	return true;
}

std::ostream& RGP::operator<<(std::ostream &out, const rgp_command &command) {
	out << command.info().name << "( ";
	BOOST_FOREACH (rgp_command_arg *arg, command.args()) {
		out << rgp_command_arg_type_names[arg->type()] << ":<" << arg->to_string() << ">";
		if (arg != command.args().back())
			out << ", ";
	}
	out << " )";
	return out;
}

// @}
