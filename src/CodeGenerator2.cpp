//
// Created by ran on 4/16/16.
//

#include "CodeGenerator2.h"
#include "Variable.h"
#include "Array.h"
#include "Record.h"

using std::string;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::dynamic_pointer_cast;

CodeGenerator2::CodeGenerator2(
        SymbolTable &symbolTable,
        const vector<std::shared_ptr<Instruction>> &instructions)
        throw (CodeGenerationException) : symbolTable(symbolTable)  {

    indentLevel = 0;
    stackSize = 0;
    totalBytes = 0;
    labelCounter = 0;

    calculateOffsets();

}

void CodeGenerator2::calculateOffsets() throw (CodeGenerationException) {
    // First, register the universal integer as 4 bytes.
    auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    typeSizes[universalInt] = 4;

    totalBytes = calculateScopeOffsets(symbolTable.getCurrentScope());
    if (totalBytes > 4294967296) {
        throw CodeGenerationException(
                "More memory requested than can be addressed!");
    }
}

unsigned long long CodeGenerator2::calculateScopeOffsets(
        const shared_ptr<Scope> &scope) {
    unsigned long long nextOffset = 0;

    vector<string> identifiers = scope->getIdentifiersSorted();
    for (unsigned c = 0; c < identifiers.size(); c++) {
        auto variable = dynamic_pointer_cast<Variable>(
                scope->getEntryInScope(identifiers[c]));
        if (variable != 0) {

            variable->setOffset(nextOffset);
            unsigned long long size = getTypeSize(variable->getType());
            nextOffset += size;
        }
    }

    return nextOffset;
}

unsigned long long CodeGenerator2::getTypeSize(const shared_ptr<Type> &type) {

    // If found in the hashmap, store its size.
    if (typeSizes.find(type) != typeSizes.end()) {
        return typeSizes[type];
    }

    // Otherwise, we're dealing with an array or a record.
    auto array = dynamic_pointer_cast<Array>(type);
    auto record = dynamic_pointer_cast<Record>(type);

    if (array != 0) {
        unsigned long long result =
                array->getLength() * getTypeSize(array->getType());
        typeSizes[array] = result;
        return result;
    } else {
        unsigned long long result = calculateScopeOffsets(record->getScope());
        typeSizes[record] = result;
        return result;
    }

}

std::string CodeGenerator2::getContent() {
    return stream.str();
}


void CodeGenerator2::indent() {
    indentLevel++;
}


void CodeGenerator2::deindent() {
    indentLevel--;
}


void CodeGenerator2::write(const string &value) {
    for (unsigned int c = 0; c < indentLevel; c++) {
        stream << "  ";
    }
    stream << value << endl;
}


/**
 * If we want to push something into the register stack,
 * this tells us which register to put our value into.
 */
unsigned int CodeGenerator2::push() {
    auto result = stackSize++ % numRegisters + startRegister;
    if (stackSize % numRegisters == numRegisters / 2
        && (stackSize / numRegisters > 0)) {
        stream << "push {r" << numRegisters / 2 + startRegister
            << "-r" << numRegisters + startRegister - 1 << "}" << endl;
    } else if (stackSize % numRegisters == 0 && stackSize > 0) {
        stream << "push {r" << startRegister << "-r"
            << numRegisters / 2 + startRegister - 1 << "}" << endl;
    }
    return result;
}

/**
 * If we want to pop something from the register stack, this tells us
 * which register our value is in.
 */
unsigned int CodeGenerator2::pop() {
    if (stackSize % numRegisters == numRegisters / 2
        && stackSize / numRegisters > 0) {
        stream << "pop {r" << numRegisters / 2 + startRegister
            << "-r" << numRegisters + startRegister - 1 << "}" << endl;

    } else if (stackSize % numRegisters == 0 && stackSize > 0) {
        stream << "pop {r" << startRegister << "-r"
            << numRegisters / 2 + startRegister - 1 << "}" << endl;
    }

    auto result = --stackSize % numRegisters + startRegister;
    return result;
}


unsigned int CodeGenerator2::getNextLabelIndex() {
    return labelCounter++;
}

