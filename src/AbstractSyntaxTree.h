//
// Created by Ran on 3/12/2016.
// An Abstract syntax tree.
//

#ifndef COMPILERS_HWK5_ABSTRACTSYNTAXTREE_H
#define COMPILERS_HWK5_ABSTRACTSYNTAXTREE_H

#include <memory>

#include "AbstractSyntaxTreeNode.h"

class AbstractSyntaxTree {
public:
    AbstractSyntaxTree();
    virtual ~AbstractSyntaxTree();

private:
    std::shared_ptr<AbstractSyntaxTreeNode> root;
};


#endif //COMPILERS_HWK5_ABSTRACTSYNTAXTREE_H
