
cmake_minimum_required(VERSION 2.6)

add_subdirectory(wave_find_path wave_find_path)
#add_subdirectory(ext_dynmod ext_dynmod)

install(DIRECTORY ${SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/fs_repo_src
	DESTINATION examples
	PATTERN ".*" EXCLUDE
)