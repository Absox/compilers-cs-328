//
// Created by Ran on 3/25/16.
//

#ifndef COMPILERS_HWK5_INTERPRETER_H
#define COMPILERS_HWK5_INTERPRETER_H

#include <memory>
#include <vector>

#include "Instruction.h"
#include "SymbolTable.h"
#include "Environment.h"
#include "RuntimeException.h"

class Interpreter {

public:
    Interpreter(const SymbolTable& symbolTable,
                const std::vector<std::shared_ptr<Instruction>>& instructions)
        throw (RuntimeException);
    virtual ~Interpreter();
private:
    std::shared_ptr<Environment> environment;

    void buildEnvironment(const SymbolTable& symbolTable);
    void run(const std::vector<std::shared_ptr<Instruction>>& instructions)
            throw (RuntimeException);
};


#endif //COMPILERS_HWK5_INTERPRETER_H
