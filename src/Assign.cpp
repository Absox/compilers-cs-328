//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Assign.h"

using std::string;
using std::shared_ptr;

Assign::Assign(const shared_ptr<Location> &location,
               const shared_ptr<Expression> &expression) {
    this->location = location;
    this->expression = expression;
}

Assign::~Assign() {

}

string Assign::getLabel() {
    return ":=";
}


shared_ptr<Location> Assign::getLocation() {
    return location;
}

shared_ptr<Expression> Assign::getExpression() {
    return expression;
}
