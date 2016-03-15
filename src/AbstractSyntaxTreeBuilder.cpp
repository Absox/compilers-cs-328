//
// Created by Ran on 3/15/2016.
//

#include "AbstractSyntaxTreeBuilder.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::endl;

AbstractSyntaxTreeBuilder::AbstractSyntaxTreeBuilder(
        const SymbolTable &symbolTable,
        const vector<shared_ptr<Instruction>>& instructions)
        : symbolTable(symbolTable) {
    indentLevel = 0;

    writeWithIndent("instructions =>");
    indent();
    processInstructions(instructions);
    deindent();
}

AbstractSyntaxTreeBuilder::~AbstractSyntaxTreeBuilder() {

}

string AbstractSyntaxTreeBuilder::getContent() {
    return stream.str();
}

void AbstractSyntaxTreeBuilder::processInstructions(
        const vector<shared_ptr<Instruction>> &instructions) {
    for (int c = 0; c < instructions.size(); c++) {
        processInstruction(instructions[c]);
    }
}

void AbstractSyntaxTreeBuilder::processInstruction(
        const shared_ptr<Instruction> &instruction) {
    auto assign = dynamic_pointer_cast<Assign>(instruction);
    auto ifInstruction = dynamic_pointer_cast<IfInstruction>(instruction);
    auto repeat = dynamic_pointer_cast<Repeat>(instruction);
    auto read = dynamic_pointer_cast<Read>(instruction);
    auto write = dynamic_pointer_cast<Write>(instruction);

    if (assign != 0) {
        processAssign(assign);
    } else if (ifInstruction != 0) {
        processIfInstruction(ifInstruction);
    } else if (repeat != 0) {
        processRepeat(repeat);
    } else if (read != 0) {
        processRead(read);
    } else if (write != 0) {
        processWrite(write);
    }
}

void AbstractSyntaxTreeBuilder::processAssign(
        const shared_ptr<Assign> &assign) {
    writeWithIndent("Assign:");
    writeWithIndent("location =>");
    indent();
    processLocation(assign->getLocation());
    deindent();
    writeWithIndent("expression =>");
    indent();
    processExpression(assign->getExpression());
    deindent();
}

void AbstractSyntaxTreeBuilder::processIfInstruction(
        const shared_ptr<IfInstruction> &ifInstruction) {
    writeWithIndent("If:");
    writeWithIndent("condition =>");
    indent();
    processCondition(ifInstruction->getCondition());
    deindent();
    writeWithIndent("instructions_true =>");
    indent();
    processInstructions(ifInstruction->getInstructionsTrue());
    deindent();
    // TODO verify handling of empty case
    auto instructionsFalse = ifInstruction->getInstructionsFalse();
    if (instructionsFalse.size() > 0) {
        writeWithIndent("instructions_false =>");
        indent();
        processInstructions(ifInstruction->getInstructionsFalse());
        deindent();
    }
}

void AbstractSyntaxTreeBuilder::processRepeat(
        const shared_ptr<Repeat> &repeat) {
    writeWithIndent("Repeat:");
    writeWithIndent("condition =>");
    indent();
    processCondition(repeat->getCondition());
    deindent();
    writeWithIndent("instructions =>");
    indent();
    processInstructions(repeat->getInstructions());
    deindent();
}

void AbstractSyntaxTreeBuilder::processRead(const std::shared_ptr<Read> &read) {
    writeWithIndent("Read:");
    writeWithIndent("location =>");
    indent();
    processLocation(read->getLocation());
    deindent();
}

void AbstractSyntaxTreeBuilder::processWrite(
        const std::shared_ptr<Write> &write) {
    writeWithIndent("Write:");
    writeWithIndent("expression =>");
    indent();
    processExpression(write->getExpression());
    deindent();
}

void AbstractSyntaxTreeBuilder::processCondition(
        const std::shared_ptr<Condition> &condition) {

}

void AbstractSyntaxTreeBuilder::processExpression(
        const std::shared_ptr<Expression> &expression) {

}

void AbstractSyntaxTreeBuilder::processNumber(
        const std::shared_ptr<NumberExpression> &number) {

}

void AbstractSyntaxTreeBuilder::processLocation(
        const std::shared_ptr<Location> &location) {

}

void AbstractSyntaxTreeBuilder::processBinary(
        const std::shared_ptr<BinaryExpression> &binary) {

}

void AbstractSyntaxTreeBuilder::processVariable(
        const std::shared_ptr<VariableLocation> &variable) {

}

void AbstractSyntaxTreeBuilder::processIndex(
        const std::shared_ptr<Index> &index) {

}

void AbstractSyntaxTreeBuilder::processField(
        const std::shared_ptr<Field> &field) {

}

void AbstractSyntaxTreeBuilder::writeWithIndent(const string &value) {
    for (unsigned int c = 0; c < indentLevel; c++) {
        stream << "  ";
    }
    stream << value << endl;
}

void AbstractSyntaxTreeBuilder::indent() {
    indentLevel++;
}

void AbstractSyntaxTreeBuilder::deindent() {
    indentLevel--;
}
