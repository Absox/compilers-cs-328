//
// Created by Ran on 3/25/16.
//

#include "Interpreter.h"

#include <iostream>

using std::vector;
using std::shared_ptr;
using std::dynamic_pointer_cast;

using std::cout;
using std::cin;
using std::endl;


Interpreter::Interpreter(const SymbolTable &symbolTable,
                         const vector<shared_ptr<Instruction>> &instructions) {
    buildEnvironment(symbolTable);
    run(instructions);
}

Interpreter::~Interpreter() {

}

void Interpreter::buildEnvironment(const SymbolTable &symbolTable) {
    this->environment = shared_ptr<Environment>(
            new Environment(symbolTable.getCurrentScope()->buildEnvironment()));
}

void Interpreter::run(const vector<shared_ptr<Instruction>> &instructions)
    throw (RuntimeException) {


}

