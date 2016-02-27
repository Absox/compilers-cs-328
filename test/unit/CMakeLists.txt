include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runUnitTests scopeTest.cpp symbolTableTest.cpp)

target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests compilers4_lib)