cmake_minimum_required(VERSION 3.28)
project(bmssp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "C++ VERSION: ${CMAKE_CXX_STANDARD}")

if(DEFINED SINGLE_HEADER)
    add_compile_definitions(SINGLE_HEADER="${SINGLE_HEADER}")
endif()

add_executable(tests tests/tests.cpp)
target_compile_options(tests PRIVATE  --max-errors=2)

add_executable(benchmarks tests/benchmarks.cpp)
# set_target_properties(benchmarks PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_compile_options(benchmarks PRIVATE  --max-errors=2 -O3)