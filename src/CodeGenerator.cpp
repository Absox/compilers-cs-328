//
// Created by ran on 4/6/16.
//

#include "CodeGenerator.h"
#include "Array.h"
#include "Record.h"
#include "NumberExpression.h"
#include "BinaryExpression.h"
#include "VariableLocation.h"
#include "Constant.h"
#include "Field.h"
#include "Index.h"


using std::string;
using std::dynamic_pointer_cast;
using std::vector;
using std::shared_ptr;
using std::endl;
using std::to_string;

CodeGenerator::CodeGenerator(
        SymbolTable &symbolTable,
        const vector<shared_ptr<Instruction>> &instructions)
        throw (CodeGenerationException)
        : symbolTable(symbolTable) {
    indentLevel = 0;
    labelCounter = 0;
    stackSize = 0;

    calculateOffsets();
    initializeProgram();
    processInstructions(instructions);
    finalizeProgram();
}

string CodeGenerator::getContent() {
    return stream.str();
}

void CodeGenerator::calculateOffsets() throw (CodeGenerationException) {
    // First, register the universal integer as 4 bytes.
    auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    typeSizes[universalInt] = 4;

    totalBytes = calculateScopeOffsets(symbolTable.getCurrentScope());
    if (totalBytes > 4294967296) {
        throw CodeGenerationException(
                "More memory requested than can be addressed!");
    }
}

unsigned long long CodeGenerator::calculateScopeOffsets(
        const shared_ptr<Scope> &scope) {
    unsigned long long nextOffset = 0;

    vector<string> identifiers = scope->getIdentifiersSorted();
    for (unsigned c = 0; c < identifiers.size(); c++) {
        auto variable = dynamic_pointer_cast<Variable>(
                scope->getEntryInScope(identifiers[c]));
        if (variable != 0) {

            variable->setOffset(nextOffset);
            unsigned long long size = getTypeSize(variable->getType());
            nextOffset += size;
        }
    }

    return nextOffset;
}


unsigned long long CodeGenerator::getTypeSize(const shared_ptr<Type> &type) {

    // If found in the hashmap, store its size.
    if (typeSizes.find(type) != typeSizes.end()) {
        return typeSizes[type];
    }

    // Otherwise, we're dealing with an array or a record.
    auto array = dynamic_pointer_cast<Array>(type);
    auto record = dynamic_pointer_cast<Record>(type);

    if (array != 0) {
        unsigned long long result =
                array->getLength() * getTypeSize(array->getType());
        typeSizes[array] = result;
        return result;
    } else {
        unsigned long long result = calculateScopeOffsets(record->getScope());
        typeSizes[record] = result;
        return result;
    }

}


void CodeGenerator::initializeProgram() {
    indent();
    writeWithIndent(".arch armv6");
    writeWithIndent(".data");
    deindent();

    writeWithIndent("string_output:");
    indent();
    writeWithIndent(".asciz \"%s\\n\"");
    deindent();
    writeWithIndent("output_format:");
    indent();
    writeWithIndent(".asciz \"%d\\n\"");
    deindent();
    writeWithIndent("input_format:");
    indent();
    writeWithIndent(".asciz \"%d\"");
    deindent();
    writeWithIndent("divide_by_zero_error:");
    indent();
    writeWithIndent(".asciz \"error: Divided by zero!\"");
    deindent();
    writeWithIndent("mod_by_zero_error:");
    indent();
    writeWithIndent(".asciz \"error: Modulus by zero!\"");
    deindent();
    writeWithIndent("array_bounds_error:");
    indent();
    writeWithIndent(".asciz \"error: Array index out of bounds!\"");

    if (totalBytes > 0) {
        deindent();
        writeWithIndent("variables:");
        indent();
        writeWithIndent(".space " + to_string(totalBytes));
    }

    writeWithIndent(".text");
    writeWithIndent(".global main");
    deindent();

    writeWithIndent("main:");

    indent();
    writeWithIndent("push {fp, lr}");
    if (totalBytes > 0) writeWithIndent("ldr r11, =variables");

    writeWithIndent("");
}


