//
// Created by Ran on 2/26/2016.
//

#include "Variable.h"

using std::string;
using std::shared_ptr;

Variable::Variable(const shared_ptr<Type>& type) {
    this->type = type;
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
