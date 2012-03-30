
project(libtgf)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PUBLIC_HEADERS "libtgf/*[^_p].h")
file(GLOB_RECURSE PRIVATE_HEADERS "libtgf/*_p.h")
file(GLOB_RECURSE SRCS "libtgf/*.c")

source_group(public_h  FILES ${PUBLIC_HEADERS})
source_group(private_h FILES ${PRIVATE_HEADERS})
source_group(src       FILES ${SRCS})

if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	add_definitions(-DTGF_UNIX)
endif()

add_library(libtgf SHARED ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${SRCS})

set_target_name(tgf)
install_target_new(libtgf)

set(LIBTGF_DIR         ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBTGF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBTGF_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/libtgf PARENT_SCOPE)
set(LIBTGF_BINARY_DIR  ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
