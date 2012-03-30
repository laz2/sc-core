
project(libpm)

file(GLOB NSM_PUBLIC_H  "libpm/nsm/*.h")
file(GLOB NSM_PRIVATE_H "libpm/nsm/*_p.h")
list(REMOVE_ITEM NSM_PUBLIC_H ${NSM_PRIVATE_H})
file(GLOB NSM_SRCS      "libpm/nsm/*.cpp")
source_group(nsm\\public_h  FILES ${NSM_PUBLIC_H})
source_group(nsm\\private_h FILES ${NSM_PRIVATE_H})
source_group(nsm\\src       FILES ${NSM_SRCS})

file(GLOB SCP_PUBLIC_H  "libpm/scp/*.h")
file(GLOB SCP_PRIVATE_H "libpm/scp/*_p.h")
list(REMOVE_ITEM SCP_PUBLIC_H ${SCP_PRIVATE_H})
file(GLOB SCP_SRCS      "libpm/scp/*.cpp")
source_group(scp\\public_h  FILES ${SCP_PUBLIC_H})
source_group(scp\\private_h FILES ${SCP_PRIVATE_H})
source_group(scp\\src       FILES ${SCP_SRCS})

file(GLOB SCL_PUBLIC_H  "libpm/scl/*.h")
file(GLOB SCL_PRIVATE_H "libpm/scl/*_p.h")
list(REMOVE_ITEM SCL_PUBLIC_H ${SCL_PRIVATE_H})
file(GLOB SCL_SRCS      "libpm/scl/*.cpp")
source_group(scl\\public_h  FILES ${SCL_PUBLIC_H})
source_group(scl\\private_h FILES ${SCL_PRIVATE_H})
source_group(scl\\src       FILES ${SCL_SRCS})

file(GLOB CORE_PUBLIC_H  "libpm/*.h")
file(GLOB CORE_PRIVATE_H "libpm/*_p.h")
list(REMOVE_ITEM CORE_PUBLIC_H ${CORE_PRIVATE_H})
file(GLOB CORE_SRCS      "libpm/*.cpp")
source_group(core\\public_h  FILES ${CORE_PUBLIC_H})
source_group(core\\private_h FILES ${CORE_PRIVATE_H})
source_group(core\\src       FILES ${CORE_SRCS})

#
# Setting precompiled headers.
#
use_precompiled_header(SCP_SRCS  "libpm/scp/precompiled_p.h" "libpm/scp/precompiled_p.cpp")
use_precompiled_header(NSM_SRCS  "libpm/libpm_precompiled_p.h" "libpm/libpm_precompiled_p.cpp")
use_precompiled_header(CORE_SRCS "libpm/libpm_precompiled_p.h" "libpm/libpm_precompiled_p.cpp")
use_precompiled_header(SCL_SRCS  "libpm/libpm_precompiled_p.h" "libpm/libpm_precompiled_p.cpp")

set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${LOG2_INCLUDE_DIR} ${BOOST_EXTENSION_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBTGF_INCLUDE_DIR} ${LIBSYS_INCLUDE_DIR} ${LIBSC_INCLUDE_DIR})
add_library(libpm SHARED ${CORE_PUBLIC_H} ${CORE_PRIVATE_H} ${CORE_SRCS}
				  ${NSM_PUBLIC_H} ${NSM_PRIVATE_H} ${NSM_SRCS}
				  ${SCP_PUBLIC_H} ${SCP_PRIVATE_H} ${SCP_SRCS}
				  ${SCL_PUBLIC_H} ${SCL_PRIVATE_H} ${SCL_SRCS})
target_link_libraries(libpm libsc libsys libtgf ${Boost_LIBRARIES})

if(UNIX)
	target_link_libraries(libpm dl)
endif()

add_definitions(-D_CONSOLE)
add_definitions(-DVERBOSE_EXECUTION)
add_definitions(-DVERBOSE_ERRORS)

set_target_name(pm)
install_target_new(libpm)

set(LIBPM_DIR         ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBPM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(LIBPM_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/libpm PARENT_SCOPE)
set(LIBPM_BINARY_DIR  ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
