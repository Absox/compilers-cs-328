//
// Created by Ran on 3/15/2016.
//

#include "AbstractSyntaxTreeBuilder.h"
#include "Variable.h"
#include "Record.h"
#include "Array.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::endl;
using std::to_string;

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
    // TODO verify handling of instructions
    writeWithIndent("If:");
    writeWithIndent("condition =>");
    indent();
    processCondition(ifInstruction->getCondition());
    deindent();
    writeWithIndent("true =>");
    indent();
    processInstructions(ifInstruction->getInstructionsTrue());
    deindent();
    // TODO verify handling of empty case
    auto instructionsFalse = ifInstruction->getInstructionsFalse();
    if (instructionsFalse.size() > 0) {
        writeWithIndent("false =>");
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
        const shared_ptr<Condition> &condition) {
    // TODO verify handling of conditions
    writeWithIndent("Condition (" + condition->getLabel() + "):");
    writeWithIndent("left =>");
    indent();
    processExpression(condition->getExpressionLeft());
    deindent();
    writeWithIndent("right =>");
    indent();
    processExpression(condition->getExpressionRight());
    deindent();
}

void AbstractSyntaxTreeBuilder::processExpression(
        const std::shared_ptr<Expression> &expression) {
    auto number = dynamic_pointer_cast<NumberExpression>(expression);
    auto location = dynamic_pointer_cast<Location>(expression);
    auto binary = dynamic_pointer_cast<BinaryExpression>(expression);

    if (number != 0) {
        processNumber(number);
    } else if (location != 0) {
        processLocation(location);
    } else if (binary != 0) {
        processBinary(binary);
    }
}

void AbstractSyntaxTreeBuilder::processNumber(
        const std::shared_ptr<NumberExpression> &number) {
    writeWithIndent("Number:");
    writeWithIndent("value =>");
    indent();
    writeWithIndent("CONST BEGIN");
    indent();
    writeWithIndent("type:");
    indent();
    writeWithIndent("INTEGER");
    deindent();
    writeWithIndent("value:");
    indent();
    writeWithIndent(to_string(number->getValue()));
    deindent();
    deindent();
    writeWithIndent("END CONST");
    deindent();
}

void AbstractSyntaxTreeBuilder::processLocation(
        const shared_ptr<Location> &location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        processVariable(variable);
    } else if (index != 0) {
        processIndex(index);
    } else if (field != 0) {
        processField(field);
    }
}

void AbstractSyntaxTreeBuilder::processBinary(
        const shared_ptr<BinaryExpression> &binary) {
    // TODO verify formatting
    writeWithIndent("Binary (" + binary->getOperation() + ")");
    writeWithIndent("left =>");
    indent();
    processExpression(binary->getExpressionLeft());
    deindent();
    writeWithIndent("right =>");
    indent();
    processExpression(binary->getExpressionRight());
    deindent();
}

void AbstractSyntaxTreeBuilder::processVariable(
        const shared_ptr<VariableLocation> &variable) {
    auto varEntry = dynamic_pointer_cast<Variable>(
            symbolTable.getCurrentScope()->getEntry(variable->getIdentifier()));
    auto type = varEntry->getType();

    writeWithIndent("Variable:");
    writeWithIndent("variable =>");
    indent();
    writeWithIndent("VAR BEGIN");
    indent();
    writeWithIndent("type:");
    indent();
    processType(type);
    deindent();
    deindent();
    writeWithIndent("END VAR");
    deindent();

}

void AbstractSyntaxTreeBuilder::processIndex(
        const shared_ptr<Index> &index) {
    writeWithIndent("Index:");
    writeWithIndent("location =>");
    indent();
    processLocation(index->getLocation());
    deindent();

    writeWithIndent("expression =>");
    indent();
    processExpression(index->getExpression());
    deindent();
}

void AbstractSyntaxTreeBuilder::processField(
        const shared_ptr<Field> &field) {
    writeWithIndent("Field:");
    writeWithIndent("location =>");
    indent();
    processLocation(field->getLocation());
    deindent();

    writeWithIndent("variable =>");
    indent();
    // TODO PROCESS RECORD MEMBERS
    deindent();
}

void AbstractSyntaxTreeBuilder::processType(const std::shared_ptr<Type> &type) {
    auto record = dynamic_pointer_cast<Record>(type);
    auto array = dynamic_pointer_cast<Array>(type);

    if (record != 0) {

    } else if (array != 0) {

    } else {
        writeWithIndent(type->getName());
    }
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

