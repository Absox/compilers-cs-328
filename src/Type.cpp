//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#include "Type.h"
#include "IntegerBox.h"

using std::string;
using std::shared_ptr;

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

shared_ptr<Box> Type::initializeBox() {
    return shared_ptr<Box>(new IntegerBox());
}

