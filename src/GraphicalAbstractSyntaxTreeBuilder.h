//
// Created by Ran on 3/15/2016.
//

#ifndef COMPILERS_HWK5_GRAPHICALABSTRACTSYNTAXTREEBUILDER_H
#define COMPILERS_HWK5_GRAPHICALABSTRACTSYNTAXTREEBUILDER_H

#include <vector>
#include <memory>
#include <sstream>

#include "SymbolTable.h"
#include "Instruction.h"
#include "Assign.h"
#include "IfInstruction.h"
#include "Read.h"
#include "Write.h"
#include "Repeat.h"
#include "VariableLocation.h"
#include "Index.h"
#include "Field.h"
#include "NumberExpression.h"
#include "BinaryExpression.h"

class GraphicalAbstractSyntaxTreeBuilder {
public:
    GraphicalAbstractSyntaxTreeBuilder(
            const std::vector<std::shared_ptr<Instruction>> instructions);
    virtual ~GraphicalAbstractSyntaxTreeBuilder();
    std::string getContent();
private:
    unsigned int processInstructions(
            const std::vector<std::shared_ptr<Instruction>>& instructions);
    unsigned int processInstruction(
            const std::shared_ptr<Instruction>& instruction);
    unsigned int processAssign(const std::shared_ptr<Assign>& assign);
    unsigned int processIf(const std::shared_ptr<IfInstruction>& ifInstruction);
    unsigned int processRepeat(const std::shared_ptr<Repeat>& repeat);
    unsigned int processRead(const std::shared_ptr<Read>& read);
    unsigned int processWrite(const std::shared_ptr<Write>& write);
    unsigned int processLocation(const std::shared_ptr<Location>& location);
    unsigned int processVariable(
            const std::shared_ptr<VariableLocation>& variable);
    unsigned int processIndex(const std::shared_ptr<Index>& index);
    unsigned int processField(const std::shared_ptr<Field>& field);
    unsigned int processCondition(const std::shared_ptr<Condition>& condition);
    unsigned int processExpression(
            const std::shared_ptr<Expression>& expression);
    unsigned int processNumber(const std::shared_ptr<NumberExpression>& number);
    unsigned int processBinary(const std::shared_ptr<BinaryExpression>& binary);

    unsigned int getNextNodeId();

    unsigned int nodeCounter;
    std::stringstream stream;
};


#endif //COMPILERS_HWK5_GRAPHICALABSTRACTSYNTAXTREEBUILDER_H
