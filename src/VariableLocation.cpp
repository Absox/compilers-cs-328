//
// Created by Ran on 3/14/2016.
//

#include "VariableLocation.h"

using std::string;

VariableLocation::VariableLocation(const string& identifier) {
    this->identifier = identifier;
}

VariableLocation::~VariableLocation() {

}

std::string VariableLocation::getLabel() {
    return identifier;
}
