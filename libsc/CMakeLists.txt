
project(libsc)

set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK)
find_package(Boost COMPONENTS system REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${MD5_INCLUDE_DIR})
include_directories(${LIBSYS_INCLUDE_DIR} ${LIBTGF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PUBLIC_HEADERS  "libsc/*[^_p].h")
file(GLOB_RECURSE PRIVATE_HEADERS "libsc/*_p.h")
file(GLOB_RECURSE SRCS            "libsc/*.cpp")

source_group(public_h  FILES ${PUBLIC_HEADERS})
source_group(private_h FILES ${PRIVATE_HEADERS})
source_group(src       FILES ${SRCS})

use_precompiled_header(SRCS "libsc/precompiled_p.h" "libsc/precompiled_p.cpp")
add_library(libsc SHARED ${SRCS} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	target_link_libraries(libsc rpcrt4)
endif()

target_link_libraries(libsc libsys libtgf ${Boost_LIBRARIES} md5)

set_target_name(sc)
install_target_new(libsc)

set(LIBSC_DIR         ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBSC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBSC_BINARY_DIR  ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

enable_project_testing()
