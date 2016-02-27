/*
 * File:   GraphNode.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 21, 2016, 9:15 AM
 */

#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <string>

class GraphNode {
public:
    GraphNode(const unsigned int& id, const std::string& label,
            const bool& terminal);
    virtual ~GraphNode();
    
    std::string getLabel() const;
    bool isTerminal() const;
    unsigned int getId() const;
private:
    std::string label;
    bool terminal;
    unsigned int id;
};

#endif /* GRAPHNODE_H */

