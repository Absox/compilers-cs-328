//
// Created by ran on 3/26/16.
//

#include "ArrayBox.h"

using std::shared_ptr;



ArrayBox::ArrayBox(const unsigned int &size, const shared_ptr<Type>& type) {
    boxes = new shared_ptr<Box>[size];
    this->size = size;

    if (type != 0) {
        for (unsigned int c = 0; c < size; c++) {
            boxes[c] = type->initializeBox();
        }
    }

}

ArrayBox::~ArrayBox() {
    delete[] boxes;
}

void ArrayBox::assign(const std::shared_ptr<ArrayBox> &other) {
    for (unsigned int c = 0; c < size; c++) {
        boxes[c] = other->boxes[c]->copy();
    }

}


std::shared_ptr<Box> ArrayBox::copy() {
    shared_ptr<ArrayBox> result(new ArrayBox(size));
    for (unsigned int c = 0; c < size; c++) {
        result->boxes[c] = boxes[c]->copy();
    }

    return result;
}

