//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Condition.h"

using std::string;
using std::shared_ptr;

Condition::Condition(const string &relation,
                     const shared_ptr<Expression> &expression_left,
                     const shared_ptr<Expression> &expression_right) {
    this->relation = relation;
    this->expression_left = expression_left;
    this->expression_right = expression_right;
}

Condition::~Condition() {

}

std::string Condition::getLabel() {
    return relation;
}

std::shared_ptr<Condition> Condition::inverse() {
    string inverseRelation;

    if (relation == "=") {
        inverseRelation = "#";
    } else if (relation == "#") {
        inverseRelation = "=";
    } else if (relation == "<") {
        inverseRelation = ">=";
    } else if (relation == "<=") {
        inverseRelation = ">";
    } else if (relation == ">") {
        inverseRelation = "<=";
    } else if (relation == ">=") {
        inverseRelation = "<";
    }

    return shared_ptr<Condition>(
            new Condition(inverseRelation, expression_left, expression_right));
}
