/*
 * File:   Token.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 10:59 AM
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

class Token {
public:
    Token(const std::string& type, const std::string& value,
            const unsigned int& start, const unsigned int& end);
    Token(const Token& orig);
    virtual ~Token();
    
    std::string getType() const;
    std::string getValue() const;
    unsigned int getStart() const;
    unsigned int getEnd() const;
    bool isEof() const;
    std::string toString() const;
    
    friend std::ostream& operator << (std::ostream& os, const Token& token);
    
private:
    std::string type;
    std::string value;
    unsigned int start;
    unsigned int end;
};

#endif /* TOKEN_H */

