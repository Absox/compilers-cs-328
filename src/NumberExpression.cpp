//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "NumberExpression.h"

NumberExpression::NumberExpression(const long long int &value) {
    this->value = value;
}

NumberExpression::~NumberExpression() {
}

std::string NumberExpression::getLabel() {
    return "Number";
}

long long int NumberExpression::getValue() const {
    return value;
}
