cmake_minimum_required(VERSION 3.13)
project(wakeonlangateway C)

set(CMAKE_C_STANDARD 11)

find_library(MHTD microhttpd)

add_executable(wakeonlangateway src/main.c src/wol.c src/wol.h src/responses.c src/responses.h src/config.c src/config.h)
target_link_libraries(wakeonlangateway microhttpd)
target_link_libraries(wakeonlangateway inih)