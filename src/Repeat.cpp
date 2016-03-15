//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Repeat.h"

using std::shared_ptr;
using std::vector;

Repeat::Repeat(const shared_ptr<Condition>& condition,
               const vector<shared_ptr<Instruction>>& instructions) {
    this->condition = condition;
    this->instructions = instructions;
}

Repeat::~Repeat() {

}

std::string Repeat::getLabel() {
    return "Repeat";
}
