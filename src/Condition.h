//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_CONDITION_H
#define COMPILERS_HWK5_CONDITION_H

#include <memory>

#include "AbstractSyntaxTreeNode.h"
#include "Expression.h"

class Condition : public AbstractSyntaxTreeNode {
public:
    Condition();
    virtual ~Condition();
    virtual std::string getType();
private:
    std::shared_ptr<Expression> expression_left;
    std::shared_ptr<Expression> expression_right;
};


#endif //COMPILERS_HWK5_CONDITION_H
