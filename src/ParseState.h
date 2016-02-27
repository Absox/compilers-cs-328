/*
 * File:   ParseState.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 8:19 PM
 */

#ifndef PARSESTATE_H
#define PARSESTATE_H

#include <string>

#include "Token.h"

class ParseState {
public:
    ParseState(const std::string& value, Token* token = 0);
    virtual ~ParseState();
    
    std::string getValue() const;
    bool isTerminal() const;
    Token* getToken() const;
private:
    std::string value;
    Token* token;
};

#endif /* PARSESTATE_H */

