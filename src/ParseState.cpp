/*
 * File:   ParseState.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 8:19 PM
 */

#include "ParseState.h"

using std::string;

ParseState::ParseState(const string& value, Token* token) {
    this->value = value;
    this->token = token;
}

ParseState::~ParseState() {
}

string ParseState::getValue() const {
    return value;
}

bool ParseState::isTerminal() const {
    return token != 0; // If token is non-null, then this is terminal.
}

Token* ParseState::getToken() const {
    return token;
}