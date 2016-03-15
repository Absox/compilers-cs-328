//
// Created by Ran on 3/12/2016.
// An Abstract syntax tree.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_ABSTRACTSYNTAXTREE_H
#define COMPILERS_HWK5_ABSTRACTSYNTAXTREE_H

#include <memory>
#include <vector>

#include "Instruction.h"

class AbstractSyntaxTree {
public:
    AbstractSyntaxTree();
    virtual ~AbstractSyntaxTree();

private:
    std::vector<std::shared_ptr<Instruction>> instructions;
};


#endif //COMPILERS_HWK5_ABSTRACTSYNTAXTREE_H
