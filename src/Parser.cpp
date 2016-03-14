/*
 * File:   Parser.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 5:27 PM
 */

#include "Parser.h"
#include "ScannerException.h"
#include "Constant.h"
#include "Array.h"
#include "Variable.h"
#include "Record.h"

#include <memory>

using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;
using std::dynamic_pointer_cast;

Parser::Parser(Scanner* scanner, const bool& suppressContextErrors,
               const bool& suppressAbstractSyntaxErrors)
        : state("init"), currentToken("", "", 0, 0) {
    this->scanner = scanner;
    this->suppressContextErrors = suppressContextErrors;
    this->suppressAbstractSyntaxErrors = suppressAbstractSyntaxErrors;
    indentLevel = 0;
}

Parser::~Parser() {
}

void Parser::parse() throw(ParseException) {
    program();
}

void Parser::addObserver(Observer* o) {
    o->setSubject(this);
    observers.push_back(o);
}

void Parser::notifyObservers() {
    for (unsigned int c = 0; c < observers.size(); c++) {
        observers[c]->update();
    }
}

ParseState Parser::getState() const {
    return state;
}

unsigned int Parser::getIndentLevel() const {
    return indentLevel;
}

void Parser::indent() {
    indentLevel++;
}

void Parser::deindent() {
    indentLevel--;
}

void Parser::setState(const string& value) {
    state = ParseState(value);
    notifyObservers();
}

Token& Parser::nextToken() throw(ParseException) {
    try {
        currentToken = scanner->next();
        return currentToken;
    } catch(ScannerException& e) {
        throw ParseException("Scanner terminated with error: "
                + e.getMessage() + " at position "
                + to_string(e.getPosition()));
    }
}

bool Parser::match(const string& type) {
    return currentToken.getType() == type;
}

Token Parser::processToken(const string& type) throw(ParseException) {
    if (currentToken.getType() == type) {
        state = ParseState("terminal", &currentToken);
        notifyObservers();

        Token result = currentToken;
        nextToken();
        return result;
    } else {
        throw ParseException("Token '" + type + "' expected; actual: "
                + currentToken.toString());
    }
}

void Parser::program() throw(ParseException) {
    shared_ptr<Scope> programScope(new Scope(symbolTable.getCurrentScope()));
    symbolTable.setCurrentScope(programScope);

    setState("Program");
    indent();
    
    nextToken();
    
    processToken("PROGRAM");
    Token programName = processToken("identifier");
    processToken(";");
    
    declarations();
    
    if (match("BEGIN")) {
        processToken("BEGIN");
        instructions();
    }
    
    processToken("END");
    Token endProgramName = processToken("identifier");
    processToken(".");

    if (!suppressContextErrors
        && programName.getValue() != endProgramName.getValue()) {
        throw ParseException("Context violation: "
                             + programName.toString() + " expected; actual: "
                             + endProgramName.toString());
    }
    
    if (!currentToken.isEof()) {
        throw ParseException("End of file expected, actual: "
                + currentToken.toString());
    }
    
    deindent();
}

void Parser::declarations() throw(ParseException) {
    setState("Declarations");
    indent();
    
    while (match("CONST") || match("TYPE") || match("VAR")) {
        if (match("CONST")) {
            constDecl();
        } else if (match("TYPE")) {
            typeDecl();
        } else {
            varDecl();
        }
    }
    deindent();
}

void Parser::constDecl() throw(ParseException) {
    setState("ConstDecl");
    indent();
    
    processToken("CONST");
    
    while (match("identifier")) {
        Token identifier = processToken("identifier");

        if (!suppressContextErrors) {
            if (symbolTable.getCurrentScope()->scopeContainsEntry(
                    identifier.getValue())) {
                throw ParseException("Context violation: duplicate "
                                     + identifier.toString());
            } else {
                shared_ptr<Entry> constant(new Constant(5, findType("INTEGER")));
                symbolTable.getCurrentScope()->addEntry(
                        identifier.getValue(), constant);
            }
        }

        processToken("=");
        expression();
        processToken(";");
    }
    
    deindent();
}

