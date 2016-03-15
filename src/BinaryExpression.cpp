//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "BinaryExpression.h"

using std::string;
using std::shared_ptr;
using std::dynamic_pointer_cast;

BinaryExpression::BinaryExpression(const string& operation,
                               const shared_ptr<Expression>& expression_left,
                               const shared_ptr<Expression>& expression_right) {
    this->operation = operation;
    this->expression_left = expression_left;
    this->expression_right = expression_right;

}

BinaryExpression::~BinaryExpression() {

}

string BinaryExpression::getLabel() {
    return operation;
}

shared_ptr<NumberExpression> BinaryExpression::fold() {

    shared_ptr<NumberExpression> left =
            dynamic_pointer_cast<NumberExpression>(expression_left);
    shared_ptr<NumberExpression> right =
            dynamic_pointer_cast<NumberExpression>(expression_right);

    if (left != 0 && right != 0) {
        if (operation == "+") {
            return shared_ptr<NumberExpression>(
                    new NumberExpression(left->getValue() + right->getValue()));
        } else if (operation == "-") {
            return shared_ptr<NumberExpression>(
                    new NumberExpression(left->getValue() - right->getValue()));
        } else if (operation == "*") {
            return shared_ptr<NumberExpression>(
                    new NumberExpression(left->getValue() * right->getValue()));
        } else if (operation == "DIV") {
            return shared_ptr<NumberExpression>(
                    new NumberExpression(left->getValue() / right->getValue()));
        } else if (operation == "MOD") {
            return shared_ptr<NumberExpression>(
                    new NumberExpression(left->getValue() % right->getValue()));
        }
    }

    return 0;

}

string BinaryExpression::getOperation() const {
    return operation;
}

std::shared_ptr<Expression> BinaryExpression::getExpressionLeft() const {
    return expression_left;
}

std::shared_ptr<Expression> BinaryExpression::getExpressionRight() const {
    return expression_right;
}
