//
// Created by ran on 4/16/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATIONMESSAGE_H
#define COMPILERS_HWK5_CODEGENERATIONMESSAGE_H


class CodeGenerationMessage {
public:
    CodeGenerationMessage(const bool& is_offset, const bool& is_register, const int& value);
private:
    bool is_offset;
    bool is_register;
    int value;

    friend class CodeGenerator2;
};


#endif //COMPILERS_HWK5_CODEGENERATIONMESSAGE_H