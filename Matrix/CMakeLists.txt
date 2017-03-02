cmake_minimum_required(VERSION 3.5)
project(Matrix)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic -Werror")

set(SOURCE_FILES main.cpp matrix.hpp)
add_executable(Matrix ${SOURCE_FILES})