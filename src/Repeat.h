//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_REPEAT_H
#define COMPILERS_HWK5_REPEAT_H


#include <vector>

#include "Instruction.h"
#include "Condition.h"

class Repeat : public Instruction {
public:
    Repeat();
    virtual ~Repeat();
    virtual std::string getType();
private:
    std::shared_ptr<Condition> condition;
    std::vector<std::shared_ptr<Instruction>> instructions;

};


#endif //COMPILERS_HWK5_REPEAT_H
