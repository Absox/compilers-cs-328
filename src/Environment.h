//
// Created by ran on 3/25/16.
//

#ifndef COMPILERS_HWK5_ENVIRONMENT_H
#define COMPILERS_HWK5_ENVIRONMENT_H


#include <string>
#include <memory>
#include <unordered_map>

#include "Box.h"

class Environment {
public:
    Environment(const std::unordered_map<std::string, std::shared_ptr<Box>>&
                scope);
    virtual ~Environment();

private:
    std::unordered_map<std::string, std::shared_ptr<Box>> scope;
};


#endif //COMPILERS_HWK5_ENVIRONMENT_H
