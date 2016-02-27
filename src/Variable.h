//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_VARIABLE_H
#define COMPILERS_HWK4_VARIABLE_H

#include <memory>

#include "Type.h"

class Variable : public Entry {
public:
    Variable();
    virtual ~Variable();
private:
    std::shared_ptr<Type> type;
};


#endif //COMPILERS_HWK4_VARIABLE_H
