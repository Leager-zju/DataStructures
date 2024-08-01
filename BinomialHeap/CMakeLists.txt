cmake_minimum_required(VERSION 3.5.0)
project(BinomialHeapTest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(BinomialHeapTest BinomialHeapTest.cpp BinomialHeap.h)
target_compile_options(BinomialHeapTest PUBLIC -Wall -Werror -g)