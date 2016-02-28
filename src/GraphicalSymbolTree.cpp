//
// Created by Ran on 2/27/2016.
//

#include "GraphicalSymbolTree.h"
#include "SymbolTable.h"
#include "Constant.h"
#include "Variable.h"
#include "Array.h"
#include "Record.h"

#include <vector>

using std::vector;
using std::string;
using std::endl;
using std::dynamic_pointer_cast;
using std::shared_ptr;

GraphicalSymbolTree::GraphicalSymbolTree(const SymbolTable& symbolTable) {
    stream << "digraph program {" << endl;

    clusterCounter = 0;
    nodeCounter = 0;

    buildTree(symbolTable);
}

GraphicalSymbolTree::~GraphicalSymbolTree() {
}

string GraphicalSymbolTree::getContent() {
    return stream.str() + "}";
}

void GraphicalSymbolTree::buildTree(const SymbolTable &symbolTable) {
    shared_ptr<Scope> programScope = symbolTable.getCurrentScope();
    shared_ptr<Scope> universeScope = programScope->getOuter();

    vector<string> identifiers = universeScope->getIdentifiersSorted();
    for (unsigned int c = 0; c < identifiers.size(); c++) {
        addEntry(universeScope->getEntry(identifiers[c]));
    }
}

int GraphicalSymbolTree::addEntry(const shared_ptr<Entry>& entry) {
    int nodeId = nodeCounter++;
    nodeMapping[entry] = nodeId;

    if (dynamic_pointer_cast<Constant>(entry) != 0) {
        // Create new constant object : diamond
        shared_ptr<Constant> constant = dynamic_pointer_cast<Constant>(entry);
        stream << nodeId << "[shape=diamond, label=\"" << constant->getValue()
            << "\"" << endl;

        if (nodeMapping.find(constant->getType()) == nodeMapping.end()) {
            addEntry(constant->getType());
        }
        stream << nodeId << " -> " << nodeMapping[constant->getType()]
            << endl;

    } else if (dynamic_pointer_cast<Variable>(entry) != 0) {
        // Create new variable object : circle
        shared_ptr<Variable> variable = dynamic_pointer_cast<Variable>(entry);
        stream << nodeId << "[shape=circle, label=\"\"]" << endl;

        if (nodeMapping.find(variable->getType()) == nodeMapping.end()) {
            addEntry(variable->getType());
        }
        stream << nodeId << " -> " << nodeMapping[variable->getType()] << endl;

    } else if (dynamic_pointer_cast<Type>(entry) != 0) {
        // Now, we have to check if we're an array, record, or just a type.
        if (dynamic_pointer_cast<Array>(entry) != 0) {
            shared_ptr<Array> array = dynamic_pointer_cast<Array>(entry);
            stream << nodeId << "[shape=rectangle style=rounded label=\"
                << "Array\nlength: " << array->getLength() << "\"]"<< endl;

            if (nodeMapping.find(array->getType()) == nodeMapping.end()) {
                addEntry(array->getType());
            }
            stream << nodeId << " -> " << nodeMapping[array->getType()] << endl;

        } else if (dynamic_pointer_cast<Record>(entry) != 0) {

        } else {
            shared_ptr<Type> type = dynamic_pointer_cast<Type>(entry);
            stream << nodeId << "[shape=rectangle style=rounded label=\""
                << type->getName() << "\"]" << endl;

        }

    }

    return nodeId;
}