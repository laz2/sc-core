
project(wave_find_path_example)

cmake_minimum_required(VERSION 2.6)

set(SRCS
    wave_find_path.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBSC_INCLUDE_DIR} ${LIBTGF_INCLUDE_DIR} ${LIBSYS_INCLUDE_DIR})
add_executable(wave_find_path ${SRCS})
target_link_libraries(wave_find_path libsc)

# Configure the template file
if(MSVC)
    file(TO_NATIVE_PATH "${LIBTGF_BINARY_DIR}\\$(OutDir)" LIBTGF_BINARY_DIR_NATIVE)
    file(TO_NATIVE_PATH "${LIBSC_BINARY_DIR}\\$(OutDir)" LIBSC_BINARY_DIR_NATIVE)
    file(TO_NATIVE_PATH "${LIBSYS_BINARY_DIR}\\$(OutDir)" LIBSYS_BINARY_DIR_NATIVE)
    set(USERFILE_ENVIRONMENT "PATH=${LIBTGF_BINARY_DIR_NATIVE};${LIBSC_BINARY_DIR_NATIVE};${LIBSYS_BINARY_DIR_NATIVE}")
    set(USERFILE_ENVIRONMENT_DEBUG ${USERFILE_ENVIRONMENT})
    set(USERFILE_ENVIRONMENT_RELEASE ${USERFILE_ENVIRONMENT})
    set(USERFILE_ENVIRONMENT_MINSIZEREL ${USERFILE_ENVIRONMENT})
    set(USERFILE_ENVIRONMENT_RELWITHDEBINFO ${USERFILE_ENVIRONMENT})
    set(USERFILE_PLATFORM "Win32")
    
    configure_user_template(wave_find_path)
endif()

set(EXAMPLE_LIB_DIR "../../lib")
set(EXAMPLE_INCLUDE_DIR "../../include")
set(EXAMPLE_LIBSC_INCLUDE_DIR "${EXAMPLE_INCLUDE_DIR}/libsc")
set(EXAMPLE_LIBTGF_INCLUDE_DIR "${EXAMPLE_INCLUDE_DIR}/libtgf")
set(EXAMPLE_LIBSYS_INCLUDE_DIR "${EXAMPLE_INCLUDE_DIR}/libsys")
configure_file(CMakeLists.txt.in ${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt @ONLY)

file(GLOB files ${CMAKE_CURRENT_BINARY_DIR}/wave)  
install(FILES ${SRCS} ${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt
    DESTINATION examples/wave_find_path
)
