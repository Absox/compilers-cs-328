/*
 * File:   GraphNode.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 21, 2016, 9:15 AM
 */

#include "GraphNode.h"

using std::string;

GraphNode::GraphNode(const unsigned int& id, const string& label,
        const bool& terminal) {
    this->id = id;
    this->label = label;
    this->terminal = terminal;
}

GraphNode::~GraphNode() {
}

string GraphNode::getLabel() const {
    return label;
}

bool GraphNode::isTerminal() const {
    return terminal;
}

unsigned int GraphNode::getId() const {
    return id;
}