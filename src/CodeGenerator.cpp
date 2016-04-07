//
// Created by ran on 4/6/16.
//

#include "CodeGenerator.h"
#include "Array.h"
#include "Record.h"
#include "Variable.h"

using std::string;
using std::dynamic_pointer_cast;
using std::vector;
using std::shared_ptr;

CodeGenerator::CodeGenerator(
        SymbolTable &symbolTable,
        const vector<shared_ptr<Instruction>> &instructions)
        : symbolTable(symbolTable) {
    calculateOffsets();
}

string CodeGenerator::getContent() {
    return stream.str();
}

void CodeGenerator::calculateOffsets() {
    // First, register the universal integer as 4 bytes.
    auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    typeSizes[universalInt] = 4;

    calculateScopeOffsets(symbolTable.getCurrentScope());

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

