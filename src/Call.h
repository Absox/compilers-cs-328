//
// Created by ran on 4/25/16.
//

#ifndef COMPILERS_HWK5_CALL_H
#define COMPILERS_HWK5_CALL_H


#include <string>
#include <memory>
#include <vector>
#include "Expression.h"
#include "Instruction.h"
#include "Procedure.h"

class Call : public Instruction {
public:
    virtual std::string getLabel() override;

    Call(const std::string& identifier,
         const std::shared_ptr<Procedure>& procedure);
private:
    std::string identifier;
    std::vector<std::shared_ptr<Expression>> actuals;

    std::shared_ptr<Procedure> procedure;

    friend class Parser;
    friend class Interpreter;
};


#endif //COMPILERS_HWK5_CALL_H
