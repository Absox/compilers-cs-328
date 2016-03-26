//
// Created by ran on 3/26/16.
//

#include "ArrayBox.h"

using std::shared_ptr;

ArrayBox::ArrayBox(const int &size) {
    boxes = new shared_ptr<ArrayBox>[size];
    this->size = size;
}

ArrayBox::~ArrayBox() {
    delete[] boxes;
}

void ArrayBox::assign(const std::shared_ptr<ArrayBox> &other) {

}

