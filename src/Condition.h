//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_CONDITION_H
#define COMPILERS_HWK5_CONDITION_H

#include <memory>

#include "AbstractSyntaxTreeNode.h"
#include "Expression.h"

class Condition : public AbstractSyntaxTreeNode {
public:
    Condition(const std::string& relation,
              const std::shared_ptr<Expression>& expression_left,
              const std::shared_ptr<Expression>& expression_right);
    virtual ~Condition();
    virtual std::string getLabel() override;

    std::shared_ptr<Condition> inverse();
    std::shared_ptr<Expression> getExpressionLeft();
    std::shared_ptr<Expression> getExpressionRight();
private:
    std::string relation;
    std::shared_ptr<Expression> expression_left;
    std::shared_ptr<Expression> expression_right;
};


#endif //COMPILERS_HWK5_CONDITION_H
