//
// Created by Ran on 3/14/2016.
//

#include "NumberExpression.h"

NumberExpression::NumberExpression(const int &value) {
    this->value = value;
}

NumberExpression::~NumberExpression() {
}

std::string NumberExpression::getLabel() {
    return "Number";
}

int NumberExpression::getValue() const {
    return value;
}
