//
// Created by ran on 3/25/16.
//

#include "Environment.h"

using std::unordered_map;
using std::string;
using std::shared_ptr;

Environment::Environment(const unordered_map<string, shared_ptr<Box>>& scope) {
    this->scope = scope;
}

Environment::~Environment() {

}


std::shared_ptr<Box> Environment::getBox(const std::string &identifier) {
    return scope[identifier];
}

