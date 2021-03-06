//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#include "Constant.h"

using std::string;
using std::to_string;
using std::shared_ptr;

Constant::Constant(const long long int &value, const shared_ptr<Type> &type) {
    this->value = value;
    this->type = type;
}

Constant::~Constant() {

}

void Constant::setValue(const long long int &value) {
    this->value = value;
}

long long int Constant::getValue() {
    return value;
}

string Constant::getEntryType() const {
    return "CONSTANT";
}

void Constant::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent("CONST BEGIN");
    visitor.indent();

    visitor.writeWithIndent("type:");
    visitor.indent();
    type->acceptVisitor(visitor);
    visitor.deindent();

    visitor.writeWithIndent("value:");
    visitor.indent();
    visitor.writeWithIndent(to_string(value));
    visitor.deindent();

    visitor.deindent();
    visitor.writeWithIndent("END CONST");
}

std::shared_ptr<Type> Constant::getType() const {
    return type;
}
