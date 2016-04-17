//
// Created by ran on 4/16/16.
//

#include "CodeGenerationMessage.h"


CodeGenerationMessage::CodeGenerationMessage(const bool &is_offset,
                                             const bool &is_register,
                                             const int &value) {
    this->is_offset = is_offset;
    this->is_register = is_register;
    this->value = value;
}

