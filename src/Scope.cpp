//
// Created by Ran on 2/26/2016.
//

#include "Scope.h"
#include "Type.h"

#include <iostream>

using std::cout;
using std::shared_ptr;
using std::string;

Scope::Scope(const shared_ptr<Scope>& outer) {
    this->outer = outer;
}

Scope::~Scope() {

}


bool Scope::scopeContainsEntry(const string &identifier) const {
    return identifiers.find(identifier) != identifiers.end();
}

bool Scope::containsEntry(const string &identifier) const {
    if (scopeContainsEntry(identifier)) {
        return true;
    } else {
        if (outer != 0) {
            return outer->containsEntry(identifier);
        } else {
            return false;
        }
    }
}

void Scope::addEntry(const string &identifier, const std::shared_ptr<Entry> &entry) {
    identifiers[identifier] = entry;
}

std::shared_ptr<Entry> Scope::getEntry(const string &identifier) const {
    if (scopeContainsEntry(identifier)) {
        return identifiers.at(identifier);
    } else {
        if (outer != 0) {
            return outer->getEntry(identifier);
        } else {
            return 0;
        }
    }
}

std::shared_ptr<Scope> Scope::getOuter() const {
    return outer;
}

void Scope::setOuter(const std::shared_ptr<Scope>& outer) {
    this->outer = outer;
}

Scope* Scope::createUniverse() {
    Scope* universe = new Scope;
    universe->addEntry("INTEGER", shared_ptr<Entry>(new Type("INTEGER")));
    return universe;
}

void Scope::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent("BEGIN SCOPE");
    visitor.indent();

    for (auto iterator = identifiers.begin();
         iterator != identifiers.end(); ++iterator) {
        shared_ptr<Entry> currentEntry = iterator->second;
        currentEntry->acceptVisitor(visitor);
    }

    visitor.deindent();
    visitor.writeWithIndent("END SCOPE");
}
