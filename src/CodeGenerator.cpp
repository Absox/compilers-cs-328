//
// Created by ran on 4/6/16.
//

#include "CodeGenerator.h"
#include "Array.h"
#include "Record.h"
#include "NumberExpression.h"
#include "BinaryExpression.h"
#include "VariableLocation.h"
#include "Constant.h"
#include "Field.h"
#include "Index.h"


using std::string;
using std::dynamic_pointer_cast;
using std::vector;
using std::shared_ptr;
using std::endl;
using std::to_string;

CodeGenerator::CodeGenerator(
        SymbolTable &symbolTable,
        const vector<shared_ptr<Instruction>> &instructions)
        : symbolTable(symbolTable) {
    indentLevel = 0;
    labelCounter = 0;

    calculateOffsets();
    initializeProgram();
    processInstructions(instructions);
    finalizeProgram();
}

string CodeGenerator::getContent() {
    return stream.str();
}

void CodeGenerator::calculateOffsets() {
    // First, register the universal integer as 4 bytes.
    auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    typeSizes[universalInt] = 4;

    totalBytes = calculateScopeOffsets(symbolTable.getCurrentScope());

}

int CodeGenerator::calculateScopeOffsets(const shared_ptr<Scope> &scope) {
    int nextOffset = 0;

    vector<string> identifiers = scope->getIdentifiersSorted();
    for (unsigned c = 0; c < identifiers.size(); c++) {
        auto variable = dynamic_pointer_cast<Variable>(
                scope->getEntryInScope(identifiers[c]));
        if (variable != 0) {

            variable->setOffset(nextOffset);
            int size = getTypeSize(variable->getType());
            nextOffset += size;
        }
    }

    return nextOffset;
}


int CodeGenerator::getTypeSize(const std::shared_ptr<Type> &type) {

    // If found in the hashmap, store its size.
    if (typeSizes.find(type) != typeSizes.end()) {
        return typeSizes[type];
    }

    // Otherwise, we're dealing with an array or a record.
    auto array = dynamic_pointer_cast<Array>(type);
    auto record = dynamic_pointer_cast<Record>(type);

    if (array != 0) {
        int result = array->getLength() * getTypeSize(array->getType());
        typeSizes[array] = result;
        return result;
    } else {
        int result = calculateScopeOffsets(record->getScope());
        typeSizes[record] = result;
        return result;
    }

}


void CodeGenerator::initializeProgram() {
    indent();
    writeWithIndent(".arch armv6");
    writeWithIndent(".data");
    deindent();

    writeWithIndent("output_format:");
    indent();
    writeWithIndent(".asciz \"%d\\n\"");
    deindent();
    writeWithIndent("input_format");
    indent();
    writeWithIndent(".asciz \"%d\"");

    if (totalBytes > 0) {
        deindent();
        writeWithIndent("variables:");
        indent();
        writeWithIndent(".space " + to_string(totalBytes));
    }

    writeWithIndent(".text");
    writeWithIndent(".global main");
    deindent();

    writeWithIndent("main:");

    indent();
    writeWithIndent("push {fp, lr}");
    if (totalBytes > 0) writeWithIndent("ldr r11, =variables");
}


void CodeGenerator::finalizeProgram() {
    writeWithIndent("pop {fp, lr}");
    writeWithIndent("bx lr");
}


void CodeGenerator::processInstructions(
        const std::vector<std::shared_ptr<Instruction>> &instructions) {
    for (int c = 0; c < instructions.size(); c++) {
        processInstruction(instructions[c]);
    }
}

void CodeGenerator::processInstruction(
        const std::shared_ptr<Instruction> &instruction) {
    auto assign = dynamic_pointer_cast<Assign>(instruction);
    auto ifInstruction = dynamic_pointer_cast<IfInstruction>(instruction);
    auto repeat = dynamic_pointer_cast<Repeat>(instruction);
    auto read = dynamic_pointer_cast<Read>(instruction);
    auto write = dynamic_pointer_cast<Write>(instruction);
}


void CodeGenerator::processAssign(const shared_ptr<Assign> &assign) {

    // We need to know if we're assigning integers, arrays, or records.

}

void CodeGenerator::processIfInstruction(
        const shared_ptr<IfInstruction> &assign) {

}

