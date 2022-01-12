cmake_minimum_required(VERSION 3.8)
project(MIMUW-FORK C)

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_C_STANDARD "11")
set(CMAKE_C_FLAGS "-g -Wall -Wextra -Wno-sign-compare")

add_library(err src/err.c)
add_library(pathUtils src/path_utils.c)
add_library(HashMap src/HashMap.c)
add_library(monitor src/monitor.c)
add_library(Tree src/Tree.c)

include("${CMAKE_CURRENT_SOURCE_DIR}/src/testy-zad2/CMakeExtension.txt")
target_link_libraries(monitor pthread)
target_link_libraries(main Tree monitor pathUtils HashMap err pthread)

install(TARGETS DESTINATION .)
