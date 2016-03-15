//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_REPEAT_H
#define COMPILERS_HWK5_REPEAT_H


#include <vector>

#include "Instruction.h"
#include "Condition.h"

class Repeat : public Instruction {
public:
    Repeat(const std::shared_ptr<Condition>& condition,
           const std::vector<std::shared_ptr<Instruction>>& instructions);
    virtual ~Repeat();
    virtual std::string getLabel();

    std::shared_ptr<Condition> getCondition();
    std::vector<std::shared_ptr<Instruction>> getInstructions();
private:
    std::shared_ptr<Condition> condition;
    std::vector<std::shared_ptr<Instruction>> instructions;

};


#endif //COMPILERS_HWK5_REPEAT_H
