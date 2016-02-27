//
// Created by Ran on 2/26/2016.
//

#include "Array.h"

using std::shared_ptr;

Array::Array(const std::shared_ptr<Type> &type, const int &length) : Type("ARRAY") {
    this->type = type;
    this->length = length;
}

Array::~Array() {

}


shared_ptr<Type> Array::getType() const {
    return type;
}


int Array::getLength() const {
    return length;
}