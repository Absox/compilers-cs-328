//
// Created by ran on 4/16/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATOR2_H
#define COMPILERS_HWK5_CODEGENERATOR2_H


#include "SymbolTable.h"
#include "Instruction.h"
#include "CodeGenerationException.h"

class CodeGenerator2 {
public:
    CodeGenerator2(
            SymbolTable& symbolTable,
            const std::vector<std::shared_ptr<Instruction>>& instructions
    ) throw (CodeGenerationException);
    std::string getContent();

private:
    SymbolTable& symbolTable;
    std::stringstream stream;

    unsigned int indentLevel;

    unsigned int stackSize;
    const unsigned int numRegisters = 9;
    const unsigned int startRegister = 2;
    const unsigned int endRegister = 10;

    void indent();
    void deindent();
    void write(const std::string& value);

    unsigned int push();
    unsigned int pop();

};


#endif //COMPILERS_HWK5_CODEGENERATOR2_H
