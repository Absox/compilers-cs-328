/*
 * File:   Parser.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 5:27 PM
 */

#include <iostream>

#include "Parser.h"
#include "Constant.h"
#include "Array.h"
#include "Variable.h"
#include "Record.h"
#include "NumberExpression.h"
#include "BinaryExpression.h"
#include "Index.h"
#include "Field.h"
#include "LocalVariable.h"
#include "Procedure.h"

using std::cout;
using std::endl;

using std::shared_ptr;
using std::string;
using std::to_string;
using std::stoll;
using std::vector;
using std::dynamic_pointer_cast;

Parser::Parser(Scanner *scanner, const bool &suppressContextErrors)
        : state("init"), currentToken("", "", 0, 0) {
    this->scanner = scanner;
    this->suppressContextErrors = suppressContextErrors;
    indentLevel = 0;
}

Parser::~Parser() {
}

void Parser::parse() throw(ParseException) {
    program();
}

void Parser::addObserver(Observer *o) {
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

void Parser::setState(const string &value) {
    state = ParseState(value);
    notifyObservers();
}

Token &Parser::nextToken() throw(ParseException) {
    try {
        currentToken = scanner->next();
        return currentToken;
    } catch (ScannerException &e) {
        throw ParseException("Scanner terminated with error: "
                             + e.getMessage() + " at position "
                             + to_string(e.getPosition()));
    }
}

bool Parser::match(const string &type) {
    return currentToken.getType() == type;
}

Token Parser::processToken(const string &type) throw(ParseException) {
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
        ast = instructions();
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

    while (match("CONST") || match("TYPE") || match("VAR") || match("PROCEDURE")) {
        if (match("CONST")) {
            constDecl();
        } else if (match("TYPE")) {
            typeDecl();
        } else if (match("VAR")){
            varDecl(false);
        } else if (match("PROCEDURE")) {
            procDecl();
        }
    }
    deindent();
}

void Parser::procDecl() {

    setState("ProcDecl");
    indent();

    processToken("PROCEDURE");
    auto proc_identifier = processToken("identifier");
    auto procedure = shared_ptr<Procedure>(new Procedure);

    symbolTable.getCurrentScope()->addEntry(proc_identifier.getValue(), procedure);

    auto proc_scope = shared_ptr<Scope>(new Scope(symbolTable.getCurrentScope()));
    procedure->scope = proc_scope;
    symbolTable.setCurrentScope(proc_scope);


    processToken("(");
    if (match("identifier")) {
        procedure->parameters = formals();
    }
    processToken(")");

    if (match(":")) {
        processToken(":");
        procedure->return_type = type(); // Return type
    }

    processToken(";");

    while (match("VAR")) varDecl(true);

    if (match("BEGIN")) {
        processToken("BEGIN");
        instructions();
    }

    if (match("RETURN")) {
        processToken("RETURN");
        expression();
    }

    processToken("END");
    processToken("identifier");
    processToken(";");

    symbolTable.setCurrentScope(proc_scope->getOuter());

    deindent();
}

vector<shared_ptr<Formal>> Parser::formals() {
    vector<shared_ptr<Formal>> result;
    setState("Formals");
    indent();

    formal();
    while (match("identifier")) {
        processToken(";");
        auto list = formal();
        for (unsigned int c = 0; c < list.size(); c++) {
            result.push_back(list[c]);
        }
    }

    deindent();
    return result;
}

vector<shared_ptr<Formal>> Parser::formal() {
    vector<shared_ptr<Formal>> result;
    setState("Formal");
    indent();

    auto identifiers = identifierList();
    processToken(":");
    auto parameter_type = type();

    for (unsigned int c = 0; c < identifiers.size(); c++) {
        if (symbolTable.getCurrentScope()->scopeContainsEntry(identifiers[c].getValue())) {
            throw ParseException("Context violation: duplicate " + identifiers[c].toString());
        } else {
            result.push_back(shared_ptr<Formal>(new Formal(identifiers[c].getValue(), parameter_type)));
            auto local_variable_entry = shared_ptr<LocalVariable>(new LocalVariable(parameter_type));
            symbolTable.getCurrentScope()->addEntry(identifiers[c].getValue(), local_variable_entry);
        }
    }

    deindent();
    return result;
}


void Parser::constDecl() throw(ParseException) {
    setState("ConstDecl");
    indent();

    processToken("CONST");

    while (match("identifier")) {
        Token identifier = processToken("identifier");
        processToken("=");
        auto value = dynamic_pointer_cast<NumberExpression>(expression());

        if (!suppressContextErrors) {
            if (symbolTable.getCurrentScope()->scopeContainsEntry(
                    identifier.getValue())) {
                throw ParseException("Context violation: duplicate "
                                     + identifier.toString());
            } else {

                if (value != 0) {
                    shared_ptr<Entry> constant(new Constant(value->getValue(),
                                                            findType(
                                                                    "INTEGER")));
                    symbolTable.getCurrentScope()->addEntry(
                            identifier.getValue(), constant);
                } else {
                    throw ParseException(
                            "Context violation: const declaration beginning at "
                            + identifier.toString()
                            + " does not evaluate to a constant!");
                }
            }
        }

        processToken(";");
    }

    deindent();
}

void Parser::typeDecl() throw(ParseException) {
    setState("TypeDecl");
    indent();

    processToken("TYPE");

    while (match("identifier")) {
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

void Parser::varDecl(const bool& local) throw(ParseException) {
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
                    if (local) {
                        shared_ptr<Entry> local_var(new LocalVariable(varType));
                        symbolTable.getCurrentScope()->addEntry(
                                identifiers[c].getValue(), local_var);
                    } else {
                        shared_ptr<Entry> varEntry(new Variable(varType));
                        symbolTable.getCurrentScope()->addEntry(
                                identifiers[c].getValue(), varEntry);
                    }

                }
            }
        }
    }
    deindent();
}

