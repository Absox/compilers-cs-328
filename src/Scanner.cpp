/*
 * File:   Scanner.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 11:12 AM
 */

#include "Scanner.h"

#include <iostream>

using std::string;
using std::vector;
using std::cerr;
using std::endl;

Scanner::Scanner(const string& source) {
    this->source = source;
    currentPosition = 0;
    isErrorState = false;
    canCallAll = true;
}

Scanner::~Scanner() {
}

Token Scanner::next() throw(ScannerException) {
    if (isErrorState) {
        throw ScannerException(currentPosition, "scanner in error state");
    }
    
    canCallAll = false;
    string tokenValue;
    
    unsigned int commentDepth = 0;
    unsigned int startPosition;
    unsigned int endPosition;
    
    while (currentPosition < source.length()) {
        if (isWhiteSpace(currentSourceCharacter())) {
            // Advance through all whitespace.
            currentPosition++;
        } else if (commentDepth > 0) {
            // If currently within a comment, process comments until no longer.
            processComment(commentDepth);
        } else if (isalpha(currentSourceCharacter())) {
            // Identifier or keyword.
            processIdentifier(tokenValue, startPosition, endPosition);
            if (isKeyword(tokenValue)) {
                return Token(tokenValue, tokenValue,
                        startPosition, endPosition);
            } else {
                return Token("identifier", tokenValue,
                        startPosition, endPosition);
            }
        } else if (isdigit(currentSourceCharacter())) {
            // Integer
            processInteger(tokenValue, startPosition, endPosition);
            return Token("integer", tokenValue, startPosition, endPosition);
            
        // Standalone character tokens.
        } else if (isStandaloneToken(currentSourceCharacter())) {
            processStandaloneToken(tokenValue, startPosition, endPosition);
            return Token(tokenValue, tokenValue, startPosition, endPosition);
            
        // Tokens that might be one or two characters
        } else if (isAssignmentOrInequality(currentSourceCharacter())) {
            processAssignmentOrInequality(tokenValue,
                    startPosition, endPosition);
            return Token(tokenValue, tokenValue, startPosition, endPosition);
            
        // Might be a comment, or just a standalone open parentheses
        } else if (currentSourceCharacter() == '(') {
            startPosition = currentPosition;
            endPosition = currentPosition;
            tokenValue += currentSourceCharacter();
            currentPosition++;
            
            if (currentSourceCharacter() == '*') {
                tokenValue = string("");
                commentDepth++;
                currentPosition++;
            } else {
                return Token(tokenValue, tokenValue,
                        startPosition, endPosition);
            }
        // Error : unrecognized character
        } else {
            string message = "unexpected character '";
            message += currentSourceCharacter();
            message += "' encountered";
            throw ScannerException(currentPosition, message);
        }
    }
    
    return Token("eof", "", currentPosition, currentPosition);
}

vector<Token> Scanner::all() throw(ScannerException) {
    if (!canCallAll) {
        throw ScannerException(currentPosition,
                "cannot call all() after next()");
    }
    
    vector<Token> result;
    
    try {
        Token currentToken = next();
        while (currentToken.getType() != "eof") {
            result.push_back(currentToken);
            currentToken = next();
        }
        result.push_back(currentToken);
    } catch (ScannerException& e) {
        cerr << "error: Scanner error incurred at position ";
        cerr << e.getPosition() << " - " << e.getMessage() << endl;
    }
    
    return result;
}

bool Scanner::isWhiteSpace(const char& c) {
    return c == '\n'
           || c == '\r'
           || c == '\t'
           || c == '\f'
           || c == ' ';
}

bool Scanner::isKeyword(const string& s) {
    return s == "PROGRAM"
           || s == "BEGIN"
           || s == "END"
           || s == "CONST"
           || s == "TYPE"
           || s == "VAR"
           || s == "ARRAY"
           || s == "OF"
           || s == "RECORD"
           || s == "DIV"
           || s == "MOD"
           || s == "IF"
           || s == "THEN"
           || s == "ELSE"
           || s == "REPEAT"
           || s == "UNTIL"
           || s == "WHILE"
           || s == "DO"
           || s == "READ"
           || s == "WRITE";
}

bool Scanner::isStandaloneToken(const char& c) {
    return c == ';' || c == '.' || c == '=' || c == '+' || c == '-' || c == ')'
           || c == '#' || c == '[' || c == ']' || c == ',' || c == '*';
}

bool Scanner::isAssignmentOrInequality(const char& c) {
    return c == ':' || c == '>' || c == '<';
}

char Scanner::currentSourceCharacter() const {
    return source[currentPosition];
}

void Scanner::processComment(unsigned int& commentDepth) {
    while (commentDepth > 0 && currentPosition < source.length()) {
        if (currentSourceCharacter() == '*') {
            if (++currentPosition < source.length()) {
                if (currentSourceCharacter() == ')') {
                    commentDepth--;
                }
                currentPosition++;
            }
        } else if (currentSourceCharacter() == '(') {
            if (++currentPosition < source.length()) {
                if (currentSourceCharacter() == '*') {
                    commentDepth++;
                }
                currentPosition++;
            }
        } else {
            currentPosition++;
        }
    }
}

void Scanner::processIdentifier(string& tokenValue, 
        unsigned int& startPosition, unsigned int& endPosition) {
    startPosition = currentPosition;
    
    while ((currentPosition < source.length())
            && isalnum(currentSourceCharacter())) {
        endPosition = currentPosition;
        tokenValue += currentSourceCharacter();
        currentPosition++;
    }
}

void Scanner::processInteger(string& tokenValue,
        unsigned int& startPosition, unsigned int& endPosition) {
    startPosition = currentPosition;
    
    while ((currentPosition < source.length())
            && isdigit(currentSourceCharacter())) {
        endPosition = currentPosition;
        tokenValue += currentSourceCharacter();
        currentPosition++;
    }
}

void Scanner::processStandaloneToken(string& tokenValue,
        unsigned int& startPosition, unsigned int& endPosition) {
    startPosition = currentPosition;
    endPosition = currentPosition;
    tokenValue += currentSourceCharacter();
    currentPosition++;
}

void Scanner::processAssignmentOrInequality(string& tokenValue,
        unsigned int& startPosition, unsigned int& endPosition) {
    startPosition = currentPosition;
    endPosition = currentPosition;
    tokenValue += currentSourceCharacter();
    currentPosition++;

    if (currentSourceCharacter() == '=') {
        tokenValue += currentSourceCharacter();
        endPosition = currentPosition;
        currentPosition++;
    }
}