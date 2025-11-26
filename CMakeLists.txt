cmake_minimum_required(VERSION 3.28)
project(bmssp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "CHECKING C++ VERSION: ${CMAKE_CXX_STANDARD}")

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.11.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(my_tests tests/tests.cpp)
target_link_libraries(my_tests PRIVATE Catch2::Catch2WithMain)