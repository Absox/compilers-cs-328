//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_LOCATION_H
#define COMPILERS_HWK5_LOCATION_H


#include "AbstractSyntaxTreeNode.h"

class Location : public AbstractSyntaxTreeNode {
public:
    Location();
    virtual ~Location();
    virtual std::string getType() = 0;
};


#endif //COMPILERS_HWK5_LOCATION_H
