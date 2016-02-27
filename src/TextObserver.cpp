/*
 * File:   TextObserver.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 7:33 PM
 */

#include "TextObserver.h"

using std::string;
using std::endl;

#include <iostream>
using std::cout;

TextObserver::TextObserver() : Observer() {
}

TextObserver::~TextObserver() {
}

void TextObserver::update() {
    for (unsigned int c = 0; c < subject->getIndentLevel(); c++) {
        stream << "  "; // Two spaces per indent
    }
    if (subject->getState().isTerminal()) {
        stream << subject->getState().getToken()->toString() << endl;
    } else {
        stream << subject->getState().getValue() << endl;
    }
}

string TextObserver::getContent() {
    return stream.str();
}