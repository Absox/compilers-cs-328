//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK5_LOCATION_H
#define COMPILERS_HWK5_LOCATION_H


#include "Expression.h"

class Location : public Expression {
public:
    Location();
    virtual ~Location();
    virtual std::string getLabel() = 0;
};


#endif //COMPILERS_HWK5_LOCATION_H