void Parser::typeDecl() throw(ParseException) {
    setState("TypeDecl");
    indent();
    
    processToken("TYPE");
    
    while(match("identifier")) {
        Token identifier = processToken("identifier");
        processToken("=");
        shared_ptr<Type> linkedType = type();
        processToken(";");

        if (!suppressContextErrors) {
            if (symbolTable.getCurrentScope()->scopeContainsEntry(
                    identifier.getValue())) {
                throw ParseException("Context violation: duplicate "
                                     + identifier.toString());
            } else {
                symbolTable.getCurrentScope()->addEntry(
                        identifier.getValue(), linkedType);
            }

        }
    }
    
    deindent();
}

void Parser::varDecl() throw(ParseException) {
    setState("VarDecl");
    indent();
    
    processToken("VAR");

    while (match("identifier")) {
        vector<Token> identifiers = identifierList();
        processToken(":");
        shared_ptr<Type> varType = type();
        processToken(";");

        if (!suppressContextErrors) {
            for (unsigned int c = 0; c < identifiers.size(); c++) {
                if (symbolTable.getCurrentScope()->scopeContainsEntry(
                        identifiers[c].getValue())) {
                    throw ParseException("Context violation: duplicate "
                                         + identifiers[c].toString());
                } else {
                    shared_ptr<Entry> varEntry(new Variable(varType));
                    symbolTable.getCurrentScope()->addEntry(
                            identifiers[c].getValue(), varEntry);
                }

            }
        }
    }
    
    
    deindent();
}

void Parser::expression() throw(ParseException) {
    setState("Expression");
    indent();
    
    if (match("+")) {
        processToken("+");
    } else if (match("-")) {
        processToken("-");
    }
    
    term();
    
    while (match("+") || match("-")) {
        if (match("+")) {
            processToken("+");
        } else if (match("-")) {
            processToken("-");
        }
        
        term();
    }
    
    deindent();
}

void Parser::term() throw(ParseException) {
    setState("Term");
    indent();
    
    factor();
    while (match("*") || match("DIV") || match("MOD")) {
        if (match("*")) {
            processToken("*");
        } else if (match("DIV")) {
            processToken("DIV");
        } else {
            processToken("MOD");
        }
        factor();
    }
    
    deindent();
}

void Parser::factor() throw(ParseException) {
    setState("Factor");
    indent();
    
    if (match("integer")) {
        processToken("integer");
    } else if (match("identifier")) {
        designator();
    } else if (match("(")) {
        processToken("(");
        expression();
        processToken(")");
        
    } else {
        throw ParseException("Factor expected; actual: "
                + currentToken.toString());
    }
    
    deindent();
}

void Parser::designator() throw(ParseException) {
    setState("Designator");
    indent();
    
    processToken("identifier");
    selector();
    
    deindent();
}

void Parser::selector() throw(ParseException) {
    setState("Selector");
    indent();
    
    while (match("[") || match(".")) {
        if (match("[")) {
            processToken("[");
            expressionList();
            processToken("]");
        } else {
            processToken(".");
            processToken("identifier");
        }
    }
    
    deindent();
}

void Parser::expressionList() throw(ParseException) {
    setState("ExpressionList");
    indent();

    expression();
    while (match(",")) {
        processToken(",");
        expression();
    }
    
    deindent();
}