void CodeGenerator::processRepeat(const shared_ptr<Repeat> &repeat) {

}

void CodeGenerator::processRead(const shared_ptr<Read> &read) {

    // Pop address we're reading to from stack.

}

void CodeGenerator::processWrite(const shared_ptr<Write> &write) {

    // Pop value we're writing from stack.
    // format goes into r0
    // value goes into r1
    // bl printf
    writeWithIndent("bl printf");
}


void CodeGenerator::resolveLocationOffset(
        const std::shared_ptr<Location> &location) {
    // Ultimately pushes an offset onto the stack.
}

std::shared_ptr<Type> CodeGenerator::getLocationType(
        const std::shared_ptr<Location> &location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        auto entry = symbolTable.getCurrentScope()->getEntry(
                variable->getIdentifier());
        auto variableEntry = dynamic_pointer_cast<Variable>(entry);
        auto constEntry = dynamic_pointer_cast<Constant>(entry);
        if (variableEntry != 0) {
            return variableEntry->getType();
        } else if (constEntry != 0) {
            return constEntry->getType();
        }
    } else if (index != 0) {
        auto parentArray = dynamic_pointer_cast<Array>(
                getLocationType(index->getLocation()));
        if (parentArray != 0) {
            return parentArray->getType();
        }

    } else if (field != 0) {
        auto parentRecord = dynamic_pointer_cast<Record>(
                getLocationType(field->getLocation()));
        if (parentRecord != 0) {
            auto fieldEntry = dynamic_pointer_cast<Variable>(
                    parentRecord->getScope()->getEntry(
                            field->getVariable()->getIdentifier()));
            if (fieldEntry != 0) {
                return fieldEntry->getType();
            }
        }
    }

    return 0;
}

void CodeGenerator::resolveExpressionValue(
        const std::shared_ptr<Expression> &expression) {
    // Ultimately pushes a numerical value onto the stack.

    auto number = dynamic_pointer_cast<NumberExpression>(expression);
    auto location = dynamic_pointer_cast<Location>(expression);
    auto binary = dynamic_pointer_cast<BinaryExpression>(expression);

    if (number != 0) {
        // Load the value into r3
        writeWithIndent("ldr r3, =" + to_string(number->getValue()));
        writeWithIndent("push {r3}");

    } else if (location != 0) {

        // We need to resolve the offset of the location first.
        resolveLocationOffset(location);
        // Pop the offset into r3.
        writeWithIndent("pop {r3}");
        // Calculate the address.
        writeWithIndent("add r0, r11, r3");
        // Load the value from the address.
        writeWithIndent("ldr r1, [r0]");
        // Push onto the stack.
        writeWithIndent("push {r1}");

    } else if (binary != 0) {

        // We need to pop two numbers, then do the operation, then push result.
        // Push right
        resolveExpressionValue(binary->getExpressionRight());
        // Push left
        resolveExpressionValue(binary->getExpressionLeft());

        // Pop left
        writeWithIndent("pop {r0}");
        // Pop right
        writeWithIndent("pop {r1}");

        if (binary->getOperation() == "+") {
            writeWithIndent("add r3, r0, r1");
        } else if (binary->getOperation() == "-") {
            writeWithIndent("sub r3, r0, r1");
        } else if (binary->getOperation() == "*") {
            writeWithIndent("mul r3, r0, r1");
        } else if (binary->getOperation() == "DIV") {
            // TODO Generate error for divide by zero.
            writeWithIndent("bl __aeabi_idiv");
            writeWithIndent("mov r3, r0");
        } else if (binary->getOperation() == "MOD") {
            writeWithIndent("bl __aaebi_idivmod");
            writeWithIndent("mov r3, r1");
            // TODO Generate error for modulus by zero.
        }
        writeWithIndent("push {r3}");

    }
}


void CodeGenerator::indent() {
    indentLevel++;
}

void CodeGenerator::deindent() {
    indentLevel--;
}

void CodeGenerator::writeWithIndent(const std::string &value) {
    for (int c = 0; c < indentLevel; c++) {
        stream << "    ";
    }
    stream << value << endl;
}


int CodeGenerator::getNextLabel() {
    return labelCounter++;
}

