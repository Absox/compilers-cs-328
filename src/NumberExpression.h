//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_NUMBEREXPRESSION_H
#define COMPILERS_HWK5_NUMBEREXPRESSION_H


#include "Expression.h"

class NumberExpression : public Expression {
public:
    NumberExpression(const int& value);
    virtual ~NumberExpression();
    virtual std::string getLabel();

    int getValue() const;
private:
    int value;
};


#endif //COMPILERS_HWK5_NUMBEREXPRESSION_H
