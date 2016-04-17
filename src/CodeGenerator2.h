//
// Created by ran on 4/16/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATOR2_H
#define COMPILERS_HWK5_CODEGENERATOR2_H


#include "SymbolTable.h"
#include "Instruction.h"
#include "CodeGenerationException.h"
#include "Assign.h"
#include "IfInstruction.h"
#include "Repeat.h"
#include "Read.h"
#include "Write.h"

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
    std::unordered_map<std::shared_ptr<Type>, unsigned long long> typeSizes;

    unsigned int indentLevel;
    unsigned int labelCounter;

    unsigned int stackSize;
    const unsigned int numRegisters = 9;
    const unsigned int startRegister = 2;

    unsigned long long totalBytes;

    void indent();
    void deindent();
    void write(const std::string& value);

    unsigned int getNextLabelIndex();

    unsigned int push();
    unsigned int pop();

    void calculateOffsets() throw (CodeGenerationException);
    unsigned long long calculateScopeOffsets(
            const std::shared_ptr<Scope>& scope);
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

    void resolveCondition(const std::shared_ptr<Condition>& condition);
    void resolveLocationOffset(const std::shared_ptr<Location>& location);
    std::shared_ptr<Type> getLocationType(
            const std::shared_ptr<Location>& location);
    void resolveExpressionValue(const std::shared_ptr<Expression>& expression)
            throw (CodeGenerationException);

};


#endif //COMPILERS_HWK5_CODEGENERATOR2_H
