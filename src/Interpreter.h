//
// Created by Ran on 3/25/16.
//

#ifndef COMPILERS_HWK5_INTERPRETER_H
#define COMPILERS_HWK5_INTERPRETER_H

#include <memory>
#include <vector>

#include "Instruction.h"
#include "SymbolTable.h"
#include "Environment.h"
#include "RuntimeException.h"
#include "Assign.h"
#include "IfInstruction.h"
#include "Repeat.h"
#include "Read.h"
#include "Write.h"
#include "NumberExpression.h"
#include "Index.h"
#include "Field.h"

class Interpreter {

public:
    Interpreter(const SymbolTable& symbolTable,
                const std::vector<std::shared_ptr<Instruction>>& instructions)
        throw (RuntimeException);
    virtual ~Interpreter();
private:
    std::shared_ptr<Environment> environment;

    void buildEnvironment(const SymbolTable& symbolTable);
    void run(const std::vector<std::shared_ptr<Instruction>>& instructions)
            throw (RuntimeException);
    void runInstruction(const std::shared_ptr<Instruction>& instruction)
            throw (RuntimeException);
    void runAssign(const std::shared_ptr<Assign>& assign)
            throw (RuntimeException);
    void runIfInstruction(const std::shared_ptr<IfInstruction>& ifInstruction)
            throw (RuntimeException);
    void runRepeat(const std::shared_ptr<Repeat>& repeat)
            throw (RuntimeException);
    void runRead(const std::shared_ptr<Read>& read) throw (RuntimeException);
    void runWrite(const std::shared_ptr<Write>& write) throw (RuntimeException);

    bool resolveCondition(const std::shared_ptr<Condition>& condition)
            throw (RuntimeException);
    std::shared_ptr<Box> resolveLocation(const std::shared_ptr<Location>& location)
            throw (RuntimeException);
    std::shared_ptr<Box> resolveIndex(const std::shared_ptr<Index>& index)
            throw (RuntimeException);
    std::shared_ptr<Box> resolveField(const std::shared_ptr<Field>& field)
            throw (RuntimeException);
    long long int resolveNumericExpression(
            const std::shared_ptr<Expression> &expression)
            throw (RuntimeException);
    bool isNumeric(const std::shared_ptr<Expression>& expression)
            throw (RuntimeException);

};


#endif //COMPILERS_HWK5_INTERPRETER_H
