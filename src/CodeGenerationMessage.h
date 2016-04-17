//
// Created by ran on 4/16/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATIONMESSAGE_H
#define COMPILERS_HWK5_CODEGENERATIONMESSAGE_H


class CodeGenerationMessage {
public:
    CodeGenerationMessage(const bool &is_offset, const bool &is_register,
                          const long long &value);
private:
    bool is_offset;
    bool is_register;
    long long value;

    friend class CodeGenerator;
};


#endif //COMPILERS_HWK5_CODEGENERATIONMESSAGE_H
