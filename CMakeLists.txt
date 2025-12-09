cmake_minimum_required(VERSION 3.28)
project(bmssp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "CHECKING C++ VERSION: ${CMAKE_CXX_STANDARD}")

add_executable(tests tests/tests.cpp)

target_compile_options(tests PRIVATE  --max-errors=2 -fsanitize=address -g)
target_link_options(tests PRIVATE --max-errors=2 -fsanitize=address -g)