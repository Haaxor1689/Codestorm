cmake_minimum_required(VERSION 3.5)
project(Matrix)

IF(MSVS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /wd4251 /wd4275")
ELSE(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic")
ENDIF(MSVC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SOURCE_FILES main.cpp matrix.hpp)
add_executable(Matrix ${SOURCE_FILES})