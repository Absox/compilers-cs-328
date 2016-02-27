/*
 * File:   GraphObserver.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 21, 2016, 1:21 AM
 */

#include "GraphObserver.h"

using std::string;
using std::endl;

GraphObserver::GraphObserver() {
    stream << "digraph program {" << endl;
    nodeCounter = 0;
    currentParent = 0;
    lastNode = 0;
    currentIndent = 0;
}

GraphObserver::~GraphObserver() {
    // Delete the last node seen, as that is a leaf node.
    if (lastNode != 0) delete lastNode;
    // Delete the current parent node, if it exists.
    if (currentParent != 0) delete currentParent;
    // Deallocate any nodes remaining on the stack.
    while (!parents.empty()) {
        delete parents.top();
        parents.pop();
    }
}

void GraphObserver::update() {
    unsigned int indent = subject->getIndentLevel();
    bool terminal = subject->getState().isTerminal();

    string label;
    if (terminal) {
        label = subject->getState().getToken()->getValue();
    } else {
        label = subject->getState().getValue();
    }

    // Allocate the current node.
    // Oh god raw pointers I hope this doesn't memory leak.
    // Probably could have stack allocated everything but let's do this.
    GraphNode* currentNode = new GraphNode(nodeCounter++, label, terminal);
    stream << currentNode->getId() << " [label=\"" << label << "\" shape=\"";
    if (terminal) {
        stream << "diamond";
    } else {
        stream << "rectangle";
    }
    stream << "\"]" << endl;
    
    // Went down one level.
    if (indent > currentIndent) {
        // If there is a current parent, push that onto the stack.
        // Make the last node seen our current parent.
        if (currentParent != 0) {
            parents.push(currentParent);
        }
        currentParent = lastNode;
    
    // Went up at least one level.
    } else if (indent < currentIndent) {
        // Check the number of times we need to go up.
        int numLevels = currentIndent - indent;
        for (int c = 0; c < numLevels; c++) {
            // Last node seen was a leaf node, deallocate it.
            if (lastNode != 0) delete lastNode;
            lastNode = 0;
            // Our current parent is a leaf node, deallocate it.
            if (currentParent != 0) delete currentParent;
            // Pop a parent from the stack, if there is one.
            if (!parents.empty()) {
                currentParent = parents.top();
                parents.pop();
            // If empty, then we've gone all the way to the top.
            } else {
                currentParent = 0;
            }
        }
        
        
        
    // Same level
    } else {
        // Last node seen was a leaf node, deallocate it.
        if (lastNode != 0) delete lastNode;
    }
    
    // If there is a parent, we have to output the relation.
    if (currentParent != 0) {
        stream << currentParent->getId() << " -> "
                << currentNode->getId() << endl;
    }
    
    lastNode = currentNode;
    currentIndent = indent;
}

string GraphObserver::getContent() {
    return stream.str() + "}";
}
