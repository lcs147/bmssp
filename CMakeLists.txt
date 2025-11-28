cmake_minimum_required(VERSION 3.28)
project(bmssp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "CHECKING C++ VERSION: ${CMAKE_CXX_STANDARD}")

add_executable(my_tests tests/tests.cpp)

target_compile_options(my_tests PRIVATE  --max-errors=2)
target_link_options(my_tests PRIVATE --max-errors=2)