cmake_minimum_required(VERSION 3.2)
project(test)

set(TEST_SOURCES Hello.cpp RegisterStack.cpp RegisterStack.h RegisterStackTest.cpp ImmediateValueTest.cpp)
#add_definitions(-std=gnu++0x)

add_executable(runUnitTests ${TEST_SOURCES})
target_link_libraries(runUnitTests gtest gtest_main pthread)