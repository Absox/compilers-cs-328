//
// Created by ran on 4/25/16.
//

#include "CallExpression.h"


CallExpression::CallExpression(const std::shared_ptr<Call> &call) {
    this->call = call;
}


std::string CallExpression::getLabel() {
    return call->getLabel();
}

