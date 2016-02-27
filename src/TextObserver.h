/*
 * File:   TextObserver.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 7:33 PM
 */

#ifndef TEXTOBSERVER_H
#define TEXTOBSERVER_H

#include <sstream>
#include <string>

#include "Observer.h"


class TextObserver : public Observer {
public:
    TextObserver();
    virtual ~TextObserver();
    void update() override;
    
    std::string getContent();
    
private:
    std::stringstream stream;
};

#endif /* TEXTOBSERVER_H */

