//
// Created by Ran on 2/27/2016.
//

#ifndef COMPILERS_HWK4_GRAPHICALSYMBOLTREE_H
#define COMPILERS_HWK4_GRAPHICALSYMBOLTREE_H

#include <sstream>
#include <memory>
#include <unordered_map>

#include "Entry.h"
#include "SymbolTable.h"
#include "Constant.h"

class GraphicalSymbolTree {
public:
    GraphicalSymbolTree(const SymbolTable& symbolTable);
    virtual ~GraphicalSymbolTree();

    std::string getContent();
private:
    int clusterCounter;
    int nodeCounter;

    std::unordered_map<std::shared_ptr<Entry>, int> nodeMapping;
    std::stringstream stream;

    void buildTree(const SymbolTable& symbolTable);
    int addEntry(const std::shared_ptr<Entry>& entry);
    int addScope(const std::shared_ptr<Scope>& scope);
};


#endif //COMPILERS_HWK4_GRAPHICALSYMBOLTREE_H
