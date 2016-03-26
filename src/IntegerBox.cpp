//
// Created by ran on 3/26/16.
//

#include "IntegerBox.h"


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

