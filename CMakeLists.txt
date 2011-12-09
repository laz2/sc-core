
cmake_minimum_required(VERSION 2.6)

project(sc-core)

set(PACKAGE_NAME sc-core)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)

option(SC_CORE_BUILD_DOC     "Build sc-core documentation (need doxygen)." OFF)
option(SC_CORE_BUILD_EXAMPLE "Build sc-core examples." OFF)
option(SC_CORE_BUILD_TOOL    "Build sc-core tools." OFF)
option(SC_CORE_PACK          "Pack sc-core module in various formats." OFF)

set(SC_CORE_SOURCE_ROOT ${CMAKE_SOURCE_DIR})

macro(use_precompiled_header SRC_LIST_VAR HDR_FILE SRC_FILE)
	get_filename_component(PCH_HEADER ${HDR_FILE} NAME)
	get_filename_component(PCH_BINARY ${HDR_FILE} NAME_WE)

	set(PCH_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PCH_BINARY}.pch")

	if(MSVC)
		set(SRC_LIST ${${SRC_LIST_VAR}})

		set_source_files_properties(${SRC_LIST} PROPERTIES
			COMPILE_FLAGS "/Yu${PCH_HEADER} /Fp${PCH_BINARY}"
			OBJECT_DEPENDS "${PCH_BINARY}")

		set_source_files_properties(${SRC_FILE} PROPERTIES
			COMPILE_FLAGS "/Yc${PCH_HEADER} /Fp${PCH_BINARY}"
			OBJECT_OUTPUTS "${PCH_BINARY}"
			OBJECT_DEPENDS "")
	endif()
endmacro()

macro(install_boost_libraries)
	if(Boost_FOUND AND WIN32)
		list(REMOVE_ITEM Boost_LIBRARIES debug optimized)

		foreach(lib ${Boost_LIBRARIES})
			if(MSVC)
				string(REPLACE ".lib" ".dll" lib ${lib})
			else()
				string(REPLACE ".a" ".dll" lib ${lib})
			endif()

			list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${lib})
		endforeach()

		list(REMOVE_DUPLICATES CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
		set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} PARENT_SCOPE)
	endif()
endmacro()

macro(install_target name)
	if(UNIX)
	    install(TARGETS ${name}
    	    RUNTIME DESTINATION bin
        	ARCHIVE DESTINATION lib
        	LIBRARY DESTINATION lib
			FRAMEWORK DESTINATION lib
			BUNDLE DESTINATION lib
	    )
	else()
		install(TARGETS ${name}
			RUNTIME DESTINATION bin
			ARCHIVE DESTINATION lib
			LIBRARY DESTINATION bin
		)
	endif()

	set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
	if(EXISTS ${INCLUDE_DIR} AND IS_DIRECTORY ${INCLUDE_DIR})
		install(
			DIRECTORY ${INCLUDE_DIR}/
			DESTINATION include/
			FILES_MATCHING PATTERN "*.h"
			PATTERN ".*" EXCLUDE
		)
	endif()
	install_boost_libraries()
endmacro()

macro(install_target_new name)
	if(UNIX)
		install(TARGETS ${name}
			RUNTIME DESTINATION bin
			ARCHIVE DESTINATION lib
			LIBRARY DESTINATION lib
			FRAMEWORK DESTINATION lib
			BUNDLE DESTINATION lib
		)
	else()
		install(TARGETS ${name}
			RUNTIME DESTINATION bin
			ARCHIVE DESTINATION lib
			LIBRARY DESTINATION bin
		)
	endif()

	install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${name}
		DESTINATION include/
		FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
		PATTERN ".*" EXCLUDE
		PATTERN "*_p.h" EXCLUDE
		PATTERN "*_p.hpp" EXCLUDE
	)
	install_boost_libraries()
endmacro()

