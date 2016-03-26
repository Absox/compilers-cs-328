//
// Created by ran on 3/26/16.
//

#include "RuntimeException.h"


RuntimeException::RuntimeException(const std::string &message) {
    this->message = message;
}

RuntimeException::~RuntimeException() {

}

std::string RuntimeException::getMessage() const {
    return message;
}

