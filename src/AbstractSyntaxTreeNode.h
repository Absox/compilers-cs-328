//
// Created by Ran on 3/10/2016.
// Abstract base class for nodes in the abstract syntax tree.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK4_ABSTRACTSYNTAXTREENODE_H
#define COMPILERS_HWK4_ABSTRACTSYNTAXTREENODE_H

#include <string>

class AbstractSyntaxTreeNode {
public:
    AbstractSyntaxTreeNode();
    virtual ~AbstractSyntaxTreeNode();
    virtual std::string getLabel() = 0;
private:

};


#endif //COMPILERS_HWK4_ABSTRACTSYNTAXTREENODE_H
