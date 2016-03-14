//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_IFINSTRUCTION_H
#define COMPILERS_HWK5_IFINSTRUCTION_H

#include <vector>

#include "Instruction.h"
#include "Condition.h"

class IfInstruction : public Instruction {
public:
    IfInstruction();
    virtual ~IfInstruction();
    virtual std::string getType();
private:
    std::shared_ptr<Condition> condition;
    std::vector<std::shared_ptr<Instruction>> instructions_true;
    std::vector<std::shared_ptr<Instruction>> instructions_false;

};


#endif //COMPILERS_HWK5_IFINSTRUCTION_H
