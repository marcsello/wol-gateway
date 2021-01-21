cmake_minimum_required(VERSION 3.17)
project(wakeonlangateway C)

set(CMAKE_C_STANDARD 11)

find_library(MHTD microhttpd)

add_executable(wakeonlangateway main.c wol.c wol.h responses.c responses.h config.c config.h)
target_link_libraries(wakeonlangateway microhttpd)
target_link_libraries(wakeonlangateway inih)