
project(librgp)

file(GLOB_RECURSE PUBLIC_HEADERS  "librgp/*.h")
file(GLOB_RECURSE PRIVATE_HEADERS "librgp/*_p.h")
list(REMOVE_ITEM PRIVATE_HEADERS PUBLIC_HEADERS)
file(GLOB_RECURSE SRCS            "librgp/*.cpp")

source_group(public_h  FILES ${PUBLIC_HEADERS})
source_group(private_h FILES ${PRIVATE_HEADERS})
source_group(src       FILES ${SRCS})

set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK)
find_package(Boost COMPONENTS system filesystem date_time regex thread REQUIRED)

use_precompiled_header(SRCS "librgp/precompiled_p.h" "librgp/precompiled_p.cpp")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${LIBTGF_INCLUDE_DIR}
	${LIBSYS_INCLUDE_DIR}
	${LIBSC_INCLUDE_DIR}
	${LIBPM_INCLUDE_DIR}
	${LOG2_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS})
add_library(librgp SHARED ${PRIVATE_HEADERS} ${PUBLIC_HEADERS} ${SRCS})
target_link_libraries(librgp libsc libsys libtgf libpm ${Boost_LIBRARIES})

if(UNIX)
	target_link_libraries(librgp pthread)
endif()

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0501)
endif()

set_target_name(rgp)
install_target_new(librgp)

set(LIBRGP_DIR         ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBRGP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBRGP_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/librgp PARENT_SCOPE)
set(LIBRGP_BINARY_DIR  ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
