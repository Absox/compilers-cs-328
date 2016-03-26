//
// Created by Ran on 3/25/16.
//

#include "Interpreter.h"
#include "BinaryExpression.h"
#include "IntegerBox.h"
#include "ArrayBox.h"
#include "RecordBox.h"

#include <iostream>
#include <limits>

using std::vector;
using std::shared_ptr;
using std::dynamic_pointer_cast;

using std::cout;
using std::cin;
using std::endl;


Interpreter::Interpreter(const SymbolTable &symbolTable,
                         const vector<shared_ptr<Instruction>> &instructions)
    throw (RuntimeException) {
    buildEnvironment(symbolTable);
    run(instructions);
}

Interpreter::~Interpreter() {

}

void Interpreter::buildEnvironment(const SymbolTable &symbolTable) {
    this->environment = shared_ptr<Environment>(
            new Environment(symbolTable.getCurrentScope()->buildEnvironment()));
}

void Interpreter::run(const vector<shared_ptr<Instruction>> &instructions)
    throw (RuntimeException) {
    for (unsigned int c = 0; c < instructions.size(); c++) {
        runInstruction(instructions[c]);
    }
}


void Interpreter::runInstruction(
        const shared_ptr<Instruction> &instruction)
        throw (RuntimeException) {
    auto assign = dynamic_pointer_cast<Assign>(instruction);
    auto ifInstruction = dynamic_pointer_cast<IfInstruction>(instruction);
    auto repeat = dynamic_pointer_cast<Repeat>(instruction);
    auto read = dynamic_pointer_cast<Read>(instruction);
    auto write = dynamic_pointer_cast<Write>(instruction);

    if (assign != 0) {
        runAssign(assign);
    } else if (ifInstruction != 0) {
        runIfInstruction(ifInstruction);
    } else if (repeat != 0) {
        runRepeat(repeat);
    } else if (read != 0) {
        runRead(read);
    } else if (write != 0) {
        runWrite(write);
    }
}


void Interpreter::runAssign(const shared_ptr<Assign> &assign)
        throw (RuntimeException) {
    auto location = resolveLocation(assign->getLocation());

    auto integer = dynamic_pointer_cast<IntegerBox>(location);
    auto array = dynamic_pointer_cast<ArrayBox>(location);
    auto record = dynamic_pointer_cast<RecordBox>(location);

    if (integer != 0) {
        integer->setValue(resolveNumericExpression(assign->getExpression()));
    } else if (array != 0) {
        auto right_location = dynamic_pointer_cast<Location>(
                assign->getExpression());
        auto right_array = dynamic_pointer_cast<ArrayBox>(
                resolveLocation(right_location));
        array->assign(right_array);
    } else if (record != 0) {
        auto right_location = dynamic_pointer_cast<Location>(
                assign->getExpression());
        auto right_record = dynamic_pointer_cast<RecordBox>(
                resolveLocation(right_location));
        record->assign(right_record);
    }
}

void Interpreter::runIfInstruction(
        const shared_ptr<IfInstruction> &ifInstruction)
        throw (RuntimeException) {
    if (resolveCondition(ifInstruction->getCondition())) {
        run(ifInstruction->getInstructionsTrue());
    } else {
        run(ifInstruction->getInstructionsFalse());
    }
}

void Interpreter::runRepeat(const shared_ptr<Repeat> &repeat)
        throw (RuntimeException) {
    do {
        run(repeat->getInstructions());
    } while(resolveCondition(repeat->getCondition()));
}

void Interpreter::runRead(const shared_ptr<Read> &read)
        throw (RuntimeException) {
    int readValue;
    auto integerBox = dynamic_pointer_cast<IntegerBox>(
            resolveLocation(read->getLocation()));
    cin.clear();
    cin >> readValue;
    integerBox->setValue(readValue);
}

void Interpreter::runWrite(const shared_ptr<Write> &write)
        throw (RuntimeException) {
    int value = resolveNumericExpression(write->getExpression());
    cout << value << endl;
}


bool Interpreter::resolveCondition(
        const shared_ptr<Condition> &condition)
        throw (RuntimeException) {
    auto left = resolveNumericExpression(condition->getExpressionLeft());
    auto right = resolveNumericExpression(condition->getExpressionRight());

    if (condition->getLabel() == "=") {
        return left == right;
    } else if (condition->getLabel() == "#") {
        return left != right;
    } else if (condition->getLabel() == "<") {
        return left < right;
    } else if (condition->getLabel() == "<=") {
        return left <= right;
    } else if (condition->getLabel() == ">") {
        return left > right;
    } else if (condition->getLabel() == ">=") {
        return left >= right;
    } else {
        throw RuntimeException("Undefined relation encountered!");
    }
}

std::shared_ptr<Box> Interpreter::resolveLocation(
        const shared_ptr<Location> &location)
        throw (RuntimeException) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        return environment->getBox(variable->getIdentifier());
    } else if (index != 0) {
        return resolveIndex(index);
    } else {
        return resolveField(field);
    }
}


std::shared_ptr<Box> Interpreter::resolveIndex(
        const std::shared_ptr<Index> &index) throw (RuntimeException) {
    auto location = dynamic_pointer_cast<ArrayBox>(
            resolveLocation(index->getLocation()));
    auto indexValue = resolveNumericExpression(index->getExpression());

    if (indexValue < 0 || abs(indexValue) >= location->getSize()) {
        throw RuntimeException("Array index out of bounds!");
    }

    return location->getEntry(indexValue);
}


std::shared_ptr<Box> Interpreter::resolveField(
        const std::shared_ptr<Field> &field) throw (RuntimeException) {
    auto location = dynamic_pointer_cast<RecordBox>(
            resolveLocation(field->getLocation()));
    return location->getEntry(field->getVariable()->getIdentifier());
}


int Interpreter::resolveNumericExpression(
        const shared_ptr<Expression> &expression)
        throw (RuntimeException) {
    auto location = dynamic_pointer_cast<Location>(expression);
    auto number = dynamic_pointer_cast<NumberExpression>(expression);
    auto binary = dynamic_pointer_cast<BinaryExpression>(expression);

    if (number != 0) {
        return number->getValue();
    } else if (location != 0) {
        auto numberBox = dynamic_pointer_cast<IntegerBox>(
                resolveLocation(location));
        return numberBox->getValue();
    } else {
        auto left = resolveNumericExpression(binary->getExpressionLeft());
        auto right = resolveNumericExpression(binary->getExpressionRight());

        if (binary->getOperation() == "+") {
            return left + right;
        } else if (binary->getOperation() == "-") {
            return left - right;
        } else if (binary->getOperation() == "*") {
            return left * right;
        } else if (binary->getOperation() == "DIV") {
            if (right == 0) {
                throw RuntimeException("Can't divide by zero!");
            }
            return left / right;
        } else if (binary->getOperation() == "MOD") {
            if (right == 0) {
                throw RuntimeException("Can't modulus by zero!");
            }
            return left % right;
        } else {
            throw RuntimeException("Undefined operator encountered!");
        }
    }
}


bool Interpreter::isNumeric(const shared_ptr<Expression> &expression)
        throw (RuntimeException) {
    // If it's not a location, it has to be numeric.
    auto location = dynamic_pointer_cast<Location>(expression);
    if (location != 0) {
        // If it is a location, we check if it's an integer box.
        auto box = dynamic_pointer_cast<IntegerBox>(resolveLocation(location));
        return box != 0;
    } else {
        return true;
    }
}



