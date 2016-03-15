//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Field.h"

using std::shared_ptr;

Field::Field(const shared_ptr<Location>& location,
             const shared_ptr<VariableLocation>& variable) {
    this->location = location;
    this->variable = variable;
}

Field::~Field() {

}

std::string Field::getLabel() {
    return "Field";
}

std::shared_ptr<Location> Field::getLocation() const {
    return location;
}

std::shared_ptr<VariableLocation> Field::getVariable() const {
    return variable;
}
