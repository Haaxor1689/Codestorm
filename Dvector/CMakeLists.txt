cmake_minimum_required(VERSION 3.0)
project(vector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic")

add_executable(main main.cpp dvector.h)
add_executable(vector_test dvector.cpp dvector.h)
