cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_COMPILER g++)

project(compilers_hwk5)

add_definitions(-std=c++11)

include_directories(src)
set(SOURCE_FILES main.cpp)
add_executable(sc ${SOURCE_FILES})

add_subdirectory(src)

target_link_libraries(sc compilers5_lib)
