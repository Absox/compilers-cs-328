//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_FIELD_H
#define COMPILERS_HWK5_FIELD_H

#include <memory>

#include "Location.h"
#include "VariableLocation.h"

class Field : public Location {
public:
    Field(const std::shared_ptr<Location>& location,
          const std::shared_ptr<VariableLocation>& variable);
    virtual ~Field();
    virtual std::string getLabel();
private:
    std::shared_ptr<Location> location;
    std::shared_ptr<VariableLocation> variable;

};


#endif //COMPILERS_HWK5_FIELD_H