void CodeGenerator::finalizeProgram() {

    writeWithIndent("b terminate");
    deindent();
    writeWithIndent("div_zero:");
    indent();
    writeWithIndent("ldr r0, standard_error");
    writeWithIndent("ldr r0, [r0]");

    writeWithIndent("ldr r1, =string_output");
    writeWithIndent("ldr r2, =divide_by_zero_error");
    writeWithIndent("bl fprintf");
    writeWithIndent("b terminate");

    deindent();
    writeWithIndent("mod_zero:");
    indent();
    writeWithIndent("ldr r0, standard_error");
    writeWithIndent("ldr r0, [r0]");
    writeWithIndent("ldr r1, =string_output");
    writeWithIndent("ldr r2, =mod_by_zero_error");
    writeWithIndent("bl fprintf");
    writeWithIndent("b terminate");

    deindent();
    writeWithIndent("array_out_of_bounds:");
    indent();
    writeWithIndent("ldr r0, standard_error");
    writeWithIndent("ldr r0, [r0]");

    writeWithIndent("ldr r1, =string_output");
    writeWithIndent("ldr r2, =array_bounds_error");
    writeWithIndent("bl fprintf");
    writeWithIndent("b terminate");

    deindent();
    writeWithIndent("terminate:");
    indent();
    // Note that we have to reset the stack to the beginning of the frame.
    writeWithIndent("mov r0, #0");
    writeWithIndent("bl exit");

    deindent();
    writeWithIndent("standard_error:");
    indent();
    writeWithIndent(".word stderr");
    deindent();
}


void CodeGenerator::processInstructions(
        const vector<shared_ptr<Instruction>> &instructions) {
    for (unsigned long c = 0; c < instructions.size(); c++) {
        processInstruction(instructions[c]);
    }
}

void CodeGenerator::processInstruction(
        const shared_ptr<Instruction> &instruction) {
    auto assign = dynamic_pointer_cast<Assign>(instruction);
    auto ifInstruction = dynamic_pointer_cast<IfInstruction>(instruction);
    auto repeat = dynamic_pointer_cast<Repeat>(instruction);
    auto read = dynamic_pointer_cast<Read>(instruction);
    auto write = dynamic_pointer_cast<Write>(instruction);

    if (assign != 0) {
        writeWithIndent("@Assign");
        processAssign(assign);
    } else if (ifInstruction != 0) {
        writeWithIndent("@If");
        processIfInstruction(ifInstruction);
    } else if (repeat != 0) {
        writeWithIndent("@Repeat");
        processRepeat(repeat);
    } else if (read != 0) {
        writeWithIndent("@Read");
        processRead(read);
    } else if (write != 0) {
        writeWithIndent("@Write");
        processWrite(write);
    }
    // Pad with extra line.
    writeWithIndent("");
}

void CodeGenerator::processAssign(const shared_ptr<Assign>& assign) {
    auto locationOffset = resolveLocationOffset(assign->getLocation());
    auto type = getLocationType(assign->getLocation());


    auto record = dynamic_pointer_cast<Record>(type);
    auto array = dynamic_pointer_cast<Array>(type);
    auto size = typeSizes[type];

    if (record != 0 || array != 0) {
        auto origin_location = resolveLocationOffset(
                dynamic_pointer_cast<Location>(assign->getExpression()));
        // Need to copy the memory word by word

    } else {
        // Simple integer assignment.
        auto expressionValue = resolveExpressionValue(assign->getExpression());
        if (!expressionValue.is_register) {
            if (canImmediateValue(expressionValue.value)) {
                writeWithIndent("mov r0, #" + to_string(expressionValue.value));
            } else {
                writeWithIndent("ldr r0, =" + to_string(expressionValue.value));
            }
        }

        if (locationOffset.is_register) {
            if (expressionValue.is_register) {
                writeWithIndent("str r" + to_string(expressionValue.value)
                                + ", [r11, r" + to_string(locationOffset.value)
                                + "]");
                pop();
            } else {
                writeWithIndent("str r0, [r11, r"
                                + to_string(locationOffset.value) + "]");
            }
            pop();
        } else {
            if (canImmediateValue(locationOffset.value)) {
                if (expressionValue.is_register) {
                    writeWithIndent("str r" + to_string(expressionValue.value)
                                    + ", [r11, #"
                                    + to_string(locationOffset.value) + "]");
                    pop();
                } else {
                    writeWithIndent("str r0, [r11, #"
                                    + to_string(locationOffset.value) + "]");
                }
            } else {
                writeWithIndent("ldr r1, =" + to_string(locationOffset.value));
                if (expressionValue.is_register) {
                    writeWithIndent("str r" + to_string(expressionValue.value)
                                    + ", [r11, r1]");
                    pop();
                } else {
                    writeWithIndent("str r0, [r11, r1]");
                }
            }
        }
    }
}

