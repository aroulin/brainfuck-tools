cmake_minimum_required(VERSION 2.8.11)
project(brainfuck_jit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp BrainfuckVM.cpp BrainfuckVM.h terminal_color.h)
add_executable(brainfuck_jit ${SOURCE_FILES})