
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

if(use_ruby_scconstrc)
	set(STD_CONSTRAINTS_IN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/libsc/gen_std_constraints.rb)
	set(STD_CONSTRAINTS_SOURCE    ${CMAKE_CURRENT_BINARY_DIR}/std_constraints.cpp)

	find_package(Ruby)

	add_custom_command(
		OUTPUT ${STD_CONSTRAINTS_SOURCE}
		COMMAND ${RUBY_EXECUTABLE}
		ARGS ${STD_CONSTRAINTS_IN_SOURCE} ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${STD_CONSTRAINTS_IN_SOURCE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating std_constraints.cpp."
	)
else()
	set(STD_CONSTRAINTS_IN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/libsc/std_constraints.cpp.in)
	set(STD_CONSTRAINTS_SOURCE    ${CMAKE_CURRENT_BINARY_DIR}/std_constraints.cpp)

	find_package(Perl REQUIRED)

	add_custom_command(
		OUTPUT ${STD_CONSTRAINTS_SOURCE}
		COMMAND ${PERL_EXECUTABLE}
		ARGS "${CMAKE_CURRENT_SOURCE_DIR}/utils/gen_constr.pl" ${STD_CONSTRAINTS_IN_SOURCE}
		DEPENDS ${STD_CONSTRAINTS_IN_SOURCE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating std_constraints.cpp."
	)
endif()

source_group(public_h  FILES ${PUBLIC_HEADERS})
source_group(private_h FILES ${PRIVATE_HEADERS})
source_group(src       FILES ${SRCS} ${STD_CONSTRAINTS_IN_SOURCE} ${STD_CONSTRAINTS_SOURCE})

use_precompiled_header(SRCS "libsc/precompiled_p.h" "libsc/precompiled_p.cpp")
add_library(libsc SHARED ${SRCS} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${STD_CONSTRAINTS_IN_SOURCE} ${STD_CONSTRAINTS_SOURCE})

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
