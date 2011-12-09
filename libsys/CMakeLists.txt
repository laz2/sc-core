
project(libsys CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(PUBLIC_HEADERS 
	libsys/concurrent.h
	libsys/context.h
	libsys/libsys.h
)

set(SRCS
	libsys/concurrent.cpp
)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set(SRCS ${SRCS}
		libsys/win32-nt4-context.cpp
	)
	set(PUBLIC_HEADERS ${PUBLIC_HEADERS}
		libsys/win32-nt4-context.h
	)
	add_definitions(-D_WIN32_WINNT=0x0500)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	set(SRCS ${SRCS}
		libsys/unix-sysv-context.c
	)
endif()

source_group(public_h  FILES ${PUBLIC_HEADERS})
source_group(src       FILES ${SRCS})

add_library(libsys SHARED ${SRCS} ${PUBLIC_HEADERS})

set_target_name(sys)
install_target_new(libsys)

set(LIBSYS_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBSYS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBSYS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
