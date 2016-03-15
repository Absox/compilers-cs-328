//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_EXPRESSION_H
#define COMPILERS_HWK5_EXPRESSION_H

#include "AbstractSyntaxTreeNode.h"

class Expression : public AbstractSyntaxTreeNode {
public:
    Expression();
    virtual ~Expression();
    virtual std::string getLabel() = 0;

};


#endif //COMPILERS_HWK5_EXPRESSION_H
