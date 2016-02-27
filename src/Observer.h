/*
 * File:   Observer.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Abstract base class for Observer design pattern.
 * Created on February 20, 2016, 6:27 PM
 */

#ifndef OBSERVER_H
#define OBSERVER_H

#include "Parser.h"

class Parser;

class Observer {
public:
    Observer();
    virtual ~Observer();
    virtual void update() = 0;
    void setSubject(Parser* parser);
protected:
    Parser* subject;
};

#endif /* OBSERVER_H */

