//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_READ_H
#define COMPILERS_HWK5_READ_H


#include "Instruction.h"

class Read : public Instruction {
public:
    Read();
    virtual ~Read();
    virtual std::string getType();
private:
    std::shared_ptr<Expression> expression;

};


#endif //COMPILERS_HWK5_READ_H