/*void CodeGenerator::processAssign(const shared_ptr<Assign> &assign) {

    // We need to resolve our target offset.
    resolveLocationOffset(assign->getLocation());

    // We need to know if we're assigning integers, arrays, or records.
    auto type = getLocationType(assign->getLocation());
    auto record = dynamic_pointer_cast<Record>(type);
    auto array = dynamic_pointer_cast<Array>(type);

    if (record != 0 || array != 0) {
        unsigned long long numBytesToMove = typeSizes[type];
        auto origin_location = dynamic_pointer_cast<Location>(
                assign->getExpression());

        resolveLocationOffset(origin_location);

        // Pop the origin offset.
        writeWithIndent("pop {r0}");
        // Calculate origin address.
        writeWithIndent("add r0, r0, r11");
        // Pop the destination offset.
        writeWithIndent("pop {r1}");
        writeWithIndent("add r1, r1, r11");

        for (unsigned long long currentOffset = 0;
             currentOffset < numBytesToMove;
             currentOffset = currentOffset + 4) {

            writeWithIndent("ldr r2, =" + to_string(currentOffset));
            writeWithIndent("add r0, r0, r2");
            writeWithIndent("add r1, r1, r2");

            writeWithIndent("ldr r3, [r0]");
            writeWithIndent("str r3, [r1]");
        }

        // We're aligned to 4 bytes so we can copy everything in 32-bit chunks.

    } else {
        // It's an integer.
        resolveExpressionValue(assign->getExpression());
        writeWithIndent("pop {r0}");
        writeWithIndent("pop {r1}");
        writeWithIndent("add r2, r1, r11");
        writeWithIndent("str r0, [r2]");
    }

}*/

void CodeGenerator::processIfInstruction(
        const shared_ptr<IfInstruction> &ifInstruction) {
    int labelSuffix = getNextLabelIndex();
    string instructions_false = "if_false_begin_" + to_string(labelSuffix);
    string instructions_end = "if_end_" + to_string(labelSuffix);

    resolveCondition(ifInstruction->getCondition());
    writeWithIndent("pop {r0}");
    writeWithIndent("cmp r0, #1");
    writeWithIndent("bne " + instructions_false);

    processInstructions(ifInstruction->getInstructionsTrue());

    writeWithIndent("b " + instructions_end);

    deindent();
    writeWithIndent(instructions_false + ":");
    indent();

    processInstructions(ifInstruction->getInstructionsFalse());

    deindent();
    writeWithIndent(instructions_end + ":");
    indent();

}

void CodeGenerator::processRepeat(const shared_ptr<Repeat> &repeat) {

    int labelSuffix = getNextLabelIndex();
    string loop_begin_label = "repeat_begin_" + to_string(labelSuffix);

    deindent();
    writeWithIndent(loop_begin_label + ":");
    indent();
    processInstructions(repeat->getInstructions());
    resolveCondition(repeat->getCondition());

    writeWithIndent("pop {r0}");
    writeWithIndent("cmp r0, #1");
    writeWithIndent("bne " + loop_begin_label);

}

