cmake_minimum_required(VERSION 3.10)
project(AlgorithmTemplates LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Compilable standalone template files ----
add_executable(binary_search binary_search.cpp)
add_executable(sorting     sorting.cpp)
add_executable(linked_list linked_list.cpp)
add_executable(tree_traversal tree_traversal.cpp)
add_executable(union_find  union_find.cpp)
add_executable(dijkstra    dijkstra.cpp)
add_executable(knapsack    knapsack.cpp)
add_executable(kmp         kmp.cpp)
add_executable(lru_cache   lru_cache.cpp)

# thread_pool.hpp is header-only and not compiled standalone.
# To use it from a .cpp file, add a target like:
#   add_executable(thread_pool_test thread_pool_test.cpp)
