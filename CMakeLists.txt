cmake_minimum_required(VERSION 3.5.0)
project(TrieTest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Trie STATIC Trie.cpp Trie.h)

add_executable(TrieTest TrieTest.cpp Trie.cpp Trie.h)
target_compile_options(TrieTest PUBLIC -Wall -Werror -g)