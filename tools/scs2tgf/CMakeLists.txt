
project(scs2tgf C)

cmake_minimum_required(VERSION 2.6)

set(SCS2TGF_SRCS
    list.h
    main.c
    map.c
    map.h
    misc.c
    misc.h
    parser.h
    parser_func.c
    sctext.c
    sctext.h
    values.h
)

find_package(BISON)
find_package(FLEX)


if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	add_definitions(-DSCS2TGF_UNIX)
endif()
bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c)
flex_target(scanner scanner.lex ${CMAKE_CURRENT_BINARY_DIR}/lexer.tab.c)

include_directories(${LIBTGF_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(scs2tgf ${SCS2TGF_SRCS} ${BISON_parser_OUTPUTS} ${FLEX_scanner_OUTPUTS})
target_link_libraries(scs2tgf libtgf)

if(MSVC)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Configure the template file
if(MSVC)
    file(TO_NATIVE_PATH "${LIBTGF_BINARY_DIR}\\$(OutDir)" LIBTGF_BINARY_DIR_NATIVE)
    file(TO_NATIVE_PATH "${LIBSC_BINARY_DIR}\\$(OutDir)" LIBSC_BINARY_DIR_NATIVE)
    file(TO_NATIVE_PATH "${LIBPM_BINARY_DIR}\\$(OutDir)" LIBPM_BINARY_DIR_NATIVE)
    file(TO_NATIVE_PATH "${LIBSYS_BINARY_DIR}\\$(OutDir)" LIBSYS_BINARY_DIR_NATIVE)
    set(USERFILE_ENVIRONMENT "PATH=${LIBTGF_BINARY_DIR_NATIVE};${LIBSC_BINARY_DIR_NATIVE};${LIBSYS_BINARY_DIR_NATIVE};${LIBPM_BINARY_DIR_NATIVE}")
    set(USERFILE_ENVIRONMENT_DEBUG ${USERFILE_ENVIRONMENT})
    set(USERFILE_ENVIRONMENT_RELEASE ${USERFILE_ENVIRONMENT})
    set(USERFILE_ENVIRONMENT_MINSIZEREL ${USERFILE_ENVIRONMENT})
    set(USERFILE_ENVIRONMENT_RELWITHDEBINFO ${USERFILE_ENVIRONMENT})
    set(USERFILE_PLATFORM "Win32")
    
    configure_user_template(scs2tgf)
endif()

install_target(scs2tgf)
