//
// Created by ran on 3/26/16.
//

#ifndef COMPILERS_HWK5_ARRAYBOX_H
#define COMPILERS_HWK5_ARRAYBOX_H


#include <memory>

#include "Box.h"

class ArrayBox : public Box {
public:
    ArrayBox(const int& size);
    virtual ~ArrayBox();

    void assign(const std::shared_ptr<ArrayBox>& other);
private:
    std::shared_ptr<Box>* boxes;
    int size;
};


#endif //COMPILERS_HWK5_ARRAYBOX_H
