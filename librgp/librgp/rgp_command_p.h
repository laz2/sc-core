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
 * @file rgp_command_p.h
 * @brief RGP-command representation.
 * @author Dmitry Lazurkin
 * @ingroup librgp
 */

#ifndef __LIBRGP_RGP_COMMAND_H_INCLUDED__
#define __LIBRGP_RGP_COMMAND_H_INCLUDED__

/// @addtogroup librgp
/// @{

#include "rgp_objs_registry_p.h"

#if defined(_MSC_VER)
// disable warning for MSVC C++ exception specification
// ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
#endif 

namespace RGP {

namespace ba=boost::asio;
namespace bs=boost::system;

class rgp_error : public std::runtime_error {
public:
	rgp_error(sc_retval _code, const std::string &msg="")
		: std::runtime_error(msg), code_(_code) {}

	virtual sc_retval code() const {
		return code_;
	}

private:
	sc_retval code_;
};

#define THROW_RGP_ERROR(code, out) \
	{ \
		std::ostringstream __out_buf; \
		__out_buf << out; \
		throw rgp_error(code, __out_buf.str()); \
	}

/// RGP command argument.
class rgp_command_arg {
public:
	/// State of RGP argument value.
	enum value_state_t {
		EMPTY,
		READING,
		VALUE,
		WRITING
	};

	rgp_command_arg(rgp_command_arg_t _type) : type_(_type), state_(EMPTY) {}

	virtual ~rgp_command_arg() {};

	/// Return arguments value if it presents.
	virtual boost::any value() const = 0;

	/// Check if argument value is nil.
	virtual bool is_nil() const { return false; }

	template< typename Type >
	Type value() const { return boost::any_cast<Type>(this->value()); }

	template< typename Type >
	Type value_not_nil() const {
		if (is_nil())
			THROW_RGP_ERROR(RV_ERR_GEN, "Value for argument with type " 
				<< rgp_command_arg_type_names[type_] << " is NIL"); // TODO: return code
		return value<Type>();
	}

	/// Set argument value and value state.
	virtual void set_value(const boost::any &v) = 0;

	/// Clone argument with empty value.
	virtual rgp_command_arg* clone() = 0;

	/// Return argument value as string. For logging purpose.
	virtual std::string to_string() const = 0;

	/**
	 * @brief Read argument value from @p in_buf.
	 * @param registry RGP objects registry.
	 * @param in_it input stream iterator.
	 * @param[in,out] available_bytes available bytes for reading from @p in_it.
	 * @exception rgp_error error occurred while argument reading.
	 * @return at least bytes amount of argument value rest.
	 */
	virtual size_t read(const rgp_objs_registry &registry, std::istreambuf_iterator< char > &in_it,
		size_t &available_bytes) throw (rgp_error) = 0;

	/**
	 * @brief Write argument value to @p socket.
	 * @param registry RGP objects registry.
	 * @param out_it output iterator.
	 * @exception rgp_error error occurred while argument writing.
	 */
	virtual void write(rgp_objs_registry &registry, std::ostreambuf_iterator< char > &out_it) throw (rgp_error) = 0;

	/// Return type of argument.
	rgp_command_arg_t type() const { return type_; }

	/// Return argument value state.
	value_state_t state() const { return state_; }

protected:
	rgp_command_arg_t type_;  ///< type of argument
	value_state_t     state_; ///< argument value state
};

/// RGP response/reply command.
class rgp_command {
public:
	/// RGP command arguments vector type.
	typedef std::vector< rgp_command_arg* > arg_vector;

public:
	rgp_command() : id_(static_cast< rgp_command_t >(-1)), reading_state(STATE_READ_CMD_ID), args_count_(0) {}

	rgp_command(rgp_command_t _id) : id_(_id), reading_state(STATE_READED), args_count_(0) {}

	~rgp_command();

	void id(const rgp_command_t _id) { id_ = _id; }

	rgp_command_t id() const { return id_; }

	const rgp_command_info& info() const {
		assert(id_ != -1);
		return rgp_command_infos[id_];
	}

	size_t args_count() const { return args_count_; }

	const arg_vector& args() const { return args_; }

	bool is_readed() const { return reading_state == STATE_READED; }

	rgp_command& operator<<(sc_addr o);

	rgp_command& operator<<(sc_segment *o);

	rgp_command& operator<<(sc_iterator *o);

	rgp_command& operator<<(sc_activity *o);

	rgp_command& operator<<(sc_wait *o);

	rgp_command& operator<<(sc_type o);

	rgp_command& operator<<(sc_wait_type o);

	rgp_command& operator<<(sc_retval o);

	rgp_command& operator<<(bool o);

	rgp_command& operator<<(const sc_string &o);

	rgp_command& operator<<(const Content *o);

	/**
	 * @brief Read command from @p in_buf.
	 * @param registry RGP objects registry.
	 * @param in_it input stream iterator.
	 * @param[in,out] available_bytes available bytes for reading from @p in_it.
	 * @exception rgp_error error occurred while command reading.
	 * @return at least bytes amount of command rest.
	 */
	size_t read(const rgp_objs_registry &registry, std::istreambuf_iterator< char > &in_it,
		size_t &available_bytes) throw (rgp_error);

	/**
	 * @brief Write command value to @p socket.
	 * @note May use async writing to socket.
	 * @param registry RGP objects registry.
	 * @param socket output socket.
	 * @exception rgp_error error occurred while command writing.
	 * @return is written or not. False if use async writing to socket.
	 */
	bool write(rgp_objs_registry &registry, ba::ip::tcp::socket &socket) throw (rgp_error);

	friend std::ostream& operator<<(std::ostream &out, const rgp_command &command);

private:
	/// State of finite machine for command reading.
	enum read_state_t {
		STATE_READ_CMD_ID,
		STATE_READ_ARG_COUNT,
		STATE_READ_ARG_TYPE,
		STATE_READ_ARG,
		STATE_READED
	};

	read_state_t reading_state; ///< current state of finite machine for command reading

	rgp_command_t  id_;         ///< command id
	size_t         args_count_; ///< real arguments count
	arg_vector     args_;       ///< arguments vector
};

std::ostream& operator<<(std::ostream &out, const rgp_command &command);

} // namespace RGP

/// @}

#endif // __LIBRGP_RGP_COMMAND_H_INCLUDED__