shared_ptr<Expression> Parser::expression() throw(ParseException) {
    shared_ptr<Expression> result;
    bool negate = false;

    setState("Expression");
    indent();

    if (match("+")) {
        processToken("+");
        if (!suppressContextErrors && !isExpressionNumeric(result)) {
            throw ParseException(
                    "Context violation: non-int expression used with operator");
        }
    } else if (match("-")) {
        processToken("-");
        negate = true;
        if (!suppressContextErrors && !isExpressionNumeric(result)) {
            throw ParseException(
                    "Context violation: non-int expression used with operator");
        }
    }

    result = term();

    while (match("+") || match("-")) {
        string operation;
        if (match("+")) {
            operation = processToken("+").getValue();
        } else if (match("-")) {
            operation = processToken("-").getValue();
        }


        auto right = term();

        if (!suppressContextErrors &&
            (!isExpressionNumeric(result) || !isExpressionNumeric(right))) {
            throw ParseException(
                    "Context violation: non-int expression used with operator");
        }
        auto binary = shared_ptr<BinaryExpression>(
                new BinaryExpression(operation, result, right));
        auto folded = binary->fold();
        if (folded != 0) {
            result = folded;
        } else {
            result = binary;
        }
    }

    deindent();

    if (negate) {
        auto zero = shared_ptr<NumberExpression>(new NumberExpression(0));
        auto negatedExpression = shared_ptr<BinaryExpression>(
                new BinaryExpression("-", zero, result));
        auto folded = negatedExpression->fold();
        if (folded != 0) {
            result = folded;
        } else {
            result = negatedExpression;
        }
    }
    return result;
}

shared_ptr<Expression> Parser::term() throw(ParseException) {
    shared_ptr<Expression> result;
    setState("Term");
    indent();

    result = factor();
    while (match("*") || match("DIV") || match("MOD")) {
        string operation;
        if (match("*")) {
            operation = processToken("*").getValue();
        } else if (match("DIV")) {
            operation = processToken("DIV").getValue();
        } else {
            operation = processToken("MOD").getValue();
        }

        auto right = factor();
        if (!suppressContextErrors &&
            (!isExpressionNumeric(result) || !isExpressionNumeric(right))) {
            throw ParseException(
                    "Context violation: non-int expression used with operator");
        }

        auto binary = shared_ptr<BinaryExpression>(
                new BinaryExpression(operation, result, right));
        auto folded = binary->fold();
        if (folded != 0) {
            result = folded;
        } else {
            result = binary;
        }
    }

    deindent();
    return result;
}

