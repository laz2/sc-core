
project(md5)

file(GLOB SRCS "md5/*.c" "md5/*.h")

add_library(md5 STATIC ${SRCS})
