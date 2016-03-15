//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "IfInstruction.h"

using std::shared_ptr;
using std::vector;

IfInstruction::IfInstruction(const shared_ptr<Condition> &condition,
                 const vector<shared_ptr<Instruction>> &instructions_true,
                 const vector<shared_ptr<Instruction>> &instructions_false) {
    this->condition = condition;
    this->instructions_true = instructions_true;
    this->instructions_false = instructions_false;
}


IfInstruction::~IfInstruction() {

}

std::string IfInstruction::getLabel() {
    return "If";
}
