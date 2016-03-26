//
// Created by ran on 3/26/16.
//

#ifndef COMPILERS_HWK5_ARRAYBOX_H
#define COMPILERS_HWK5_ARRAYBOX_H


#include <memory>

#include "Box.h"
#include "Type.h"

class ArrayBox : public Box {
public:
    ArrayBox(const unsigned int& size, const std::shared_ptr<Type>& type = 0);
    virtual ~ArrayBox();

    void assign(const std::shared_ptr<ArrayBox>& other);
    virtual std::shared_ptr<Box> copy() override;

    unsigned int getSize() const;
    std::shared_ptr<Box> getEntry(const int& index) const;

private:
    std::shared_ptr<Box>* boxes;
    unsigned int size;
};


#endif //COMPILERS_HWK5_ARRAYBOX_H
