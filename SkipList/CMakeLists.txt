cmake_minimum_required(VERSION 3.5.0)
project(SkipListTest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(SkipListTest SkipListTest.cpp SkipList.h)
target_compile_options(SkipListTest PUBLIC -Wall -Werror -g)