macro(set_target_name name)
	set(TGT "lib${name}")

	if(MINGW)
		set(TGT_NAME ${name})
	elseif(MSVC)
		set(TGT_NAME "lib${name}")
	elseif(UNIX)
		set(TGT_NAME ${name})
	endif()

	set_target_properties(${TGT} PROPERTIES OUTPUT_NAME ${TGT_NAME})
	set_target_properties(${TGT} PROPERTIES OUTPUT_NAME_RELEASE ${TGT_NAME})
	set_target_properties(${TGT} PROPERTIES OUTPUT_NAME_DEBUG "${TGT_NAME}d")
endmacro()

macro(enable_project_testing)
	if(${PROJECT_NAME}_test)
		file(GLOB_RECURSE TEST_SRCS "test/*.cpp")

		find_package(Boost COMPONENTS unit_test_framework REQUIRED)
		include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
		set(TEST ${PROJECT_NAME}_test)
		add_executable(${TEST} ${TEST_SRCS})
		target_link_libraries(${TEST} libsc libtgf libsys libpm librgp ${Boost_LIBRARIES})
		enable_testing()
		add_test(${TEST} ${TEST})
	endif()
endmacro()

macro(add_subdirectory_with_bin name)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${name} ${CMAKE_CURRENT_BINARY_DIR}/${name})
endmacro()

macro(configure_user_template PROJECT_NAME)
	# Find user and system name
	set(SYSTEM_NAME $ENV{USERDOMAIN} CACHE STRING SystemName)
	set(USER_NAME $ENV{USERNAME} CACHE STRING UserName)

	set(USER_FILE ${PROJECT_NAME}.vcproj.${SYSTEM_NAME}.${USER_NAME}.user)

	set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})

	configure_file(${SC_CORE_SOURCE_ROOT}/cmake/UserTemplate.user ${USER_FILE} @ONLY)
endmacro()

set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty ${CMAKE_CURRENT_BINARY_DIR}/thirdparty)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libsys ${CMAKE_CURRENT_BINARY_DIR}/libsys)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libtgf ${CMAKE_CURRENT_BINARY_DIR}/libtgf)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libsc ${CMAKE_CURRENT_BINARY_DIR}/libsc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpm ${CMAKE_CURRENT_BINARY_DIR}/libpm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/librgp ${CMAKE_CURRENT_BINARY_DIR}/librgp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libscc ${CMAKE_CURRENT_BINARY_DIR}/libscc)

if(SC_CORE_BUILD_DOCS)
	add_subdirectory_with_bin(docs)
endif()

if(SC_CORE_BUILD_EXAMPLES)
	add_subdirectory_with_bin(examples)
endif()

if(SC_CORE_BUILD_TOOLS)
	add_subdirectory_with_bin(tools)
endif()

set(CMAKE_INSTALL_DEBUG_LIBRARIES ON)
include(InstallRequiredSystemLibraries)

if(PACK)
	file(TO_NATIVE_PATH ${SC_CORE_SOURCE_ROOT} SC_CORE_SOURCE_ROOT_NATIVE)
	string(REPLACE "\\" "\\\\" CPACK_SC_CORE_SOURCE_ROOT ${SC_CORE_SOURCE_ROOT_NATIVE})

	set(CPACK_PACKAGING_INSTALL_PREFIX "../../../../../build/sc-core")
	set(CPACK_TEMPORARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sc-core")

	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Semantic Code Core Module")
	set(CPACK_PACKAGE_VENDOR "OSTIS")
	#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER")
	set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "sc-core-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
	if(WIN32 AND NOT UNIX)
		# There is a bug in NSI that does not handle full unix paths properly. Make
		# sure there is at least one set of four (4) backlasshes.
		#set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
		#set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
		#set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
		#set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
		#set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
		#set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
		set(CPACK_NSIS_MODIFY_PATH ON)
		set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
	else()
		set(CPACK_STRIP_FILES "bin/MyExecutable")
		set(CPACK_SOURCE_STRIP_FILES "")
	endif()

	include(CPack)
endif()
