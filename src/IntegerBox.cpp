//
// Created by ran on 3/26/16.
//

#include "IntegerBox.h"

using std::shared_ptr;

IntegerBox::IntegerBox() {
    value = 0;
}

IntegerBox::~IntegerBox() {

}

int IntegerBox::getValue() const {
    return value;
}

void IntegerBox::setValue(int newValue) {
    value = newValue;
}

shared_ptr<Box> IntegerBox::copy() {
    shared_ptr<IntegerBox> result(new IntegerBox);
    result->setValue(value);
    return result;
}

