add_subdirectory(lib/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(TEST_SOURCES helloWorldTest.cpp)
add_executable(runTests ${TEST_SOURCES})
target_link_libraries(runTests gtest gtest_main)