void CodeGenerator::processRead(const shared_ptr<Read> &read) {

    auto location = resolveLocationOffset(read->getLocation());
    writeWithIndent("ldr r0, =input_format");

    // Put address into r1.
    if (location.is_register) {
        writeWithIndent("add r1, r" + to_string(location.value) + ", r11");
        pop();
    } else {
        if (canImmediateValue(location.value)) {
            writeWithIndent("add r1, r11, #" + to_string(location.value));
        } else {
            writeWithIndent("ldr r1, =" + to_string(location.value));
            writeWithIndent("add r1, r1, r11");
        }
    }

    // Don't want to trash these registers.
    writeWithIndent("push {r2-r3}");
    writeWithIndent("bl __isoc99_scanf");
    writeWithIndent("pop {r2-r3}");
}

void CodeGenerator::processWrite(const shared_ptr<Write>& write) {

    auto expression = resolveExpressionValue(write->getExpression());
    writeWithIndent("ldr r0, =output_format");


    if (expression.is_register) {
        writeWithIndent("mov r1, r" + to_string(expression.value));
        pop();
    } else {
        if (canImmediateValue(expression.value)) {
            writeWithIndent("mov r1, #" + to_string(expression.value));
        } else {
            writeWithIndent("ldr r1, =" + to_string(expression.value));
        }
    }

    // We need to not trash our stack registers when we bl into the function.
    writeWithIndent("push {r2-r3}");
    writeWithIndent("bl printf");
    writeWithIndent("pop {r2-r3}");

}

void CodeGenerator::resolveCondition(
        const shared_ptr<Condition> &condition) {
    resolveExpressionValue(condition->getExpressionLeft());
    resolveExpressionValue(condition->getExpressionRight());
    // Pop right, pop left
    writeWithIndent("pop {r1}");
    writeWithIndent("pop {r0}");

    writeWithIndent("cmp r0, r1");

    if (condition->getLabel() == "=") {
        writeWithIndent("ldrne r3, =0");
        writeWithIndent("ldreq r3, =1");
    } else if (condition->getLabel() == "#") {
        writeWithIndent("ldrne r3, =1");
        writeWithIndent("ldreq r3, =0");
    } else if (condition->getLabel() == "<") {
        writeWithIndent("ldrlt r3, =1");
        writeWithIndent("ldrge r3, =0");
    } else if (condition->getLabel() == "<=") {
        writeWithIndent("ldrle r3, =1");
        writeWithIndent("ldrgt r3, =0");
    } else if (condition->getLabel() == ">") {
        writeWithIndent("ldrgt r3, =1");
        writeWithIndent("ldrle r3, =0");
    } else if (condition->getLabel() == ">=") {
        writeWithIndent("ldrge r3, =1");
        writeWithIndent("ldrlt r3, =0");
    }

    writeWithIndent("push {r3}");
}

