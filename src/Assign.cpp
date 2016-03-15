//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Assign.h"

Assign::Assign(const std::shared_ptr<Location> &location,
               const std::shared_ptr<Expression> &expression) {
    this->location = location;
    this->expression = expression;
}

Assign::~Assign() {

}

std::string Assign::getLabel() {
    return ":=";
}


