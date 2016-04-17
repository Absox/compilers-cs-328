//
// Created by ran on 4/16/16.
//

#include "CodeGenerator2.h"

using std::string;
using std::endl;

CodeGenerator2::CodeGenerator2(
        SymbolTable &symbolTable,
        const std::vector<std::shared_ptr<Instruction>> &instructions)
        throw (CodeGenerationException) : symbolTable(symbolTable)  {

    indentLevel = 0;
    stackSize = 0;
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


unsigned int CodeGenerator2::push() {
    auto result = stackSize++ % numRegisters + startRegister;

    // We're half full. Push the upper half stack.
    if (stackSize % numRegisters == 5 && stackSize / numRegisters > 0) {
        // push {r2-r5}
    }

    // We're full. Push the lower half stack.
    if (stackSize % numRegisters == 0) {

    }

    return result;
}


unsigned int CodeGenerator2::pop() {

    if (stackSize % numRegisters == numRegisters - 1) {

    }
    return 0;
}

