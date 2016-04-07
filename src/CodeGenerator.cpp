//
// Created by ran on 4/6/16.
//

#include "CodeGenerator.h"
#include "Array.h"
#include "Record.h"
#include "Assign.h"
#include "IfInstruction.h"
#include "Repeat.h"
#include "Read.h"
#include "Write.h"


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
    writeWithIndent("ldr r11, =variables");
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

