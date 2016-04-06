//
// Created by ran on 4/6/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATOR_H
#define COMPILERS_HWK5_CODEGENERATOR_H


#include "SymbolTable.h"
#include "Instruction.h"

class CodeGenerator {
public:
    CodeGenerator(
            const SymbolTable& symbolTable,
            const std::vector<std::shared_ptr<Instruction>>& instructions);

};


#endif //COMPILERS_HWK5_CODEGENERATOR_H
