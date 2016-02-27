//
// Created by Ran on 2/26/2016.
//

#include "Scope.h"
#include "Type.h"

#include <iostream>
#include <algorithm>

using std::cout;
using std::shared_ptr;
using std::string;
using std::vector;

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

    vector<string> sortedIdentifiers = getIdentifiersSorted();
    for (unsigned int c = 0; c < sortedIdentifiers.size(); c++) {

        visitor.writeWithIndent(sortedIdentifiers[c] + "=>");
        visitor.indent();
        shared_ptr<Entry> currentEntry = identifiers[sortedIdentifiers[c]];
        currentEntry->acceptVisitor(visitor);
        visitor.deindent();

    }

    visitor.deindent();
    visitor.writeWithIndent("END SCOPE");
}

vector<string> Scope::getIdentifiersSorted() {
    vector<string> result;

    for (auto iterator = identifiers.begin();
         iterator != identifiers.end(); ++iterator) {
        result.push_back(iterator->first);
    }

    sort(result.begin(), result.end(), wayToSort);

    return result;
}

/**
 * Sort descending by ascii characters, and then by string lengths.
 */
bool Scope::wayToSort(string a, string b) {
    unsigned int minLength = a.size();
    if (b.size() < a.size()) minLength = b.size();

    for (unsigned int c = 0; c < minLength; c++) {
        if (a[c] < b[c]) return true;
    }
    if (a.size() < minLength) return true;
    return false;
}