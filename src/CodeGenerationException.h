//
// Created by ran on 4/7/16.
//

#ifndef COMPILERS_HWK5_CODEGENERATIONEXCEPTION_H
#define COMPILERS_HWK5_CODEGENERATIONEXCEPTION_H

#include <string>

class CodeGenerationException {
public:
    CodeGenerationException(const std::string& message);
    std::string getMessage() const;
private:
    std::string message;
};


#endif //COMPILERS_HWK5_CODEGENERATIONEXCEPTION_H
