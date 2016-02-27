//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_SYMBOLTABLE_H
#define COMPILERS_HWK4_SYMBOLTABLE_H

#include <memory>

#include "Scope.h"

class SymbolTable {
public:
    SymbolTable();
    virtual ~SymbolTable();

    std::shared_ptr<Scope> getCurrentScope() const;
private:
    std::shared_ptr<Scope> currentScope;
};


#endif //COMPILERS_HWK4_SYMBOLTABLE_H
