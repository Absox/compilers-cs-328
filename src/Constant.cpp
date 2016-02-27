//
// Created by Ran on 2/26/2016.
//

#include "Constant.h"

Constant::Constant(const int& value) {
    this->value = value;
}

Constant::~Constant() {

}

void Constant::setValue(const int &value) {
    this->value = value;
}

int Constant::getValue() {
    return value;
}

std::string Constant::getEntryType() const {
    return "CONSTANT";
}
