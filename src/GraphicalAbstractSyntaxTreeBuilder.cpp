//
// Created by Ran on 3/15/2016.
//

#include "GraphicalAbstractSyntaxTreeBuilder.h"

using std::vector;
using std::shared_ptr;
using std::string;

GraphicalAbstractSyntaxTreeBuilder::GraphicalAbstractSyntaxTreeBuilder(
        const SymbolTable &symbolTable,
        const vector<shared_ptr<Instruction>> instructions) {
    nodeCounter = 0;
    processInstructions(instructions);
}

GraphicalAbstractSyntaxTreeBuilder::~GraphicalAbstractSyntaxTreeBuilder() {
}

string GraphicalAbstractSyntaxTreeBuilder::getContent() {
    return stream.str();
}

// Returns id of first instruction.
unsigned int GraphicalAbstractSyntaxTreeBuilder::processInstructions(
        const vector<shared_ptr<Instruction>> &instructions) {
    // TODO implement
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::getNextNodeId() {
    return nodeCounter++;
}
