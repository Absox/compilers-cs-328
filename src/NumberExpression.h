//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_NUMBEREXPRESSION_H
#define COMPILERS_HWK5_NUMBEREXPRESSION_H


#include "Expression.h"

class NumberExpression : public Expression {
public:
    NumberExpression(const long long &value);
    virtual ~NumberExpression();
    virtual std::string getLabel();

    long long getValue() const;
private:
    long long value;
};


#endif //COMPILERS_HWK5_NUMBEREXPRESSION_H
