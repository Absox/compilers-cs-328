//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_ASSIGN_H
#define COMPILERS_HWK5_ASSIGN_H


#include "Instruction.h"
#include "Location.h"

class Assign : public Instruction {
public:
    Assign();
    virtual ~Assign();
    virtual std::string getType();
private:
    std::shared_ptr<Location> location;
    std::shared_ptr<Expression> expression;
};


#endif //COMPILERS_HWK5_ASSIGN_H
