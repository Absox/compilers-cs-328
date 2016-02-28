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
    addScope(programScope);
}

int GraphicalSymbolTree::addEntry(const shared_ptr<Entry>& entry) {
    int nodeId = nodeCounter++;
    nodeMapping[entry] = nodeId;

    if (dynamic_pointer_cast<Constant>(entry) != 0) {
        // Create new constant object : diamond
        shared_ptr<Constant> constant = dynamic_pointer_cast<Constant>(entry);
        stream << nodeId << "[shape=diamond, label=\"" << constant->getValue()
            << "\"]" << endl;

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
            stream << nodeId << "[shape=rectangle style=rounded label=\""
                << "Array\\nlength: " << array->getLength() << "\"]"<< endl;

            if (nodeMapping.find(array->getType()) == nodeMapping.end()) {
                addEntry(array->getType());
            }
            stream << nodeId << " -> " << nodeMapping[array->getType()] << endl;

        } else if (dynamic_pointer_cast<Record>(entry) != 0) {
            shared_ptr<Record> record = dynamic_pointer_cast<Record>(entry);
            stream << nodeId << "[shape=rectangle style=rounded label=\""
                << "Record" << "\"]";
            int scopeNodeId = addScope(record->getScope());
            stream << nodeId << " -> " << scopeNodeId << endl;

        } else {
            shared_ptr<Type> type = dynamic_pointer_cast<Type>(entry);
            stream << nodeId << "[shape=rectangle style=rounded label=\""
                << type->getName() << "\"]" << endl;
        }
    }

    return nodeId;
}

int GraphicalSymbolTree::addScope(const shared_ptr<Scope>& scope) {
    int scopeClusterId = clusterCounter++;
    int scopeNodeId = nodeCounter++;
    stream << "subgraph cluster" << scopeClusterId << " {" << endl;
    stream << "shape=rectangle" << endl;
    stream << "node[style=filled color=white]" << endl;
    stream << scopeNodeId << "[label=\"\"]" << endl;

    vector<string> identifiers = scope->getIdentifiersSorted();
    for (unsigned int c = 0; c < identifiers.size(); c++) {
        int currentNodeId = nodeCounter++;
        stream << currentNodeId << "[label=\"" << identifiers[c] << "\"]"
            << endl;
    }

    stream << "{" << endl << "rank=same" << endl;
    for (unsigned int c = 0; c < identifiers.size(); c++) {
        stream << scopeNodeId + c << " -> " << scopeNodeId + c + 1
            << "[style=invis]" << endl;
    }
    stream << "}" << endl;
    stream << "}" << endl;

    for (unsigned int c = 0; c < identifiers.size(); c++) {
        shared_ptr<Entry> entry = scope->getEntry(identifiers[c]);
        if (nodeMapping.find(entry) == nodeMapping.end()) {
            addEntry(entry);
        }
        stream << scopeNodeId + 1 + c << " -> " << nodeMapping[entry] << endl;
    }
    return scopeNodeId;
}