CodeGenerationMessage CodeGenerator::resolveLocationOffset(
        const shared_ptr<Location> &location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        // If variable, we can just look up in the symbol table.
        auto variableEntry = dynamic_pointer_cast<Variable>(
                symbolTable.getCurrentScope()->getEntry
                        (variable->getIdentifier()));
        return CodeGenerationMessage(true, false, variableEntry->getOffset());
    } else if (index != 0) {
        auto locationOffset = resolveLocationOffset(index->getLocation());
        auto expressionValue = resolveExpressionValue(index->getExpression());

        auto array = dynamic_pointer_cast<Array>(
                getLocationType(index->getLocation()));
        auto elementSize = typeSizes[array->getType()];

        if (expressionValue.is_register) {
            // If it's a register, let's check the index.
            if (canImmediateValue(array->getLength())) {
                writeWithIndent("cmp r" + to_string(expressionValue.value)
                                + ", #" + to_string(array->getLength()));
            } else {
                writeWithIndent("ldr r0, =" + to_string(array->getLength()));
                writeWithIndent("cmp r" + to_string(expressionValue.value)
                                + ", r0");
            }
            writeWithIndent("bhs array_out_of_bounds");
        } else {
            // If index is constant
            if (expressionValue.value < 0
                || expressionValue.value > array->getLength()) {
                throw CodeGenerationException("Array index out of bounds!");
            }
        }

        if (locationOffset.is_register) {
            if (expressionValue.is_register) {

                if (canImmediateValue(elementSize)) {
                    writeWithIndent("mov r0, #" + to_string(elementSize));
                } else {
                    writeWithIndent("ldr r0, =" + to_string(elementSize));
                }
                writeWithIndent("mul r" + to_string(expressionValue.value)
                                + ", r" + to_string(expressionValue.value)
                                + ", r0");
                writeWithIndent("add r" + to_string(locationOffset.value)
                                + ", r" + to_string(locationOffset.value)
                                + ", r" + to_string(expressionValue.value));
                pop();
            } else {
                auto newOffset = expressionValue.value * elementSize;
                if (canImmediateValue(newOffset)) {
                    writeWithIndent("add r" + to_string(locationOffset.value)
                                    + ", r" + to_string(locationOffset.value)
                                    + ", #" + to_string(newOffset));
                } else {
                    writeWithIndent("ldr r0, =" + to_string(newOffset));
                    writeWithIndent("add r" + to_string(locationOffset.value)
                                    + ", r" + to_string(locationOffset.value) +
                                            ", r0");
                }
            }
            return CodeGenerationMessage(true, true, locationOffset.value);

        } else {
            // The offset is a constant if the index is a constant.
            if (expressionValue.is_register) {
                // Need to calculate new offset
                if (canImmediateValue(elementSize)) {
                    writeWithIndent("mov r0, #" + to_string(elementSize));
                } else {
                    writeWithIndent("ldr r0, =" + to_string(elementSize));
                }
                writeWithIndent("mul r" + to_string(expressionValue.value)
                                + ", r" + to_string(expressionValue.value)
                                + ", r0");
                if (canImmediateValue(locationOffset.value)) {
                    writeWithIndent("add r" + to_string(expressionValue.value)
                                    + ", r" + to_string(expressionValue.value)
                                    + ", #" + to_string(locationOffset.value));
                } else {
                    writeWithIndent("ldr r0, ="
                                    + to_string(locationOffset.value));
                    writeWithIndent("add r" + to_string(expressionValue.value)
                                    + ", r" + to_string(expressionValue.value)
                                    + ", r0");
                }
                return CodeGenerationMessage(true, true, expressionValue.value);
            } else {
                auto newOffset = locationOffset.value
                                 + expressionValue.value * elementSize;
                return CodeGenerationMessage(true, false, newOffset);
            }
        }

    } else if (field != 0) {
        auto locationOffset = resolveLocationOffset(field->getLocation());
        auto record = dynamic_pointer_cast<Record>(
                getLocationType(field->getLocation()));
        auto fieldOffset = dynamic_pointer_cast<Variable>(
                record->getScope()->getEntry(
                        field->getVariable()->getIdentifier()))->getOffset();

        if (locationOffset.is_register) {
            if (canImmediateValue(fieldOffset)) {
                writeWithIndent("add r" + to_string(locationOffset.value)
                                + ", r" + to_string(locationOffset.value)
                                + ", #" + to_string(fieldOffset));
            } else {
                writeWithIndent("ldr r0, =" + to_string(fieldOffset));
                writeWithIndent("add r" + to_string(locationOffset.value)
                                + ", r" + to_string(locationOffset.value)
                                + ", r0");
            }
            return CodeGenerationMessage(true, true, locationOffset.value);
        } else {
            auto newOffset = locationOffset.value + fieldOffset;
            return CodeGenerationMessage(true, false, newOffset);
        }
    }
}

