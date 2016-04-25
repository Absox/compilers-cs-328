//
// Created by ran on 4/25/16.
//

#ifndef COMPILERS_HWK5_FORMAL_H
#define COMPILERS_HWK5_FORMAL_H


#include <string>
#include "Type.h"

class Formal {
public:
    Formal(const std::string& identifier, const std::shared_ptr<Type>& type);
private:
    std::string identifier;
    std::shared_ptr<Type> type;

    friend class Parser;
};


#endif //COMPILERS_HWK5_FORMAL_H
