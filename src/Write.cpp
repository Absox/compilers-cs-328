//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Write.h"

using std::string;
using std::shared_ptr;

Write::Write(const shared_ptr<Expression>& expression) {
    this->expression = expression;
}

Write::~Write() {

}

string Write::getLabel() {
    return "Write";
}

shared_ptr<Expression> Write::getExpression() {
    return expression;
}
