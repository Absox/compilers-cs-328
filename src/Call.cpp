//
// Created by ran on 4/25/16.
//

#include "Call.h"

using std::string;
using std::shared_ptr;

Call::Call(const string& identifier, const shared_ptr<Procedure>& procedure) {
    this->identifier = identifier;
    this->procedure = procedure;
}


std::string Call::getLabel() {
    return identifier;
}
