/*
 * File:   Token.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 10:59 AM
 */

#include "Token.h"

using std::string;
using std::ostream;
using std::to_string;

Token::Token(const string& type, const string& value,
        const unsigned int& start, const unsigned int& end) {
    this->type = type;
    this->value = value;
    this->start = start;
    this->end = end;
}

Token::Token(const Token& orig) {
    this->type = orig.type;
    this->value = orig.value;
    this->start = orig.start;
    this->end = orig.end;
}

Token::~Token() {
}

string Token::getType() const {
    return type;
}

string Token::getValue() const {
    return value;
}

unsigned int Token::getStart() const {
    return start;
}

unsigned int Token::getEnd() const {
    return end;
}

bool Token::isEof() const {
    if (type == "eof") return true;
    return false;
}

string Token::toString() const {
    string result;
    
    if (type == "identifier" || type == "integer") {
        result += type;
        result += '<';
        result += value;
        result += '>';
    } else if (type == "eof") {
        result += type;
    } else {
        result += value;
    }
    
    result += "@(";
    result += to_string(start);
    result += ", ";
    result += to_string(end);
    result += ')';
    
    return result;
    
}

ostream& operator << (ostream& os, const Token& token) {
    os << token.toString();

    return os;
}