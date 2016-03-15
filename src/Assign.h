//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_ASSIGN_H
#define COMPILERS_HWK5_ASSIGN_H


#include "Instruction.h"
#include "Location.h"

class Assign : public Instruction {
public:
    Assign(const std::shared_ptr<Location>& location,
           const std::shared_ptr<Expression>& expression);
    virtual ~Assign();
    virtual std::string getLabel();

    std::shared_ptr<Location> getLocation();
    std::shared_ptr<Expression> getExpression();
private:
    std::shared_ptr<Location> location;
    std::shared_ptr<Expression> expression;
};


#endif //COMPILERS_HWK5_ASSIGN_H
