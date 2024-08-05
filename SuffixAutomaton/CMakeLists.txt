cmake_minimum_required(VERSION 3.5.0)
project(SuffixAutomatonTest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(SuffixAutomaton STATIC SuffixAutomaton.cpp SuffixAutomaton.h)

add_executable(SuffixAutomatonTest SuffixAutomatonTest.cpp SuffixAutomaton.cpp SuffixAutomaton.h)
target_compile_options(SuffixAutomatonTest PUBLIC -Wall -Werror -g)