//
// Created by ran on 4/6/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATOR_H
#define COMPILERS_HWK5_CODEGENERATOR_H

#include <iostream>
#include <unordered_map>

#include "SymbolTable.h"
#include "Instruction.h"
#include "Location.h"
#include "Variable.h"
#include "IfInstruction.h"
#include "Assign.h"
#include "Write.h"
#include "Read.h"
#include "Repeat.h"
#include "CodeGenerationException.h"
#include "CodeGenerationMessage.h"

class CodeGenerator {
public:
    CodeGenerator(
            SymbolTable& symbolTable,
            const std::vector<std::shared_ptr<Instruction>>& instructions)
            throw (CodeGenerationException);
    std::string getContent();
private:
    unsigned int stackSize;
    const unsigned int numRegisters = 9;
    const unsigned int startRegister = 2;

    unsigned int pop();
    unsigned int push();

    SymbolTable& symbolTable;
    unsigned long long totalBytes;
    int indentLevel;
    int labelCounter;

    std::stringstream stream;
    std::unordered_map<std::shared_ptr<Type>, unsigned long long> typeSizes;

    void calculateOffsets() throw (CodeGenerationException);
    unsigned long long calculateScopeOffsets(
            const std::shared_ptr<Scope> &scope);
    unsigned long long getTypeSize(const std::shared_ptr<Type> &type);

    void initializeProgram();
    void finalizeProgram();
    void processInstructions(
            const std::vector<std::shared_ptr<Instruction>>& instructions);
    void processInstruction(const std::shared_ptr<Instruction>& instruction);

    void processAssign(const std::shared_ptr<Assign>& assign);
    void processIfInstruction(const std::shared_ptr<IfInstruction>& ifInstruction);
    void processRepeat(const std::shared_ptr<Repeat>& repeat);
    void processRead(const std::shared_ptr<Read>& read);
    void processWrite(const std::shared_ptr<Write>& write);

    CodeGenerationMessage resolveCondition(
            const std::shared_ptr<Condition>& condition);
    CodeGenerationMessage resolveLocationOffset(
            const std::shared_ptr<Location>& location);
    std::shared_ptr<Type> getLocationType(
            const std::shared_ptr<Location>& location);
    CodeGenerationMessage resolveExpressionValue(
            const std::shared_ptr<Expression>& expression)
        throw (CodeGenerationException);

    void indent();
    void deindent();
    void writeWithIndent(const std::string &value);

    int getNextLabelIndex();

    bool canImmediateValue(const long long int &value);
    bool powerOfTwo(const long long& value);
    unsigned int logTwo(const long long& value);
};


#endif //COMPILERS_HWK5_CODEGENERATOR_H
