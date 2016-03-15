//
// Created by Ran on 3/15/2016.
//

#ifndef COMPILERS_HWK5_ABSTRACTSYNTAXTREEBUILDER_H
#define COMPILERS_HWK5_ABSTRACTSYNTAXTREEBUILDER_H

#include <sstream>

#include "SymbolTable.h"
#include "Instruction.h"
#include "Assign.h"
#include "IfInstruction.h"
#include "Repeat.h"
#include "Read.h"
#include "Write.h"
#include "Field.h"
#include "Index.h"
#include "BinaryExpression.h"

class AbstractSyntaxTreeBuilder {
public:
    AbstractSyntaxTreeBuilder(const SymbolTable& symbolTable,
              const std::vector<std::shared_ptr<Instruction>>& instructions);
    virtual ~AbstractSyntaxTreeBuilder();
    std::string getContent();
private:
    const SymbolTable& symbolTable;
    int indentLevel;
    std::stringstream stream;

    void processInstructions(
            const std::vector<std::shared_ptr<Instruction>> &instructions);
    void processInstruction(const std::shared_ptr<Instruction>& instruction);
    void processAssign(const std::shared_ptr<Assign>& assign);
    void processIfInstruction(
            const std::shared_ptr<IfInstruction>& ifInstruction);
    void processRepeat(const std::shared_ptr<Repeat>& repeat);
    void processRead(const std::shared_ptr<Read>& read);
    void processWrite(const std::shared_ptr<Write>& write);
    void processCondition(const std::shared_ptr<Condition>& condition);
    void processExpression(const std::shared_ptr<Expression>& expression);
    void processNumber(const std::shared_ptr<NumberExpression>& number);
    void processLocation(const std::shared_ptr<Location>& location);
    void processBinary(const std::shared_ptr<BinaryExpression>& binary);
    void processVariable(const std::shared_ptr<VariableLocation>& variable);
    void processIndex(const std::shared_ptr<Index>& index);
    void processField(const std::shared_ptr<Field>& field);

    void writeWithIndent(const std::string& value);
    void indent();
    void deindent();
};


#endif //COMPILERS_HWK5_ABSTRACTSYNTAXTREEBUILDER_H
