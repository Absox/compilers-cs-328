//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#include "Record.h"

using std::shared_ptr;

Record::Record(const shared_ptr<Scope>& scope) : Type("RECORD") {
    this->scope = scope;
}

Record::~Record() {

}

shared_ptr<Scope> Record::getScope() const {
    return scope;
}

void Record::setScope(const shared_ptr<Scope>& scope) {
    this->scope = scope;
}

void Record::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent("RECORD BEGIN");
    visitor.indent();

    scope->acceptVisitor(visitor);

    visitor.deindent();
    visitor.writeWithIndent("END RECORD");
}
