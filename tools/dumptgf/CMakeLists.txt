
project(dumptgf)

cmake_minimum_required(VERSION 2.6)

set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK)
find_package(Boost COMPONENTS filesystem REQUIRED)

include_directories(
	${Boost_INCLUDE_DIRS}
	${TCLAP_INCLUDE_DIR}

	${LIBTGF_INCLUDE_DIR}
	${LIBSC_INCLUDE_DIR}
	${LIBPM_INCLUDE_DIR}
)
add_executable(dumptgf dumptgf.cpp)
target_link_libraries(dumptgf libtgf libsc libpm ${Boost_LIBRARIES})

install_target(dumptgf)
