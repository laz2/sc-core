
project(libb64)

cmake_minimum_required(VERSION 2.6)

file(GLOB PUBLIC_H "include/b64/*.h")
file(GLOB SRCS "src/*.c")

include_directories(include)
add_library(b64 STATIC ${PUBLIC_H} ${SRCS})

set(LIBB64_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
