//
// Created by Ran on 3/14/2016.
//

#include <memory>
#include <BinaryExpression.h>

#include "NumberExpression.h"

#include "gtest/gtest.h"

using std::shared_ptr;

TEST(binaryExpressionTest, expressionFoldingTest) {

    auto left = shared_ptr<NumberExpression>(new NumberExpression(5));
    auto right = shared_ptr<NumberExpression>(new NumberExpression(6));
    BinaryExpression addition("+", left, right);

    auto additionResult = addition.fold();
    ASSERT_TRUE(additionResult != 0);
    ASSERT_TRUE(additionResult->getValue() == 11);

    BinaryExpression multiplication("*", left, right);
    auto multiplicationResult = multiplication.fold();
    ASSERT_TRUE(multiplicationResult != 0);
    ASSERT_TRUE(multiplicationResult->getValue() == 30);

    BinaryExpression division("DIV", left, right);
    auto divisionResult = division.fold();
    ASSERT_TRUE(divisionResult != 0);
    ASSERT_TRUE(divisionResult->getValue() == 0);

    BinaryExpression mod("MOD", right, left);
    auto modResult = mod.fold();
    ASSERT_TRUE(modResult != 0);
    ASSERT_TRUE(modResult->getValue() == 1);

    BinaryExpression invalid("asdf", left, right);
    auto invalidResult = invalid.fold();
    ASSERT_FALSE(invalidResult != 0);

}