/*void CodeGenerator::resolveLocationOffset(
        const shared_ptr<Location> &location) {
    // Ultimately pushes an offset onto the stack.
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        // If we have a variable, we can find its offset from the symbol table.
        auto variableEntry = dynamic_pointer_cast<Variable>(
                symbolTable.getCurrentScope()->getEntry(
                        variable->getIdentifier()));
        writeWithIndent("ldr r3, =" + to_string(variableEntry->getOffset()));
    } else if (index != 0) {
        // If we have an index:
        // We need to to know offset of parent array and width of elements.
        resolveLocationOffset(index->getLocation());
        resolveExpressionValue(index->getExpression());
        // Pop index
        writeWithIndent("pop {r0}");
        // Pop offset of parent array.
        writeWithIndent("pop {r1}");

        // Calculate what we have to add to this offset.
        auto array = dynamic_pointer_cast<Array>(
                getLocationType(index->getLocation()));
        unsigned long long elementSize = typeSizes[array->getType()];

        // Check if negative
        writeWithIndent("cmp r0, #0");
        writeWithIndent("blt array_out_of_bounds");
        writeWithIndent("ldr r2, =" + to_string(array->getLength()));
        writeWithIndent("cmp r0, r2");
        writeWithIndent("bge array_out_of_bounds");

        writeWithIndent("ldr r2, =" + to_string(elementSize));
        writeWithIndent("mul r3, r0, r2");
        writeWithIndent("add r3, r1, r3");
    } else if (field != 0) {
        // If we have a field:
        // We need to know offset of parent record and variable offset.
        resolveLocationOffset(field->getLocation());
        // Pop offset of parent record.
        writeWithIndent("pop {r0}");

        auto record = dynamic_pointer_cast<Record>(
                getLocationType(field->getLocation()));
        auto fieldVariable = dynamic_pointer_cast<Variable>(
                record->getScope()->getEntry(
                        field->getVariable()->getIdentifier()));
        writeWithIndent("ldr r1, =" + to_string(fieldVariable->getOffset()));
        writeWithIndent("add r3, r0, r1");
    }
    writeWithIndent("push {r3}");

}*/

// Copied verbatim from Parser.cpp
shared_ptr<Type> CodeGenerator::getLocationType(
        const shared_ptr<Location> &location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        auto entry = symbolTable.getCurrentScope()->getEntry(
                variable->getIdentifier());
        auto variableEntry = dynamic_pointer_cast<Variable>(entry);
        auto constEntry = dynamic_pointer_cast<Constant>(entry);
        if (variableEntry != 0) {
            return variableEntry->getType();
        } else if (constEntry != 0) {
            return constEntry->getType();
        }
    } else if (index != 0) {
        auto parentArray = dynamic_pointer_cast<Array>(
                getLocationType(index->getLocation()));
        if (parentArray != 0) {
            return parentArray->getType();
        }

    } else if (field != 0) {
        auto parentRecord = dynamic_pointer_cast<Record>(
                getLocationType(field->getLocation()));
        if (parentRecord != 0) {
            auto fieldEntry = dynamic_pointer_cast<Variable>(
                    parentRecord->getScope()->getEntry(
                            field->getVariable()->getIdentifier()));
            if (fieldEntry != 0) {
                return fieldEntry->getType();
            }
        }
    }

    return 0;
}

