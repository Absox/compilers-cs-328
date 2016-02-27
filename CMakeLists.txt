cmake_minimum_required(VERSION 3.2)
project(compilers_hwk4)

add_definitions(-std=c++11)

include_directories(src)

set(SOURCE_FILES main.cpp)
add_executable(sc ${SOURCE_FILES})

add_subdirectory(test)
add_subdirectory(src)

target_link_libraries(sc compilers4_lib)
