
project(antlr3c)

cmake_minimum_required(VERSION 2.6)

file(GLOB PUBLIC_H "include/*.h")
file(GLOB SRCS "src/*.c")

include_directories(include)
add_library(antlr3c STATIC ${PUBLIC_H} ${SRCS})

set(LIBANTLR3C_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
