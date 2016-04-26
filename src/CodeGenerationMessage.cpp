//
// Created by ran on 4/16/16.
//

#include "CodeGenerationMessage.h"


CodeGenerationMessage::CodeGenerationMessage(const bool &is_offset,
                                             const bool &is_register,
                                             const long long int &value,
                                             const bool& is_stack) {
    this->is_offset = is_offset;
    this->is_register = is_register;
    this->value = value;
    this->is_stack = is_stack;
}

