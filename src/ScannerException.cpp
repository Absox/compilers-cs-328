/*
 * File:   ScannerException.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 11:13 AM
 */

#include "ScannerException.h"

using std::string;

ScannerException::ScannerException(const unsigned int& position,
        const string& message) {
    this->position = position;
    this->message = message;
}

ScannerException::~ScannerException() {
}

unsigned int ScannerException::getPosition() const {
    return position;
}

string ScannerException::getMessage() const {
    return message;
}