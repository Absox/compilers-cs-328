//
// Created by ran on 4/26/16.
//

#ifndef COMPILERS_HWK5_PARAMETERVARIABLE_H
#define COMPILERS_HWK5_PARAMETERVARIABLE_H


#include "Variable.h"

class ParameterVariable : public Variable {

public:
    ParameterVariable(const std::shared_ptr<Type> &type);
private:
    bool is_register;

    friend class CodeGenerator;
};


#endif //COMPILERS_HWK5_PARAMETERVARIABLE_H
