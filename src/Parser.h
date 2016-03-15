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
#include <memory>

#include "ParseState.h"
#include "ParseException.h"
#include "Scanner.h"
#include "Observer.h"
#include "Token.h"
#include "SymbolTable.h"
#include "Type.h"
#include "VariableLocation.h"
#include "Condition.h"
#include "Write.h"
#include "Read.h"
#include "Assign.h"
#include "IfInstruction.h"
#include "Repeat.h"

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
    std::vector<std::shared_ptr<Instruction>>& getAbstractSyntaxTree();

private:
    bool suppressContextErrors;
    bool suppressAbstractSyntaxErrors;
    SymbolTable symbolTable;
    std::vector<std::shared_ptr<Instruction>> ast;

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
    Token processToken(const std::string& type) throw(ParseException);
    
    void program() throw(ParseException);
    void declarations() throw(ParseException);
    void constDecl() throw(ParseException);
    void typeDecl() throw(ParseException);
    void varDecl() throw(ParseException);
    std::shared_ptr<Expression> expression() throw(ParseException);
    std::shared_ptr<Expression> term() throw(ParseException);
    std::shared_ptr<Expression> factor() throw(ParseException);
    std::shared_ptr<Location> designator() throw(ParseException);
    std::shared_ptr<Location> selector(const Token& identifier)
            throw(ParseException);
    std::vector<std::shared_ptr<Expression>> expressionList()
            throw(ParseException);
    std::shared_ptr<Type> type() throw(ParseException);
    std::vector<Token> identifierList() throw(ParseException);
    std::vector<std::shared_ptr<Instruction>> instructions()
            throw(ParseException);
    std::shared_ptr<Instruction> instruction() throw(ParseException);
    std::shared_ptr<Assign> assign() throw(ParseException);
    std::shared_ptr<IfInstruction> parseIf() throw(ParseException);
    std::shared_ptr<Condition> condition() throw(ParseException);
    std::shared_ptr<Repeat> repeat() throw(ParseException);
    std::shared_ptr<IfInstruction> parseWhile() throw(ParseException);
    std::shared_ptr<Read> read() throw(ParseException);
    std::shared_ptr<Write> write() throw(ParseException);

    std::shared_ptr<Type> findType(const std::string& identifier);
    bool isExpressionNumeric(const std::shared_ptr<Expression>& expression);
    std::shared_ptr<Type> getExpressionType(
            const std::shared_ptr<Expression>& expression);
    std::shared_ptr<Type> getLocationType(
            const std::shared_ptr<Location>& location);
    
};

#endif /* PARSER_H */

