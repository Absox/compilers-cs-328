//
// Created by Ran on 2/26/2016.
//

#include "Type.h"

using std::string;

Type::Type(const string& name) {
    this->name = name;
}

Type::~Type() {

}

string Type::getName() const {
    return name;
}

string Type::getEntryType() const {
    return getName();
}