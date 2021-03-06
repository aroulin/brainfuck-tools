cmake_minimum_required(VERSION 2.8.11)
project(brainfuck_jit)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

include_directories(${PROJECT_SOURCE_DIR})
set(SOURCE_FILES Brainfuck.h Brainfuck_Interpreter.cpp x86_64/Brainfuck_JIT.cpp x86_64/Brainfuck_Compiler.cpp)
add_executable(Brainfuck main.cpp ${SOURCE_FILES})

if (DEFINED ENV{GMOCK_HOME})
    include_directories($ENV{GMOCK_HOME}/include $ENV{GMOCK_HOME}/gtest/include)
    link_directories($ENV{GMOCK_HOME}/mybuild $ENV{GMOCK_HOME}/gtest/mybuild)
    set(TEST_FILES tests/BrainfuckVM_Test.cpp tests/BrainfuckVM_DebuggerTest.cpp)
    add_executable(Brainfuck_Tests tests/tests.cpp ${TEST_FILES} ${SOURCE_FILES})
    target_link_libraries(Brainfuck_Tests gmock gtest pthread)
endif()