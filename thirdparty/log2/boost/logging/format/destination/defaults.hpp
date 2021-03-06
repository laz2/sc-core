// destination_defaults.hpp

// Boost Logging library
//
// Author: John Torjo, www.torjo.com
//
// Copyright (C) 2007 John Torjo (see www.torjo.com for email)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
// See http://www.torjo.com/log2/ for more details


#ifndef JT28092007_destination_defaults_HPP_DEFINED
#define JT28092007_destination_defaults_HPP_DEFINED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/logging/detail/fwd.hpp>
#include <boost/logging/detail/manipulator.hpp>
#include <boost/logging/format/destination/convert_destination.hpp>
#include <boost/logging/format/destination/file.hpp>
#include <iostream>

namespace boost { namespace logging { namespace destination {


/** 
    @brief Writes the string to console
*/
template<class convert_dest = do_convert_destination > struct cout_t : is_generic, boost::logging::op_equal::always_equal {

    template<class msg_type> void operator()(const msg_type & msg) const {
#ifndef BOOST_LOG_USE_WCHAR_T
        convert_dest::write(msg, std::cout);
        std::cout.flush();
#else
        convert_dest::write(msg, std::wcout);
        std::wcout.flush();
#endif
    }
};


/** 
    @brief Writes the string to cerr
*/
template<class convert_dest = do_convert_destination > struct cerr_t : is_generic, boost::logging::op_equal::always_equal {

    template<class msg_type> void operator()(const msg_type & msg) const {
#ifndef BOOST_LOG_USE_WCHAR_T
        convert_dest::write(msg, std::cerr);
        std::cerr.flush();
#else
        convert_dest::write(msg, std::wcerr);
        std::wcerr.flush();
#endif
    }
};



/** 
    @brief writes to stream. 

    @note:
    The stream must outlive this object! Or, clear() the stream, before the stream is deleted.
*/
template<class convert_dest = do_convert_destination > struct stream_t : is_generic, non_const_context< std::basic_ostream<boost::logging::char_type> * > {
    typedef std::basic_ostream<char_type> stream_type;
    typedef non_const_context< stream_type* > non_const_context_base;

    stream_t(stream_type * s) : non_const_context_base(s) {
    }
    stream_t(stream_type & s) : non_const_context_base(&s) {
    }

    template<class msg_type> void operator()(const msg_type & msg) const {
        if ( non_const_context_base::context() )
            convert_dest::write(msg, *non_const_context_base::context());
    }

    bool operator==(const stream_t & other) const {
        return non_const_context_base::context() != other.non_const_context_base::context();
    }

    /** 
        @brief resets the stream. Further output will be written to this stream
    */
    void stream(stream_type * p) { non_const_context_base::context() = p; }

    /** 
        @brief clears the stream. Further output will be ignored
    */
    void clear() { stream(0); }
};




/** 
    @brief Writes the string to output debug window

    For non-Windows systems, this is the console.
*/
template<class convert_dest = do_convert_destination > struct dbg_window_t : is_generic, boost::logging::op_equal::always_equal {

    template<class msg_type> void operator()(const msg_type & msg) const {
#ifdef BOOST_WINDOWS
#ifndef BOOST_LOG_USE_WCHAR_T
    ::OutputDebugStringA( convert_dest::do_convert(msg, into<const char*>() ) );
#else
    ::OutputDebugStringW( convert_dest::do_convert(msg, into<const wchar_t*>() ) );    
#endif
#else
        // non windows - dump to console
        std::cout << msg; 
#endif
    }
};


/** @brief cout_t with default values. See cout_t

@copydoc cout_t
*/
typedef cout_t<> cout;

/** @brief cerr_t with default values. See cerr_t

@copydoc cerr_t
*/
typedef cerr_t<> cerr;

/** @brief stream_t with default values. See stream_t

@copydoc stream_t
*/
typedef stream_t<> stream;

/** @brief dbg_window_t with default values. See dbg_window_t

@copydoc dbg_window_t
*/
typedef dbg_window_t<> dbg_window;


}}}

#endif

