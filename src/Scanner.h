/*
 * File:   Scanner.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 11:12 AM
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>

#include "Token.h"
#include "ScannerException.h"

class Scanner {
public:
    Scanner(const std::string& source);
    virtual ~Scanner();
    
    Token next() throw(ScannerException);
    std::vector<Token> all() throw(ScannerException);
    
    static bool isWhiteSpace(const char& c);
    static bool isKeyword(const std::string& s);
    static bool isStandaloneToken(const char& c);
    static bool isAssignmentOrInequality(const char& c);
    
private:
    unsigned int currentPosition;
    std::string source;
    bool isErrorState;
    bool canCallAll;
    
    char currentSourceCharacter() const;
    
    void processComment(unsigned int& commentDepth);
    void processIdentifier(std::string& tokenValue,
            unsigned int& startPosition, unsigned int& endPosition);
    void processInteger(std::string& tokenValue,
            unsigned int& startPosition, unsigned int& endPosition);
    void processStandaloneToken(std::string& tokenValue,
            unsigned int& startPosition, unsigned int& endPosition);
    void processAssignmentOrInequality(std::string& tokenValue,
            unsigned int& startPosition, unsigned int& endPosition);

};

#endif /* SCANNER_H */

