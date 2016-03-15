//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK4_SCOPEVISITOR_H
#define COMPILERS_HWK4_SCOPEVISITOR_H

#include <sstream>
#include <string>

class ScopeVisitor {
public:
    ScopeVisitor();
    virtual ~ScopeVisitor();

    void indent();
    void deindent();
    void write(const std::string& content);
    void writeWithIndent(const std::string& content);
    std::string getContent();
    int getIndentLevel() const;
private:
    int indentLevel;
    std::stringstream stream;
};


#endif //COMPILERS_HWK4_SCOPEVISITOR_H