shared_ptr<Expression> Parser::factor() throw(ParseException) {
    shared_ptr<Expression> result;
    setState("Factor");
    indent();

    if (match("integer")) {
        long long value = stoll(processToken("integer").getValue());
        result = shared_ptr<NumberExpression>(new NumberExpression(value));
    } else if (match("identifier")) {

        auto designatorLocation = designator();
        result = designatorLocation;
        auto type = getLocationType(designatorLocation);

        /*
         * For now, this seems to be an unreachable case.
         */
        // Check that this denotes either a constant or variable.
        if (!suppressContextErrors && type == 0) {
            throw ParseException(
                    "Context error: designator must denote var or const!");
        }

        // Check if we can do constant folding with this designator.
        auto variable = dynamic_pointer_cast<VariableLocation>(result);
        if (variable != 0) {
            auto entry = symbolTable.getCurrentScope()
                    ->getEntry(variable->getLabel());
            auto constant = dynamic_pointer_cast<Constant>(entry);
            if (constant != 0) {
                result = shared_ptr<NumberExpression>(
                        new NumberExpression(constant->getValue()));
            }
        }

    } else if (match("(")) {
        processToken("(");

        result = expression();
        if (!suppressContextErrors && !isExpressionNumeric(result)) {
            throw ParseException(
                    "Context violation: non-int expression used with operator");
        }
        processToken(")");

    } else {
        throw ParseException("Factor expected; actual: "
                             + currentToken.toString());
    }

    deindent();
    return result;
}

shared_ptr<Location> Parser::designator() throw(ParseException) {
    shared_ptr<Location> result;
    setState("Designator");
    indent();

    result = selector(processToken("identifier"));

    deindent();
    return result;
}

shared_ptr<Location> Parser::selector(
        const Token &identifier) throw(ParseException) {
    shared_ptr<Location> result;
    shared_ptr<Entry> currentEntry;
    shared_ptr<Type> currentType;

    setState("Selector");
    indent();

    if (!suppressContextErrors) {
        result = shared_ptr<VariableLocation>(
                new VariableLocation(identifier.getValue()));
        currentEntry = symbolTable.getCurrentScope()->getEntry(
                identifier.getValue());

        if (currentEntry == 0) {
            throw ParseException("Context violation: "
                                 + identifier.toString() + " not declared!");
        }

        // If var, we should get the type.
        auto var = dynamic_pointer_cast<Variable>(currentEntry);

        // Type checking: must be variable or a constant
        if (var == 0 && dynamic_pointer_cast<Constant>(currentEntry) == 0) {
            throw ParseException("Context violation: " + identifier.toString()
                                 + " is not a constant or a variable!");
        }
        if (var != 0) currentType = var->getType();
    }


    while (match("[") || match(".")) {
        if (match("[")) {

            // Bracket operator only applicable to arrays.

            processToken("[");

            auto indices = expressionList();

            if (!suppressContextErrors) {
                for (unsigned int c = 0; c < indices.size(); c++) {

                    // Check that each expression in list is numeric
                    if (!isExpressionNumeric(indices[c])) {
                        throw ParseException("Context violation: indices at "
                                             + identifier.toString() +
                                             " must be numeric!");
                    }

                    auto array = dynamic_pointer_cast<Array>(currentType);
                    if (array == 0) {
                        throw ParseException("Context violation: selector at "
                                             + identifier.toString() +
                                             " does not denote an array!");
                    }
                    currentType = array->getType();

                    result = shared_ptr<Index>(new Index(result, indices[c]));
                }
            }


            processToken("]");
        } else {

            // Selector operator only applicable to records.
            processToken(".");
            auto variableLocation = shared_ptr<VariableLocation>(
                    new VariableLocation(
                            processToken("identifier").getValue()));

            if (!suppressContextErrors) {
                auto record = dynamic_pointer_cast<Record>(currentType);
                if (record == 0) {
                    throw ParseException("Context violation: selector at "
                                         + identifier.toString() +
                                         " does not denote a record!");
                }
                // Check existence of field
                auto field = dynamic_pointer_cast<Variable>(
                        record->getScope()->getEntry(
                                variableLocation->getIdentifier()));
                if (field == 0) {
                    throw ParseException("Context violation: selector at "
                                         + identifier.toString() +
                                         " denotes a nonexistent field!");
                }
                currentType = field->getType();

                result = shared_ptr<Field>(new Field(result, variableLocation));
            }
        }
    }

    if (!suppressContextErrors) {

        if (dynamic_pointer_cast<Variable>(currentEntry) == 0
            && dynamic_pointer_cast<Constant>(currentEntry) == 0) {
            throw ParseException("Context violation: designator beginning at "
                                 + identifier.toString()
                                 + "must specify either variable or constant!");
        }

    }

    deindent();
    return result;
}

