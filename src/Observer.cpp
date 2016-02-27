/*
 * File:   Observer.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 * 
 * Created on February 20, 2016, 6:27 PM
 */

#include "Observer.h"

Observer::Observer() {
    subject = 0;
}

Observer::~Observer() {
}

void Observer::setSubject(Parser *parser) {
    subject = parser;
}