shared_ptr<Type> Parser::type() throw(ParseException) {
    shared_ptr<Type> result;

    setState("Type");
    indent();
    
    if (match("identifier")) {
        Token identifier = processToken("identifier");

        if (!suppressContextErrors) {
            result = findType(identifier.getValue());
            if (result == 0) {
                throw ParseException("Context violation: type "
                                     + identifier.toString()
                                     + " not declared!");
            }
        }
    } else if (match("ARRAY")) {
        processToken("ARRAY");
        expression();
        processToken("OF");
        shared_ptr<Type> arrayType = type();

        if (!suppressContextErrors) {
            result = shared_ptr<Type>(new Array(arrayType, 5));
        }

    } else if (match("RECORD")) {

        shared_ptr<Scope> recordScope(new Scope(symbolTable.getCurrentScope()));
        if (!suppressContextErrors) {
            symbolTable.setCurrentScope(recordScope);
        }

        processToken("RECORD");
        while (match("identifier")) {
            vector<Token> identifiers = identifierList();
            processToken(":");
            shared_ptr<Type> fieldType = type();
            processToken(";");

            if (!suppressContextErrors) {
                for (unsigned int c = 0; c < identifiers.size(); c++) {
                    if (symbolTable.getCurrentScope()->scopeContainsEntry(
                            identifiers[c].getValue())) {
                        throw ParseException("Context violation: duplicate "
                                             + identifiers[c].toString());
                    } else {
                        shared_ptr<Variable> entry(new Variable(fieldType));
                        symbolTable.getCurrentScope()->addEntry(
                                identifiers[c].getValue(), entry);
                    }
                }
            }

        }
        processToken("END");

        if (!suppressContextErrors) {
            symbolTable.setCurrentScope(recordScope->getOuter());
            recordScope->setOuter(0); // Set outer to null.

            result = shared_ptr<Record>(new Record(recordScope));
        }

    } else {
        throw ParseException("Type expected; actual: "
                + currentToken.toString());
    }
    
    deindent();

    return result;
}

vector<Token> Parser::identifierList() throw(ParseException) {
    setState("IdentifierList");
    indent();

    vector<Token> result;
    
    result.push_back(processToken("identifier"));
    while (match(",")) {
        processToken(",");
        result.push_back(processToken("identifier"));
    }
    
    deindent();

    return result;
}

void Parser::instructions() throw(ParseException) {
    setState("Instructions");
    indent();
    
    instruction();
    
    while (match(";")) {
        processToken(";");
        instruction();
    }
    
    deindent();
}

void Parser::instruction() throw(ParseException) {
    setState("Instruction");
    indent();
    
    if (match("identifier")) {
        assign();
    } else if (match("IF")) {
        parseIf();
    } else if (match("REPEAT")) {
        repeat();
    } else if (match("WHILE")) {
        parseWhile();
    } else if (match("READ")) {
        read();
    } else if (match("WRITE")) {
        write();
    } else {
        throw ParseException("Instruction expected; actual: "
                + currentToken.toString());
    }
    
    deindent();
}

void Parser::assign() throw(ParseException) {
    setState("Assign");
    indent();
    
    designator();
    processToken(":=");
    expression();
    
    deindent();
}

void Parser::parseIf() throw(ParseException) {
    setState("If");
    indent();
    
    processToken("IF");
    condition();
    processToken("THEN");
    instructions();
    
    if (match("ELSE")) {
        processToken("ELSE");
        instructions();
    }
    
    processToken("END");
    
    deindent();
}

void Parser::condition() throw(ParseException) {
    setState("Condition");
    indent();
    
    expression();
    if (match("=")) {
        processToken("=");
    } else if (match("#")) {
        processToken("#");
    } else if (match("<")) {
        processToken("<");
    } else if (match(">")) {
        processToken(">");
    } else if (match("<=")) {
        processToken("<=");
    } else if (match(">=")) {
        processToken(">=");
    } else {
        throw ParseException("Operator expected in condition; actual: "
                + currentToken.toString());
    }
    expression();
    
    deindent();
}

void Parser::repeat() throw(ParseException) {
    setState("Repeat");
    indent();
    
    processToken("REPEAT");
    instructions();
    processToken("UNTIL");
    condition();
    processToken("END");

    deindent();
}

void Parser::parseWhile() throw(ParseException) {
    setState("While");
    indent();
    
    processToken("WHILE");
    condition();
    processToken("DO");
    instructions();
    processToken("END");
    
    deindent();
}

void Parser::read() throw(ParseException) {
    setState("Read");
    indent();
    
    processToken("READ");
    designator();
    
    deindent();
}

void Parser::write() throw(ParseException) {
    setState("Write");
    indent();
    
    processToken("WRITE");
    expression();
    
    deindent();
}

SymbolTable &Parser::getSymbolTable() {
    return symbolTable;
}

AbstractSyntaxTree &Parser::getAbstractSyntaxTree() {
    return ast;
}

std::shared_ptr<Type> Parser::findType(const string& identifier) {
    return dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getEntry(identifier));
}
