//
// Created by Ran on 2/26/2016.
//

#include "SymbolTable.h"

using std::shared_ptr;

SymbolTable::SymbolTable() : currentScope(Scope::createUniverse()) {
}

SymbolTable::~SymbolTable() {

}

shared_ptr<Scope> SymbolTable::getCurrentScope() const {
    return currentScope;
}