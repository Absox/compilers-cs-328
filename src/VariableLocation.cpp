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

string VariableLocation::getLabel() {
    return identifier;
}

string VariableLocation::getIdentifier() const {
    return identifier;
}
