//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_IFINSTRUCTION_H
#define COMPILERS_HWK5_IFINSTRUCTION_H

#include <vector>

#include "Instruction.h"
#include "Condition.h"

class IfInstruction : public Instruction {
public:
    IfInstruction(const std::shared_ptr<Condition>& condition,
          const std::vector<std::shared_ptr<Instruction>>& instructions_true,
          const std::vector<std::shared_ptr<Instruction>>& instructions_false);
    virtual ~IfInstruction();
    virtual std::string getLabel();

    std::shared_ptr<Condition> getCondition();
    std::vector<std::shared_ptr<Instruction>> getInstructionsTrue();
    std::vector<std::shared_ptr<Instruction>> getInstructionsFalse();
private:
    std::shared_ptr<Condition> condition;
    std::vector<std::shared_ptr<Instruction>> instructions_true;
    std::vector<std::shared_ptr<Instruction>> instructions_false;
};


#endif //COMPILERS_HWK5_IFINSTRUCTION_H
