cmake_minimum_required(VERSION 3.17)

project(mlang)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH "${mlang_SOURCE_DIR}/cmake")

add_subdirectory(src)