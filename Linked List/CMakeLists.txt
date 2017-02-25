cmake_minimum_required(VERSION 3.5)
project(Linked_List)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -Wold-style-cast -pedantic")

set(L_LIST_SOURCES linkedlist.h)
set(SOURCE_FILES main.cpp )
add_executable(Linked_List ${SOURCE_FILES} ${L_LIST_SOURCES})
add_executable(Tests linkedlist.cpp catch.hpp  ${L_LIST_SOURCES})
add_executable(Fix fixthis.cpp)