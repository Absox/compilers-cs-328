//
// Created by ran on 4/25/16.
//

#ifndef COMPILERS_HWK5_CALLEXPRESSION_H
#define COMPILERS_HWK5_CALLEXPRESSION_H


#include "Expression.h"
#include "Call.h"

class CallExpression : public Expression {
public:
    virtual std::string getLabel() override;

    CallExpression(const std::shared_ptr<Call>& call);
private:
    std::shared_ptr<Call> call;

};


#endif //COMPILERS_HWK5_CALLEXPRESSION_H
