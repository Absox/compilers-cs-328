//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_WRITE_H
#define COMPILERS_HWK5_WRITE_H


#include "Instruction.h"

class Write : public Instruction {
public:
    Write(const std::shared_ptr<Expression>& expression);
    virtual ~Write();
    virtual std::string getLabel();
private:
    std::shared_ptr<Expression> expression;

};


#endif //COMPILERS_HWK5_WRITE_H
