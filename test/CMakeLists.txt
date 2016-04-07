project(test)
set(TEST_SOURCES Hello.cpp)
add_executable(runUnitTests ${TEST_SOURCES})
target_link_libraries(runUnitTests gtest gtest_main)