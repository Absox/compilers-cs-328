//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_READ_H
#define COMPILERS_HWK5_READ_H


#include "Instruction.h"
#include "Location.h"

class Read : public Instruction {
public:
    Read(const std::shared_ptr<Location>& location);
    virtual ~Read();
    virtual std::string getLabel();
private:
    std::shared_ptr<Location> location;

};


#endif //COMPILERS_HWK5_READ_H
