//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#include "ScopeVisitor.h"

using std::endl;

ScopeVisitor::ScopeVisitor() {
    indentLevel = 0;
}

ScopeVisitor::~ScopeVisitor() {

}

void ScopeVisitor::indent() {
    indentLevel++;
}

void ScopeVisitor::deindent() {
    indentLevel--;
}

void ScopeVisitor::write(const std::string &content) {
    stream << content;
}

std::string ScopeVisitor::getContent() {
    return stream.str();
}

int ScopeVisitor::getIndentLevel() const {
    return indentLevel;
}

void ScopeVisitor::writeWithIndent(const std::string &content) {
    for (int c = 0; c < indentLevel; c++) {
        stream << "  ";
    }
    stream << content << endl;
}
