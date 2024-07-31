cmake_minimum_required(VERSION 3.5.0)
project(RBTreeTest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(RBTreeTest RBTreeTest.cpp RBTree.h)
target_compile_options(RBTreeTest PUBLIC -Wall -Werror -g)