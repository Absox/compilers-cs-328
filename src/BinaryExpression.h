//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_BINARYEXPRESSION_H
#define COMPILERS_HWK5_BINARYEXPRESSION_H

#include <memory>

#include "Expression.h"
#include "NumberExpression.h"

class BinaryExpression : public Expression {
public:
    BinaryExpression(const std::string& operation,
                     const std::shared_ptr<Expression>& expression_left,
                     const std::shared_ptr<Expression>& expression_right);
    virtual ~BinaryExpression();
    virtual std::string getType();

    std::shared_ptr<NumberExpression> fold();
    std::string getOperation() const;


private:
    std::string operation;
    std::shared_ptr<Expression> expression_left;
    std::shared_ptr<Expression> expression_right;
};


#endif //COMPILERS_HWK5_BINARYEXPRESSION_H
