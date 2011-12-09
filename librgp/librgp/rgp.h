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
 * @defgroup librgp librgp - Remote Graph Protocol library
 * @{
 */

/**
 * @file rgp.h
 * @brief Definition of Remote Graph Protocol (RGP) for working with SC-memory.
 * RGP implements remote sc_session interface.
 **/

#ifndef __LIBRGP_RGP_H_INCLUDED__
#define __LIBRGP_RGP_H_INCLUDED__

#include <libsc/libsc.h>

#include <boost/cstdint.hpp>

#include "config.h"

/**
Remote Graph Protocol (RGP) description:

1. 
  offset   size    description
---------------------------------------------------------------
    0       1       commands type
    1       1       arguments count
    4       *       arguments (see previous field)
---------------------------------------------------------------
	
2. Arguments format
  offset   size    description
---------------------------------------------------------------
    0       2      arguments type
    2              arguments data (depends on type)
---------------------------------------------------------------

3. Arguments type
  name    type id   description
---------------------------------------------------------------
sc_type     0       one byte
int16       1       16bit integer
int32       2       32bit integer
data        3       first int32 gives length
                    other - binary data of given length
string      4       first int32 gives length
                    other - non null-terminated string of 8 bit
                    chars. NOTE: preferred encoding is UTF-8
---------------------------------------------------------------

4. Commands type
  name                cmd id    params
---------------------------------------------------------------
PING                    0       none
PONG                    1       none
CREATE_ELEMENT          2       sc_type
ERASE_ELEMENT           3       sc_addr(uint32)
MERGE_ELEMENT           4       sc_addr(uint32) from
                                sc_addr(uint32)	to
SET_ELEMENT_END         5       sc_addr(uint32) arc
                                sc_addr(uint32)	elmn
GET_ELEMENT_END         6       sc_addr(uint32) arc
SET_ELEMENT_BEGIN       7       sc_addr(uint32) arc
                                sc_addr(uint32)	elmn
GET_ELEMENT_BEGIN       8       sc_addr(uint32) arc
GET_ELEMENT_TYPE        9       sc_addr(uint32) elmn
CHANGE_ELEMENT_TYPE     10      sc_addr(uint32) elmn
                                sc_type
GET_ELEMENT_CONTENT     11      sc_addr(uint32) elmn
SET_ELEMENT_CONTENT     12      sc_addr(uint32) elmn
                                sc_content(data)
CREATE_ITERATOR         13      sc_constratint(uint32)
NEXT_ITERATOR           14      sc_iterator(uint32)
GET_ITERATOR_VALUE      15      sc_iterator(uint32)
                                sc_iterator_index(uint16)
ERASE_ITERATOR          16      sc_iterator(uint32)
ATTACH_WAIT             17      sc_wait(uint32)
DETACH_WAIT             18      sc_wait(uint32)
ACTIVATE                19      sc_addr(uint32)	agent
                                sc_addr(uint32) [0..3] prm 
REIMPLEMENT             20
RETURN                  21      sc_error(uint16) error status
                                sc_addr(uint32) [0..n] prm 
LOGIN                   22      none
CLOSE                   23      none

4. Interaction
---------------------------------------------------------------
PING                    PONG
CREATE_ELEMENT          RETURN sc_error, sc_addr?
ERASE_ELEMENT           RETURN sc_error
MERGE_ELEMENT           RETURN sc_error
SET_ELEMENT_END         RETURN sc_error
GET_ELEMENT_END         RETURN sc_error, sc_addr?
SET_ELEMENT_BEGIN       RETURN sc_error
GET_ELEMENT_BEGIN       RETURN sc_error, sc_addr?
GET_ELEMENT_TYPE        RETURN sc_error, sc_type?
CHANGE_ELEMENT_TYPE     RETURN sc_error
GET_ELEMENT_CONTENT     RETURN sc_error, sc_content?
SET_ELEMENT_CONTENT     RETURN sc_error
CREATE_ITERATOR         RETURN sc_error, sc_iterator?
NEXT_ITERATOR           RETURN sc_error, boolean (!)
GET_ITERATOR_VALUE      RETURN sc_error, sc_addr?
ERASE_ITERATOR          RETURN sc_error
RETURN                  none
LOGIN                   none
CLOSE                   none
 **/

namespace RGP {

using boost::int8_t;
using boost::uint8_t;
using boost::int16_t;
using boost::uint16_t;
using boost::int32_t;
using boost::uint32_t;

/// Values for command_id
enum rgp_command_t {
	// Requests
	RGP_REQ_LOGIN = 0,
	RGP_REQ_CLOSE,
	RGP_REQ_CREATE_SEGMENT,
	RGP_REQ_OPEN_SEGMENT,
	
	RGP_REQ_GET_IDTF,
	RGP_REQ_SET_IDTF,
	RGP_REQ_ERASE_IDTF,
	RGP_REQ_FIND_BY_IDTF,
	
	RGP_REQ_CREATE_EL,
	RGP_REQ_GEN3_F_A_F,
	RGP_REQ_GEN5_F_A_F_A_F,
	RGP_REQ_ERASE_EL,
	RGP_REQ_MERGE_EL,

	RGP_REQ_SET_EL_BEGIN,
	RGP_REQ_SET_EL_END,
	RGP_REQ_GET_EL_BEGIN,
	RGP_REQ_GET_EL_END,
	RGP_REQ_GET_EL_TYPE,
	RGP_REQ_CHANGE_EL_TYPE,
	RGP_REQ_GET_EL_CONTENT,
	RGP_REQ_SET_EL_CONTENT,

	RGP_REQ_CREATE_ITERATOR,
	RGP_REQ_NEXT_ITERATOR,
	RGP_REQ_ERASE_ITERATOR,
	
	RGP_REQ_ATTACH_WAIT,
	RGP_REQ_DETACH_WAIT,
	RGP_REQ_REIMPLEMENT,

	RGP_REQ_ACTIVATE,

	RGP_REQ_GET_SEG_INFO,
	RGP_REQ_GET_EL_INFO,

	RGP_REQ_ACTIVATE_WAIT,

	// Replies
	RGP_REP_RETURN
};

const rgp_command_t RGP_COMMAND_T_FIRST = RGP_REQ_LOGIN;
const rgp_command_t RGP_COMMAND_T_LAST  = RGP_REP_RETURN;

/// Constants for sending argument type
enum rgp_command_arg_t {
	RGP_ARG_UNKNOWN = 0,   /// not argument type, for internal purpose
	RGP_ARG_SC_TYPE,       /// sc_type_id
	RGP_ARG_SC_SEGMENT,    /// sc_segment_id
	RGP_ARG_SC_ADDR,       /// sc_addr_id
	RGP_ARG_SC_ITERATOR,   /// sc_iterator_id
	RGP_ARG_SC_CONTENT,    /// sc_content_id and data (see #rgp_content_t)
	RGP_ARG_SC_ACTIVITY,   /// sc_activity_id
	RGP_ARG_SC_WAIT_TYPE,  /// sc_wait_type_id
	RGP_ARG_SC_WAIT,       /// sc_wait_id
	RGP_ARG_SC_RETVAL,     /// sc_retval_id
	RGP_ARG_SC_CONSTRAINT, /// sc_constraint_id and constraint arguments (see #rgp_constraint_t)
	RGP_ARG_SC_INT16,      /// int16
	RGP_ARG_SC_INT32,      /// int32
	RGP_ARG_SC_BOOLEAN,    /// boolean
	RGP_ARG_SC_STRING,     /// int32 + non null terminated string data with 8-bit chars
};

const rgp_command_arg_t RGP_COMMAND_ARG_T_FIRST = RGP_ARG_UNKNOWN;
const rgp_command_arg_t RGP_COMMAND_ARG_T_LAST  = RGP_ARG_SC_STRING;

/// Content types. First value for ARG_SC_CONTENT
enum rgp_content_t {
	RGP_CT_EMPTY = 0,
	RGP_CT_STRING,
	RGP_CT_INT,
	RGP_CT_REAL,
	RGP_CT_DATA
};

/// Predefined constraint types. First value for ARG_SC_CONSTRAINT
enum rgp_constraint_t {
	RGP_CONSTR_UNKNOWN=0,
	RGP_CONSTR_ALL_INPUT,
	RGP_CONSTR_ALL_OUTPUT,
	RGP_CONSTR_3_f_a_a,
	RGP_CONSTR_3_f_a_f,
	RGP_CONSTR_3_a_a_f,
	RGP_CONSTR_5_f_a_a_a_a,
	RGP_CONSTR_5_f_a_a_a_f,
	RGP_CONSTR_5_f_a_f_a_a,
	RGP_CONSTR_5_f_a_f_a_f,
	RGP_CONSTR_5_a_a_a_a_f,
	RGP_CONSTR_5_a_a_f_a_a,
	RGP_CONSTR_5_a_a_f_a_f,
	RGP_CONSTR_3l2_f_a_a_a_f,
	RGP_CONSTR_ON_SEGMENT,
	RGP_CONSTR_ORD_BIN_CONN1,
	RGP_CONSTR_ORD_BIN_CONN2
};

struct rgp_command_info {
	rgp_command_t     id;
	const char       *name;
	size_t            args_count;
	bool              read_args_rest;
	signed int        max_rest_count;
	rgp_command_arg_t args[7];
};

struct rgp_constraint_info {
	rgp_constraint_t rgp_id;
	sc_uint          sc_id;
};

extern LIBRGP_API rgp_command_info    rgp_command_infos[];
extern LIBRGP_API rgp_constraint_info rgp_constraint_infos[];
extern LIBRGP_API const char *rgp_command_arg_type_names[]; ///< Human-readable names for rgp_command_arg_t values

const sc_retval RV_RGP_ERR_GEN          = -1000;                  ///< RGP Generic error
const sc_retval RV_RGP_ERR_CMD          = RV_RGP_ERR_GEN - 1;     ///< Bad RGP command id
const sc_retval RV_RGP_ERR_ARG_CNT      = RV_RGP_ERR_CMD - 1;     ///< Bad RGP argument count for command
const sc_retval RV_RGP_ERR_TYPE         = RV_RGP_ERR_ARG_CNT - 1; ///< Bad RGP type id
const sc_retval RV_RGP_ERR_NOT_EXP_TYPE = RV_RGP_ERR_TYPE - 1;    ///< Not expected current RGP type id for command

//const sc_retval RV_RGP_ERR_LOGIN = RV_RGP_ERR_TYPE - 1; ///< RGP client not logged

typedef uint8_t  rgp_command_id;        /// id of command in RGP-session
typedef uint32_t rgp_sc_type_id;        /// sc_type in RGP-session
typedef uint32_t rgp_object_id;         /// id of anyone object in RGP-session
typedef uint16_t rgp_sc_content_id;     /// id of content type in RGP-session
typedef uint16_t rgp_sc_wait_type_id;   /// sc_wait_type in RGP-session
typedef int16_t  rgp_sc_retval_id;      /// sc_retval in RGP-session
typedef uint16_t rgp_sc_constraint_id;  /// id of predefined sc_constraints in RGP-session

} // namespace RGP

#endif //__LIBRGP_RGP_H_INCLUDED__

/**@}*/
