/*
 * File:   ParseException.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 6:35 PM
 */

#ifndef PARSEEXCEPTION_H
#define PARSEEXCEPTION_H

#include <string>

class ParseException {
public:
    ParseException(const std::string& message);
    virtual ~ParseException();
    std::string getMessage() const;
private:
    std::string message;
};

#endif /* PARSEEXCEPTION_H */

