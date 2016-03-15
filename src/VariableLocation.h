//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_VARIABLELOCATION_H
#define COMPILERS_HWK5_VARIABLELOCATION_H


#include "Location.h"

class VariableLocation : public Location {
public:
    VariableLocation(const std::string& identifier);
    virtual ~VariableLocation();
    virtual std::string getLabel();

    std::string getIdentifier() const;
private:
    std::string identifier;

};


#endif //COMPILERS_HWK5_VARIABLELOCATION_H
