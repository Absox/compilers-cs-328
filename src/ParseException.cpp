/*
 * File:   ParseException.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 6:35 PM
 */

#include "ParseException.h"

using std::string;

ParseException::ParseException(const string& message) {
    this->message = message;
}

ParseException::~ParseException() {
}

string ParseException::getMessage() const {
    return message;
}