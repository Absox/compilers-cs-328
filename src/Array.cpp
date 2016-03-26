//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#include "Array.h"
#include "ArrayBox.h"

using std::to_string;
using std::shared_ptr;

Array::Array(const std::shared_ptr<Type> &type, const int &length)
        : Type("ARRAY") {
    this->type = type;
    this->length = length;
}

Array::~Array() {

}


shared_ptr<Type> Array::getType() const {
    return type;
}


int Array::getLength() const {
    return length;
}

void Array::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent("ARRAY BEGIN");
    visitor.indent();

    visitor.writeWithIndent("type:");
    visitor.indent();
    type->acceptVisitor(visitor);
    visitor.deindent();

    visitor.writeWithIndent("length:");
    visitor.indent();
    visitor.writeWithIndent(to_string(length));
    visitor.deindent();

    visitor.deindent();
    visitor.writeWithIndent("END ARRAY");
}


std::shared_ptr<Box> Array::initializeBox() {
    return shared_ptr<Box>(new ArrayBox(length, type));
}

