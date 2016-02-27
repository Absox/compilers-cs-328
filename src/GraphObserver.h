/*
 * File:   GraphObserver.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 21, 2016, 1:21 AM
 */

#ifndef GRAPHOBSERVER_H
#define GRAPHOBSERVER_H

#include <string>
#include <sstream>
#include <stack>

#include "Observer.h"
#include "GraphNode.h"

class GraphObserver : public Observer {
public:
    GraphObserver();
    virtual ~GraphObserver();
    void update() override;
    
    std::string getContent();
    
private:
    std::stringstream stream;
    std::stack<GraphNode*> parents;
    GraphNode* currentParent;
    GraphNode* lastNode;
    unsigned int nodeCounter;
    unsigned int currentIndent;
};

#endif /* GRAPHOBSERVER_H */

