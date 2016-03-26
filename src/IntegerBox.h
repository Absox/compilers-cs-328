//
// Created by ran on 3/26/16.
//

#ifndef COMPILERS_HWK5_INTEGERBOX_H
#define COMPILERS_HWK5_INTEGERBOX_H


#include "Box.h"

class IntegerBox : public Box {
public:
    IntegerBox();
    virtual ~IntegerBox();

    int getValue() const;
    void setValue(int newValue);

    virtual std::shared_ptr<Box> copy() override;
private:
    int value;
};


#endif //COMPILERS_HWK5_INTEGERBOX_H