vector<shared_ptr<Expression>> Parser::expressionList() throw(ParseException) {
    vector<shared_ptr<Expression>> result;
    setState("ExpressionList");
    indent();

    result.push_back(expression());
    while (match(",")) {
        processToken(",");
        result.push_back(expression());
    }

    deindent();
    return result;
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
        auto length = dynamic_pointer_cast<NumberExpression>(expression());
        processToken("OF");
        shared_ptr<Type> arrayType = type();

        if (!suppressContextErrors) {
            if (length == 0) {
                throw ParseException("Context error: array length non const!");
            }
            if (length->getValue() <= 0) {
                throw ParseException("Length of array must be positive!");
            }

            result = shared_ptr<Type>(new Array(arrayType, length->getValue()));
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

vector<shared_ptr<Instruction>> Parser::instructions() throw(ParseException) {
    vector<shared_ptr<Instruction>> result;
    setState("Instructions");
    indent();

    result.push_back(instruction());

    while (match(";")) {
        processToken(";");
        result.push_back(instruction());
    }

    deindent();
    return result;
}

shared_ptr<Instruction> Parser::instruction() throw(ParseException) {
    shared_ptr<Instruction> result;
    setState("Instruction");
    indent();

    if (match("identifier")) {
        result = assign();
    } else if (match("IF")) {
        result = parseIf();
    } else if (match("REPEAT")) {
        result = repeat();
    } else if (match("WHILE")) {
        result = parseWhile();
    } else if (match("READ")) {
        result = read();
    } else if (match("WRITE")) {
        result = write();
    } else {
        throw ParseException("Instruction expected; actual: "
                             + currentToken.toString());
    }

    deindent();
    return result;
}

shared_ptr<Assign> Parser::assign() throw(ParseException) {
    shared_ptr<Assign> result;
    setState("Assign");
    indent();

    auto assignmentLocation = designator();
    processToken(":=");
    auto assignmentExpression = expression();

    if (!suppressContextErrors) {

        // Left side must be a variable - i.e. non-const
        auto variableLocation = dynamic_pointer_cast<VariableLocation>(
                assignmentLocation);
        if (variableLocation != 0) {
            auto variableEntry = dynamic_pointer_cast<Variable>(
                    symbolTable.getCurrentScope()->getEntry(
                            variableLocation->getIdentifier()));
            if (variableEntry == 0) {
                throw ParseException("Context error: must assign to variable!");
            }
        }

        // Types must be compatible
        auto locationType = getLocationType(assignmentLocation);
        auto expressionType = getExpressionType(assignmentExpression);

        if (locationType != expressionType) {
            throw ParseException(
                    "Context error: assignment with incompatible types!");
        }

        result = shared_ptr<Assign>(
                new Assign(assignmentLocation, assignmentExpression));
    }

    deindent();
    return result;
}

shared_ptr<IfInstruction> Parser::parseIf() throw(ParseException) {
    shared_ptr<IfInstruction> result;

    setState("If");
    indent();

    processToken("IF");
    auto ifCondition = condition();
    processToken("THEN");
    auto trueInstructions = instructions();

    vector<shared_ptr<Instruction>> falseInstructions;

    if (match("ELSE")) {
        processToken("ELSE");
        falseInstructions = instructions();
    }

    result = shared_ptr<IfInstruction>(
            new IfInstruction(
                    ifCondition, trueInstructions, falseInstructions));

    processToken("END");

    deindent();

    return result;
}

shared_ptr<Condition> Parser::condition() throw(ParseException) {
    shared_ptr<Condition> result;

    setState("Condition");
    indent();

    auto left = expression();
    string relation;
    if (match("=")) {
        relation = processToken("=").getValue();
    } else if (match("#")) {
        relation = processToken("#").getValue();
    } else if (match("<")) {
        relation = processToken("<").getValue();
    } else if (match(">")) {
        relation = processToken(">").getValue();
    } else if (match("<=")) {
        relation = processToken("<=").getValue();
    } else if (match(">=")) {
        relation = processToken(">=").getValue();
    } else {
        throw ParseException("Operator expected in condition; actual: "
                             + currentToken.toString());
    }
    auto right = expression();

    if (!suppressContextErrors) {
        if (!isExpressionNumeric(left) || !isExpressionNumeric(right)) {
            throw ParseException(
                    "Context error: expressions in condition must be numeric");
        }
        result = shared_ptr<Condition>(new Condition(relation, left, right));
    }

    deindent();
    return result;
}

shared_ptr<Repeat> Parser::repeat() throw(ParseException) {
    shared_ptr<Repeat> result;
    setState("Repeat");
    indent();

    processToken("REPEAT");
    auto repeatInstructions = instructions();
    processToken("UNTIL");
    auto repeatCondition = condition();
    processToken("END");

    result = shared_ptr<Repeat>(
            new Repeat(repeatCondition, repeatInstructions));

    deindent();
    return result;
}

shared_ptr<IfInstruction> Parser::parseWhile() throw(ParseException) {
    shared_ptr<IfInstruction> result;
    setState("While");
    indent();

    // Must convert into an IF wrapped around a REPEAT
    processToken("WHILE");
    auto whileCondition = condition();
    processToken("DO");
    auto whileInstructions = instructions();
    processToken("END");

    if (!suppressContextErrors) {
        auto inverseCondition = whileCondition->inverse();
        auto repeatClause = shared_ptr<Repeat>(
                new Repeat(inverseCondition, whileInstructions));
        vector<shared_ptr<Instruction>> ifInstructions;
        ifInstructions.push_back(repeatClause);
        vector<shared_ptr<Instruction>> falseInstructions;
        result = shared_ptr<IfInstruction>(
                new IfInstruction(whileCondition, ifInstructions,
                                  falseInstructions));
    }

    deindent();
    return result;
}

shared_ptr<Read> Parser::read() throw(ParseException) {
    shared_ptr<Read> result;
    setState("Read");
    indent();

    processToken("READ");
    auto location = designator();

    if (!suppressContextErrors) {
        // Have to check that we're reading into a variable, if our location
        // is determined by a bare identifier.
        auto variableLocation =
                dynamic_pointer_cast<VariableLocation>(location);
        if (variableLocation != 0) {
            auto variableEntry = dynamic_pointer_cast<Variable>(
                    symbolTable.getCurrentScope()->getEntry(
                            variableLocation->getIdentifier()));
            if (variableEntry == 0) {
                throw ParseException("Context error: reading into non-variable");
            }
        }

        if (!isExpressionNumeric(location)) {
            throw ParseException(
                    "Context error: reading into location that is non-numeric");
        }

        result = shared_ptr<Read>(new Read(location));
    }

    deindent();
    return result;
}

shared_ptr<Write> Parser::write() throw(ParseException) {
    shared_ptr<Write> result;
    setState("Write");
    indent();

    processToken("WRITE");
    auto writeExpression = expression();
    if (!suppressContextErrors) {

        if (!isExpressionNumeric(writeExpression)) {
            throw ParseException(
                    "Context error: expression being written is not numeric");
        }

        result = shared_ptr<Write>(new Write(writeExpression));
    }

    deindent();
    return result;
}

SymbolTable &Parser::getSymbolTable() {
    return symbolTable;
}

vector<shared_ptr<Instruction>> &Parser::getAbstractSyntaxTree() {
    return ast;
}

std::shared_ptr<Type> Parser::findType(const string &identifier) {
    return dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getEntry(identifier));
}

/*
 * If we have an expression that indicates a location, we check if
 * that location denotes an integer. Otherwise we assume true, because
 * operators can only be worked on integers.
 */
bool Parser::isExpressionNumeric(const shared_ptr<Expression> &expression) {
    static auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    return getExpressionType(expression) == universalInt;
}

// Checks the type of an expression.
std::shared_ptr<Type> Parser::getExpressionType(
        const std::shared_ptr<Expression> &expression) {
    static auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    auto location = dynamic_pointer_cast<Location>(expression);

    // If we're not a location, we're binary (which must be integer) or number.
    if (location != 0) {
        return getLocationType(location);
    }

    return universalInt;
}

// Gets the type associated with a location, recursively.
shared_ptr<Type> Parser::getLocationType(const shared_ptr<Location> &location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        auto entry = symbolTable.getCurrentScope()->getEntry(
                variable->getIdentifier());
        auto variableEntry = dynamic_pointer_cast<Variable>(entry);
        auto constEntry = dynamic_pointer_cast<Constant>(entry);
        if (variableEntry != 0) {
            return variableEntry->getType();
        } else if (constEntry != 0) {
            return constEntry->getType();
        }
    } else if (index != 0) {
        auto parentArray = dynamic_pointer_cast<Array>(
                getLocationType(index->getLocation()));
        if (parentArray != 0) {
            return parentArray->getType();
        }

    } else if (field != 0) {
        auto parentRecord = dynamic_pointer_cast<Record>(
                getLocationType(field->getLocation()));
        if (parentRecord != 0) {
            auto fieldEntry = dynamic_pointer_cast<Variable>(
                    parentRecord->getScope()->getEntry(
                            field->getVariable()->getIdentifier()));
            if (fieldEntry != 0) {
                return fieldEntry->getType();
            }
        }
    }

    return 0;
}
