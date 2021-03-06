//
// Created by ran on 4/16/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATIONMESSAGE_H
#define COMPILERS_HWK5_CODEGENERATIONMESSAGE_H

class CodeGenerationMessage {
public:
    CodeGenerationMessage(const bool &is_offset, const bool &is_register,
                          const long long &value, const bool &is_stack = false);
private:
    bool is_offset;
    bool is_register;
    bool is_stack;
    long long value;

    friend class CodeGenerator;
};


#endif //COMPILERS_HWK5_CODEGENERATIONMESSAGE_H
