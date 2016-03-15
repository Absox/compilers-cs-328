//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

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

void Type::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent(name);
}
