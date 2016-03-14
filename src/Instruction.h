//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_INSTRUCTION_H
#define COMPILERS_HWK5_INSTRUCTION_H

#include <memory>

#include "AbstractSyntaxTreeNode.h"
#include "Expression.h"

class Instruction : public AbstractSyntaxTreeNode {
public:
    Instruction();
    virtual ~Instruction();
    virtual std::string getType() = 0;
private:
    std::shared_ptr<Expression> expression;
};


#endif //COMPILERS_HWK5_INSTRUCTION_H
