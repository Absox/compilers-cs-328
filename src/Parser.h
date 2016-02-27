/*
 * File:   Parser.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 5:27 PM
 */

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

#include "ParseState.h"
#include "ParseException.h"
#include "Scanner.h"
#include "Observer.h"
#include "Token.h"
#include "SymbolTable.h"

class Observer;

class Parser {
public:
    Parser(Scanner* scanner, const bool& suppressContextErrors = true);
    virtual ~Parser();
    
    void parse() throw(ParseException);
    void addObserver(Observer* o);
    void notifyObservers();
    
    ParseState getState() const;
    unsigned int getIndentLevel() const;

    SymbolTable& getSymbolTable();
    
private:
    bool suppressContextErrors;
    SymbolTable symbolTable;

    ParseState state;
    Token currentToken;
    unsigned int indentLevel;
    
    Scanner* scanner;
    std::vector<Observer*> observers;
    
    void indent();
    void deindent();
    void setState(const std::string& value);
    
    Token& nextToken() throw(ParseException);
    bool match(const std::string& type);
    Token& processToken(const std::string& type) throw(ParseException);
    
    void program() throw(ParseException);
    void declarations() throw(ParseException);
    void constDecl() throw(ParseException);
    void typeDecl() throw(ParseException);
    void varDecl() throw(ParseException);
    void expression() throw(ParseException);
    void term() throw(ParseException);
    void factor() throw(ParseException);
    void designator() throw(ParseException);
    void selector() throw(ParseException);
    void expressionList() throw(ParseException);
    void type() throw(ParseException);
    void identifierList() throw(ParseException);
    void instructions() throw(ParseException);
    void instruction() throw(ParseException);
    void assign() throw(ParseException);
    void parseIf() throw(ParseException);
    void condition() throw(ParseException);
    void repeat() throw(ParseException);
    void parseWhile() throw(ParseException);
    void read() throw(ParseException);
    void write() throw(ParseException);
    
};

#endif /* PARSER_H */

