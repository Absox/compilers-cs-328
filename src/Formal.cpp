//
// Created by ran on 4/25/16.
//

#include "Formal.h"

using std::string;
using std::shared_ptr;

Formal::Formal(const string &identifier,
               const shared_ptr <Type> &type) {
    this->identifier = identifier;
    this->type = type;
}

