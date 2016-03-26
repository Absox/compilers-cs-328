//
// Created by ran on 3/25/16.
//

#ifndef COMPILERS_HWK5_BOX_H
#define COMPILERS_HWK5_BOX_H


#include <memory>

class Box {
public:
    Box();
    virtual ~Box();

    virtual std::shared_ptr<Box> copy() = 0;

};


#endif //COMPILERS_HWK5_BOX_H
