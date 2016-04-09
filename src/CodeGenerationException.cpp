//
// Created by ran on 4/7/16.
//

#include "CodeGenerationException.h"


CodeGenerationException::CodeGenerationException(const std::string &message) {
    this->message = message;
}


std::string CodeGenerationException::getMessage() const {
    return message;
}

