//
// Created by ran on 4/16/16.
//

#include "gtest/gtest.h"
#include "RegisterStack.h"

using std::cout;
using std::endl;

TEST(register_stack_test, init) {

    cout << endl << "Initial tests" << endl;
    RegisterStack stack(7, 4);
    EXPECT_EQ(stack.push(), 4);
    EXPECT_EQ(stack.pop(), 4);
}

TEST(register_stack_test, push_pop) {

    cout << endl << "Push & pop tests" << endl;
    RegisterStack stack(7, 4);
    EXPECT_EQ(stack.push(), 4);
    EXPECT_EQ(stack.push(), 5);
    EXPECT_EQ(stack.push(), 6);
    EXPECT_EQ(stack.push(), 7);
    EXPECT_EQ(stack.push(), 8);
    EXPECT_EQ(stack.push(), 9);
    EXPECT_EQ(stack.push(), 10);
    EXPECT_EQ(stack.push(), 4);
    EXPECT_EQ(stack.push(), 5);
    EXPECT_EQ(stack.push(), 6);
    EXPECT_EQ(stack.push(), 7);
    EXPECT_EQ(stack.push(), 8);
    EXPECT_EQ(stack.push(), 9);
    EXPECT_EQ(stack.push(), 10);

    EXPECT_EQ(stack.pop(), 10);
    EXPECT_EQ(stack.pop(), 9);
    EXPECT_EQ(stack.pop(), 8);
    EXPECT_EQ(stack.pop(), 7);
    EXPECT_EQ(stack.pop(), 6);
    EXPECT_EQ(stack.pop(), 5);
    EXPECT_EQ(stack.pop(), 4);
    EXPECT_EQ(stack.pop(), 10);



}