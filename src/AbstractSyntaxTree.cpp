//
// Created by Ran on 3/12/2016.
// rliu14@jhu.edu

#include "AbstractSyntaxTree.h"


AbstractSyntaxTree::AbstractSyntaxTree() {

}

AbstractSyntaxTree::~AbstractSyntaxTree() {

}

void AbstractSyntaxTree::setInstructions(
        const std::vector<std::shared_ptr<Instruction>> &instructions) {
    this->instructions = instructions;
}