CodeGenerationMessage CodeGenerator::resolveExpressionValue(
        const shared_ptr<Expression> &expression)
        throw (CodeGenerationException) {

    auto number = dynamic_pointer_cast<NumberExpression>(expression);
    auto location = dynamic_pointer_cast<Location>(expression);
    auto binary = dynamic_pointer_cast<BinaryExpression>(expression);

    if (number != 0) {
        auto value = number->getValue();
        if (value > 2147483647 || value < -2147483648) {
            throw CodeGenerationException(
                    "Value of constant can't be held in 32 bits!");
        }
        return CodeGenerationMessage(false, false, value);

    } else if (location != 0) {
        auto message = resolveLocationOffset(location);
        if (message.is_register) {
            writeWithIndent("ldr r" + to_string(message.value)
                            + ", [r" + to_string(message.value) + ", r11]");
            return CodeGenerationMessage(false, true, message.value);
        } else {
            // We need to ask for a new register to store the value.
            auto register_number = push();

            if (canImmediateValue(message.value)) {
                writeWithIndent("ldr r" + to_string(register_number)
                                + ", [r11, #" + to_string(message.value) + "]");
            } else {
                writeWithIndent("ldr r" + to_string(register_number)
                                + ", =" + to_string(message.value));
                writeWithIndent("ldr r" + to_string(register_number)
                                + ", [r11, r"
                                + to_string(register_number) + "]");
            }
            return CodeGenerationMessage(false, true, register_number);
        }
    } else if (binary != 0) {
        auto right = resolveExpressionValue(binary->getExpressionRight());
        auto left = resolveExpressionValue(binary->getExpressionLeft());

        if (left.is_register) {
            writeWithIndent("mov r0, r" + to_string(left.value));
            pop();
        } else {
            if (canImmediateValue(left.value)) {
                writeWithIndent("mov r0, #" + to_string(left.value));
            } else {
                writeWithIndent("ldr r0, =" + to_string(left.value));
            }
        }

        if (right.is_register) {
            writeWithIndent("mov r1, r" + to_string(right.value));
            pop();
        } else {
            if (canImmediateValue(right.value)) {
                writeWithIndent("mov r1, #" + to_string(right.value));
            } else {
                writeWithIndent("ldr r1, =" + to_string(right.value));
            }
        }

        auto register_number = push();

        if (binary->getOperation() == "+") {
            writeWithIndent("add r" + to_string(register_number) + ", r0, r1");
        } else if (binary->getOperation() == "-") {
            writeWithIndent("sub r" + to_string(register_number) + ", r0, r1");
        } else if (binary->getOperation() == "*") {
            writeWithIndent("mul r" + to_string(register_number) + ", r0, r1");
        } else if (binary->getOperation() == "DIV") {
            if (!right.is_register && right.value == 0) {
                throw CodeGenerationException("Dividing by zero!");
            }
            if (!right.is_register) {
                writeWithIndent("cmp r1, #0");
                writeWithIndent("beq div_zero");
            }
            writeWithIndent("bl __aeabi_idiv");
            writeWithIndent("mov r" + to_string(register_number) + ", r0");

        } else if (binary->getOperation() == "MOD") {
            if (!right.is_register && right.value == 0) {
                throw CodeGenerationException("Modulus by zero!");
            }
            if (!right.is_register) {
                writeWithIndent("cmp r1, #0");
                writeWithIndent("beq mod_zero");
            }

            writeWithIndent("bl __aeabi_idivmod");
            writeWithIndent("mov r" + to_string(register_number) + ", r1");
        }

        return CodeGenerationMessage(false, true, register_number);
    }
}

void CodeGenerator::indent() {
    indentLevel++;
}

void CodeGenerator::deindent() {
    indentLevel--;
}

void CodeGenerator::writeWithIndent(const string &value) {
    for (int c = 0; c < indentLevel; c++) {
        stream << "    ";
    }
    stream << value << endl;
}

int CodeGenerator::getNextLabelIndex() {
    return labelCounter++;
}

/**
 * If we want to push something into the register stack,
 * this tells us which register to put our value into.
 */
unsigned int CodeGenerator::push() {
    auto result = stackSize++ % numRegisters + startRegister;
    if (stackSize % numRegisters == numRegisters / 2
        && (stackSize / numRegisters > 0)) {
        stream << "push {r" << numRegisters / 2 + startRegister
        << "-r" << numRegisters + startRegister - 1 << "}" << endl;
    } else if (stackSize % numRegisters == 0 && stackSize > 0) {
        stream << "push {r" << startRegister << "-r"
        << numRegisters / 2 + startRegister - 1 << "}" << endl;
    }
    return result;
}

/**
 * If we want to pop something from the register stack, this tells us
 * which register our value is in.
 */
unsigned int CodeGenerator::pop() {
    if (stackSize % numRegisters == numRegisters / 2
        && stackSize / numRegisters > 0) {
        stream << "pop {r" << numRegisters / 2 + startRegister
        << "-r" << numRegisters + startRegister - 1 << "}" << endl;

    } else if (stackSize % numRegisters == 0 && stackSize > 0) {
        stream << "pop {r" << startRegister << "-r"
        << numRegisters / 2 + startRegister - 1 << "}" << endl;
    }

    auto result = --stackSize % numRegisters + startRegister;
    return result;
}


bool CodeGenerator::canImmediateValue(const int &value) {
    return value < 256 && value >= -256;
}

