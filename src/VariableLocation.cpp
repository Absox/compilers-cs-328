//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "VariableLocation.h"

using std::string;

VariableLocation::VariableLocation(
        const string& identifier, const bool& local) {
    this->identifier = identifier;
    this->local = local;
}

VariableLocation::~VariableLocation() {

}

string VariableLocation::getLabel() {
    return identifier;
}

string VariableLocation::getIdentifier() const {
    return identifier;
}
