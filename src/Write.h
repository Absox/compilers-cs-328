//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_WRITE_H
#define COMPILERS_HWK5_WRITE_H


#include "Instruction.h"

class Write : public Instruction {
public:
    Write();
    virtual ~Write();
    virtual std::string getType();
private:
    std::shared_ptr<Expression> expression;

};


#endif //COMPILERS_HWK5_WRITE_H
