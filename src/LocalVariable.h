//
// Created by ran on 4/25/16.
//

#ifndef COMPILERS_HWK5_LOCALVARIABLE_H
#define COMPILERS_HWK5_LOCALVARIABLE_H


#include "Variable.h"

class LocalVariable : public Variable {

public:
    LocalVariable(const std::shared_ptr<Type> &type);
    virtual ~LocalVariable();
};


#endif //COMPILERS_HWK5_LOCALVARIABLE_H
