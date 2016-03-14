include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_definitions(-std=gnu++0x)

add_executable(runUnitTests scopeTest.cpp symbolTableTest.cpp scopeVisitorTest.cpp binaryExpressionTest.cpp)

target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests compilers5_lib)