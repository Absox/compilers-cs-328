//
// Created by Ran on 3/15/2016.
//

#include "AbstractSyntaxTreeBuilder.h"
#include "Variable.h"

#include <iostream>

using std::cout;

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
    for (unsigned long c = 0; c < instructions.size(); c++) {
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
    writeWithIndent("true =>");
    indent();
    processInstructions(ifInstruction->getInstructionsTrue());
    deindent();
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
    writeWithIndent("Binary (" + binary->getOperation() + "):");
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
    writeWithIndent("Variable:");
    writeWithIndent("variable =>");
    indent();
    writeWithIndent("VAR BEGIN");
    indent();
    writeWithIndent("type:");
    indent();
    processType(getLocationType(variable));
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
    writeWithIndent("VAR BEGIN");
    indent();
    writeWithIndent("type:");
    indent();
    processType(getLocationType(field));
    deindent();
    deindent();
    writeWithIndent("END VAR");
    deindent();
}

void AbstractSyntaxTreeBuilder::processType(const std::shared_ptr<Type> &type) {
    auto record = dynamic_pointer_cast<Record>(type);
    auto array = dynamic_pointer_cast<Array>(type);

    if (record != 0) {
        processRecord(record);
    } else if (array != 0) {
        processArray(array);
    } else {
        writeWithIndent(type->getName());
    }
}

void AbstractSyntaxTreeBuilder::processRecord(
        const std::shared_ptr<Record> &record) {
    auto scope = record->getScope();
    auto scopeIdentifiers = scope->getIdentifiersSorted();

    writeWithIndent("RECORD BEGIN");
    indent();
    writeWithIndent("SCOPE BEGIN");
    indent();

    for (unsigned int c = 0; c < scopeIdentifiers.size(); c++) {
        auto currentEntry = dynamic_pointer_cast<Variable>(
                scope->getEntry(scopeIdentifiers[c]));
        writeWithIndent(scopeIdentifiers[c] + " =>");
        indent();
        writeWithIndent("VAR BEGIN");
        indent();
        writeWithIndent("type:");
        indent();
        processType(currentEntry->getType());
        deindent();
        deindent();
        writeWithIndent("END VAR");
        deindent();

    }


    deindent();
    writeWithIndent("END SCOPE");
    deindent();
    writeWithIndent("END RECORD");
}

void AbstractSyntaxTreeBuilder::processArray(
        const std::shared_ptr<Array> &array) {
    writeWithIndent("ARRAY BEGIN");
    indent();
    writeWithIndent("type:");
    indent();
    processType(array->getType());
    deindent();
    writeWithIndent("length:");
    indent();
    writeWithIndent(to_string(array->getLength()));
    deindent();
    deindent();
    writeWithIndent("END ARRAY");

}

// Returns the type associated with a location.
shared_ptr<Type> AbstractSyntaxTreeBuilder::getLocationType(
        const shared_ptr<Location>& location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto field = dynamic_pointer_cast<Field>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    if (variable != 0) {
        auto variableEntry = dynamic_pointer_cast<Variable>(
                symbolTable.getCurrentScope()->getEntry(
                variable->getIdentifier()));
        return variableEntry->getType();
    } else if (field != 0) {
        auto recordEntry = dynamic_pointer_cast<Record>(
                getLocationType(field->getLocation()));
        auto fieldEntry = dynamic_pointer_cast<Variable>(
                recordEntry->getScope()->getEntry(
                        field->getVariable()->getIdentifier()));
        return fieldEntry->getType();
    } else if (index != 0) {
        auto arrayEntry = dynamic_pointer_cast<Array>(
                getLocationType(index->getLocation()));
        return arrayEntry->getType();
    }
    return 0;
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
