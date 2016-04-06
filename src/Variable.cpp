//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#include "Variable.h"

using std::string;
using std::shared_ptr;

Variable::Variable(const shared_ptr<Type>& type) {
    this->type = type;
    this->offset = -1;
}

Variable::~Variable() {

}

string Variable::getEntryType() const {
    return "VARIABLE";
}

void Variable::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent("VAR BEGIN");
    visitor.indent();
    visitor.writeWithIndent("type:");
    visitor.indent();
    type->acceptVisitor(visitor);
    visitor.deindent();
    visitor.deindent();
    visitor.writeWithIndent("END VAR");
}

std::shared_ptr<Type> Variable::getType() const {
    return type;
}
