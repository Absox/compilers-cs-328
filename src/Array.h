//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_ARRAY_H
#define COMPILERS_HWK4_ARRAY_H

#include <memory>

#include "Type.h"

class Array : public Type {
public:
    Array(const std::shared_ptr<Type>& type, const int& length);
    virtual ~Array();

    std::shared_ptr<Type> getType() const;
    int getLength() const;
private:
    std::shared_ptr<Type> type;
    int length;
};


#endif //COMPILERS_HWK4_ARRAY_H
