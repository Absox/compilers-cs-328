//
// Created by ran on 4/7/16.
//

#include "gtest/gtest.h"

using std::cout;
using std::endl;

TEST(helloworld_test, helloworld) {
    int x = 0;
    cout << endl << "Hello, world!" << endl;
    EXPECT_EQ(1,1);
}

TEST(helloworld_tes, failure) {
    EXPECT_EQ(0, 1);
}