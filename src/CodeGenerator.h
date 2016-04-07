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

class CodeGenerator {
public:
    CodeGenerator(
            SymbolTable& symbolTable,
            const std::vector<std::shared_ptr<Instruction>>& instructions);
    std::string getContent();
private:
    SymbolTable& symbolTable;
    int totalBytes;
    int indentLevel;
    int labelCounter;

    std::stringstream stream;
    std::unordered_map<std::shared_ptr<Type>, int> typeSizes;

    void calculateOffsets();
    int calculateScopeOffsets(const std::shared_ptr<Scope> &scope);
    int getTypeSize(const std::shared_ptr<Type>& type);

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

    void resolveCondition(const std::shared_ptr<Condition>& condition);
    void resolveLocationOffset(const std::shared_ptr<Location>& location);
    std::shared_ptr<Type> getLocationType(
            const std::shared_ptr<Location>& location);
    void resolveExpressionValue(const std::shared_ptr<Expression>& expression);

    void indent();
    void deindent();
    void writeWithIndent(const std::string& value);

    int getNextLabelIndex();
};


#endif //COMPILERS_HWK5_CODEGENERATOR_H
