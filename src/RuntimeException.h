//
// Created by ran on 3/26/16.
//

#ifndef COMPILERS_HWK5_RUNTIMEEXCEPTION_H
#define COMPILERS_HWK5_RUNTIMEEXCEPTION_H

#include <string>

class RuntimeException {
public:
    RuntimeException(const std::string& message);
    virtual ~RuntimeException();

    std::string getMessage() const;
private:
    std::string message;
};


#endif //COMPILERS_HWK5_RUNTIMEEXCEPTION_H
