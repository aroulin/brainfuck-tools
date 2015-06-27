cmake_minimum_required(VERSION 2.8.11)
project(brainfuck_jit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O4")

include_directories($ENV{GMOCK_HOME}/include $ENV{GMOCK_HOME}/gtest/include)
link_directories($ENV{GMOCK_HOME}/mybuild $ENV{GMOCK_HOME}/gtest/mybuild)

set(SOURCE_FILES main.cpp BrainfuckVM.h BrainfuckVM.cpp BrainfuckVM_JIT.cpp)
set(TEST_FILES BrainfuckVM_Test.cpp BrainfuckVM_DebuggerTest.cpp)
add_executable(BrainfuckVM_Tests ${TEST_FILES} ${SOURCE_FILES})

target_link_libraries(BrainfuckVM_Tests pthread gmock gtest)