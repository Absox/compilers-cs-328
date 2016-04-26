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
#include "ParameterVariable.h"
#include "LocalVariable.h"
#include "CallExpression.h"


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

    universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));

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
            auto local = dynamic_pointer_cast<LocalVariable>(variable);
            auto param = dynamic_pointer_cast<ParameterVariable>(variable);

            if (local != 0) {
                local->setOffset(nextOffset);
                unsigned long long size = getTypeSize(variable->getType());
                nextOffset += size;
            } else if (param != 0) {
                param->setOffset(nextOffset);
                nextOffset += 4;
            } else {
                variable->setOffset(nextOffset);
                unsigned long long size = getTypeSize(variable->getType());
                nextOffset += size;
            }
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

    processProcedures();

    writeWithIndent(".global main");
    deindent();

    writeWithIndent("main:");

    indent();
    writeWithIndent("push {fp, lr}");
    if (totalBytes > 0) writeWithIndent("ldr r11, =variables");

    writeWithIndent("");
}

void CodeGenerator::processProcedures() {
    auto identifiers = symbolTable.getCurrentScope()->getIdentifiersSorted();
    for (auto c = 0; c < identifiers.size(); c++) {
        auto procedure = dynamic_pointer_cast<Procedure>(
                symbolTable.getCurrentScope()->getEntry(identifiers[c]));
        if (procedure != 0) {
            processProcedure(identifiers[c], procedure);
        }
    }
}

void CodeGenerator::processProcedure(
        const std::string &identifier,
        const std::shared_ptr<Procedure> &procedure) {
    deindent();
    writeWithIndent(identifier + ":");
    indent();

    // Calculate scope offsets.
    auto total_proc_scope_size = calculateScopeOffsets(procedure->scope);
    // Move the parameters into their offsets.
    for (auto c = 3; c < procedure->parameters.size(); c++) {
        auto register_number = push();
        writeWithIndent("pop {r" + to_string(register_number) + "}");
    }

    writeWithIndent("push {lr}");

    if (canImmediateValue(total_proc_scope_size)) {
        writeWithIndent("sub sp, sp, #" + to_string(total_proc_scope_size));
    } else {
        auto scratch_register = push();
        writeWithIndent("ldr r" + to_string(scratch_register) + ", ="
                        + to_string(total_proc_scope_size));
        writeWithIndent("sub sp, sp, r" + to_string(scratch_register));
        pop();
    }

    for (auto c = 0; c < procedure->parameters.size(); c++) {
        auto offset = dynamic_pointer_cast<ParameterVariable>(
                procedure->scope->getEntry(
                        procedure->parameters[c]->identifier))->getOffset();
        if (c < 3) {
            writeWithIndent("str r" + to_string(c)
                            + ", [sp, #" + to_string(offset) + "]");
        } else {
            auto register_number = pop();
            writeWithIndent("str r" + to_string(register_number)
                            + ", [sp, #" + to_string(offset) + "]");
        }
    }

    // Initialize local variables with zeros.
    writeWithIndent("mov r0, #0");
    auto identifiers = procedure->scope->getIdentifiersSorted();
    for (auto c = 0; c < identifiers.size(); c++) {
        auto localVariable = dynamic_pointer_cast<LocalVariable>(
                procedure->scope->getEntry(identifiers[c]));
        if (localVariable != 0) {
            auto offset = localVariable->getOffset();
            auto var_size = getTypeSize(localVariable->getType());
            for (auto d = 0; d < var_size; d += 4) {
                writeWithIndent("str r0, [sp, #" + to_string(offset + d) + "]");
            }
        }
    }

    symbolTable.setCurrentScope(procedure->scope);

    processInstructions(procedure->instructions);

    if (procedure->return_type != 0) {
        auto return_value = resolveExpressionValue(procedure->return_expression);
        if (return_value.is_register) {
            writeWithIndent("mov r0, r" + to_string(return_value.value));
            pop();
        } else {
            if (canImmediateValue(return_value.value)) {
                writeWithIndent("mov r0, #" + to_string(return_value.value));
            } else {
                writeWithIndent("lrd r0, =" + to_string(return_value.value));
            }
        }
    }

    symbolTable.setCurrentScope(procedure->scope->getOuter());

    if (canImmediateValue(total_proc_scope_size)) {
        writeWithIndent("add sp, sp, #" + to_string(total_proc_scope_size));
    } else {
        writeWithIndent("ldr r1, =" + to_string(total_proc_scope_size));
        writeWithIndent("add sp, sp, r1");
    }
    writeWithIndent("pop {lr}");
    writeWithIndent("bx lr");
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
    auto call = dynamic_pointer_cast<Call>(instruction);

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
    } else if (call != 0) {
        writeWithIndent("@Call " + call->identifier + "()");
        processCall(call);
    }
    // Pad with extra line.
    writeWithIndent("");
}


void CodeGenerator::processCall(const std::shared_ptr<Call> &call) {
    for (auto c = 0; c < call->procedure->parameters.size(); c++) {
        auto parameter_type = call->procedure->parameters[c]->type;
        if (parameter_type == universalInt) {
            writeWithIndent("@parameter is numeric");
            auto parameter = resolveExpressionValue(call->actuals[c]);

            if (parameter.is_register) {
                if (c < 3) {
                    writeWithIndent(
                            "mov r" + to_string(c) + ", r"
                            + to_string(parameter.value));
                } else {
                    writeWithIndent(
                            "push {r" + to_string(parameter.value) + "}");
                }
                pop();
            } else {
                if (c < 3) {
                    if (canImmediateValue(parameter.value)) {
                        writeWithIndent("mov r" + to_string(c)
                                        + ", #" + to_string(parameter.value));
                    } else {
                        writeWithIndent("lrd r" + to_string(c)
                                        + ", =" + to_string(parameter.value));
                    }
                } else {
                    auto spare_register = push();
                    if (canImmediateValue(parameter.value)) {
                        writeWithIndent("mov r" + to_string(spare_register) + ", #" + to_string(parameter.value));
                    } else {
                        writeWithIndent("ldr r" + to_string(spare_register) + ", =" + to_string(parameter.value));
                    }
                    writeWithIndent("push {" +to_string(spare_register) + "}");
                    pop();
                }
            }
        } else {
            writeWithIndent("@parameter is array or record");
            auto location = dynamic_pointer_cast<Location>(call->actuals[c]);
            auto parameter = resolveLocationOffset(location);
            if (parameter.is_register) {
                if (c < 3) {

                } else {

                }
                pop();
            } else {

            }
            // TODO
        }
    }

    writeWithIndent("bl " + call->identifier);
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
        // Move origin address into r0
        if (origin_location.is_register) {
            writeWithIndent("add r0, r11, r"
                            + to_string(origin_location.value));
            pop();
        } else {
            if (canImmediateValue(origin_location.value)) {
                writeWithIndent("add r0, r11, #"
                                + to_string(origin_location.value));
            } else {
                writeWithIndent("ldr r0, =" + to_string(origin_location.value));
                writeWithIndent("add r0, r0, r11");
            }
        }

        unsigned int destination_register = 0;
        if (locationOffset.is_register) {
            destination_register = locationOffset.value;
            writeWithIndent("add r" + to_string(destination_register)
                            + ", r" + to_string(destination_register)
                            + ", r11");
        } else {
            destination_register = push();
            if (canImmediateValue(locationOffset.value)) {
                writeWithIndent("add r" + to_string(destination_register)
                                + ", r11, #"
                                + to_string(locationOffset.value));
            } else {
                writeWithIndent("ldr r" + to_string(destination_register)
                                + ", =" + to_string(locationOffset.value));
                writeWithIndent("add r" + to_string(destination_register)
                                + ", r" + to_string(destination_register)
                                + ", r11");
            }
        }

        for (unsigned long long c = 0; c < size; c = c + 4) {
            if (canImmediateValue(size)) {
                writeWithIndent("ldr r1, [r0, #" + to_string(c) + "]");
                writeWithIndent("str r1, [r" + to_string(destination_register)
                                + ", #" + to_string(c) + "]");
            } else {
                writeWithIndent("ldr r1, [r0]");
                writeWithIndent("str r1, [r" + to_string(destination_register)
                                + "]");
                writeWithIndent("add r0, r0, #4");
                writeWithIndent("add r" + to_string(destination_register)
                                + ", r" + to_string(destination_register)
                                + ", #4");
            }
        }
        pop();

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
                if (locationOffset.is_stack) {
                    writeWithIndent(
                            "str r" + to_string(expressionValue.value)
                            + ", [sp, r" + to_string(locationOffset.value)
                            + "]");
                } else {
                    writeWithIndent("str r" + to_string(expressionValue.value)
                                    + ", [r11, r" + to_string(locationOffset.value)
                                    + "]");
                }
                pop();
            } else {
                if (locationOffset.is_stack) {
                    writeWithIndent("str r0, [sp, r"
                                    + to_string(locationOffset.value) + "]");
                } else {
                    writeWithIndent("str r0, [r11, r"
                                    + to_string(locationOffset.value) + "]");
                }

            }
            pop();
        } else {
            if (canImmediateValue(locationOffset.value)) {
                if (expressionValue.is_register) {
                    if (locationOffset.is_stack) {
                        writeWithIndent(
                                "str r" + to_string(expressionValue.value)
                                + ", [sp, #" + to_string(locationOffset.value)
                                + "]");
                    } else {
                        writeWithIndent("str r" + to_string(expressionValue.value)
                                        + ", [r11, #"
                                        + to_string(locationOffset.value) + "]");
                    }

                    pop();
                } else {
                    if (locationOffset.is_stack) {
                        writeWithIndent("str r0, [sp, #"
                                        + to_string(locationOffset.value) + "]");
                    } else {
                        writeWithIndent("str r0, [r11, #"
                                        + to_string(locationOffset.value) + "]");
                    }

                }
            } else {
                writeWithIndent("ldr r1, =" + to_string(locationOffset.value));
                if (expressionValue.is_register) {
                    if (locationOffset.is_stack) {
                        writeWithIndent("str r" + to_string(expressionValue.value)
                                        + ", [sp, r1]");
                    } else {
                        writeWithIndent("str r" + to_string(expressionValue.value)
                                        + ", [r11, r1]");
                    }

                    pop();
                } else {
                    if (locationOffset.is_stack) {
                        writeWithIndent("str r0, [sp, r1]");
                    } else {
                        writeWithIndent("str r0, [r11, r1]");

                    }
                }
            }
        }
    }
}

void CodeGenerator::processIfInstruction(
        const shared_ptr<IfInstruction> &ifInstruction) {
    auto condition = resolveCondition(ifInstruction->getCondition());
    if (condition.is_register) {
        int labelSuffix = getNextLabelIndex();
        string instructions_false = "if_false_begin_" + to_string(labelSuffix);
        string instructions_end = "if_end_" + to_string(labelSuffix);

        writeWithIndent("cmp r" + to_string(condition.value) + ", #1");
        writeWithIndent("bne " + instructions_false);
        pop();

        processInstructions(ifInstruction->getInstructionsTrue());
        writeWithIndent("b " + instructions_end);
        deindent();
        writeWithIndent(instructions_false + ":");
        indent();
        processInstructions(ifInstruction->getInstructionsFalse());
        deindent();
        writeWithIndent(instructions_end + ":");
        indent();
    } else {
        // If our condition is constant, we need only generate one or the other.
        if (condition.value) {
            processInstructions(ifInstruction->getInstructionsTrue());
        } else {
            processInstructions(ifInstruction->getInstructionsFalse());
        }
    }
}

void CodeGenerator::processRepeat(const shared_ptr<Repeat> &repeat) {

    int labelSuffix = getNextLabelIndex();
    string loop_begin_label = "repeat_begin_" + to_string(labelSuffix);

    deindent();
    writeWithIndent(loop_begin_label + ":");
    indent();
    processInstructions(repeat->getInstructions());

    auto condition = resolveCondition(repeat->getCondition());

    if (condition.is_register) {
        writeWithIndent("cmp r" + to_string(condition.value) + ", #1");
        writeWithIndent("bne " + loop_begin_label);
        pop();
    } else {
        if (!condition.value) {
            writeWithIndent("b " + loop_begin_label);
        }
    }
}

void CodeGenerator::processRead(const shared_ptr<Read> &read) {

    auto location = resolveLocationOffset(read->getLocation());
    writeWithIndent("ldr r0, =input_format");

    // Put address into r1.
    if (location.is_register) {
        if (location.is_stack) {
            writeWithIndent("add r1, r" + to_string(location.value) + ", sp");
        } else {
            writeWithIndent("add r1, r" + to_string(location.value) + ", r11");
        }
        pop();
    } else {
        if (canImmediateValue(location.value)) {
            if (location.is_stack) {
                writeWithIndent("add r1, sp, #" + to_string(location.value));
            } else {
                writeWithIndent("add r1, r11, #" + to_string(location.value));
            }
        } else {
            writeWithIndent("ldr r1, =" + to_string(location.value));
            if (location.is_stack) {
                writeWithIndent("add r1, r1, sp");
            } else {
                writeWithIndent("add r1, r1, r11");
            }
        }
    }

    // Don't want to trash these registers.
    writeWithIndent("bl __isoc99_scanf");
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
    writeWithIndent("bl printf");

}

CodeGenerationMessage CodeGenerator::resolveCondition(
        const shared_ptr<Condition>& condition) {
    auto right = resolveExpressionValue(condition->getExpressionRight());
    auto left = resolveExpressionValue(condition->getExpressionLeft());

    if (left.is_register) {
        if (right.is_register) {
            writeWithIndent("cmp r" + to_string(left.value)
                            + ", r" + to_string(right.value));
            pop();
        } else {
            if (canImmediateValue(right.value)) {
                writeWithIndent("cmp r" + to_string(left.value)
                                + ", #" + to_string(right.value));
            } else {
                writeWithIndent("ldr r0, =" + to_string(right.value));
                writeWithIndent("cmp r" + to_string(left.value) + ", r"
                                + to_string(right.value));
            }
        }
        if (condition->getLabel() == "=") {
            writeWithIndent("moveq r" + to_string(left.value) + ", #1");
            writeWithIndent("movne r" + to_string(left.value) + ", #0");
        } else if (condition->getLabel() == "#") {
            writeWithIndent("movne r" + to_string(left.value) + ", #1");
            writeWithIndent("moveq r" + to_string(left.value) + ", #0");
        } else if (condition->getLabel() == "<") {
            writeWithIndent("movlt r" + to_string(left.value) + ", #1");
            writeWithIndent("movge r" + to_string(left.value) + ", #0");
        } else if (condition->getLabel() == "<=") {
            writeWithIndent("movle r" + to_string(left.value) + ", #1");
            writeWithIndent("movgt r" + to_string(left.value) + ", #0");
        } else if (condition->getLabel() == ">") {
            writeWithIndent("movgt r" + to_string(left.value) + ", #1");
            writeWithIndent("movle r" + to_string(left.value) + ", #0");
        } else if (condition->getLabel() == ">=") {
            writeWithIndent("movge r" + to_string(left.value) + ", #1");
            writeWithIndent("movlt r" + to_string(left.value) + ", #0");
        }
        return CodeGenerationMessage(false, true, left.value);
    } else {
        if (right.is_register) {
            if (canImmediateValue(left.value)) {
                writeWithIndent("cmp r" + to_string(right.value)
                                + ", #" + to_string(left.value));
            } else {
                writeWithIndent("ldr r0, =" + to_string(left.value));
                writeWithIndent("cmp r" + to_string(right.value) + ", r0");
            }

            if (condition->getLabel() == "=") {
                writeWithIndent("moveq r" + to_string(right.value) + ", #1");
                writeWithIndent("movne r" + to_string(right.value) + ", #0");
            } else if (condition->getLabel() == "#") {
                writeWithIndent("movne r" + to_string(right.value) + ", #1");
                writeWithIndent("moveq r" + to_string(right.value) + ", #0");
            } else if (condition->getLabel() == "<") {
                writeWithIndent("movgt r" + to_string(right.value) + ", #1");
                writeWithIndent("movle r" + to_string(right.value) + ", #0");
            } else if (condition->getLabel() == "<=") {
                writeWithIndent("movge r" + to_string(right.value) + ", #1");
                writeWithIndent("movlt r" + to_string(right.value) + ", #0");
            } else if (condition->getLabel() == ">") {
                writeWithIndent("movlt r" + to_string(right.value) + ", #1");
                writeWithIndent("movge r" + to_string(right.value) + ", #0");
            } else if (condition->getLabel() == ">=") {
                writeWithIndent("movle r" + to_string(right.value) + ", #1");
                writeWithIndent("movgt r" + to_string(right.value) + ", #0");
            }
            return CodeGenerationMessage(false, true, right.value);
        } else {
            // If both sides are constant, we can return a constant.
            if (condition->getLabel() == "=") {
                if (left.value == right.value) {
                    return CodeGenerationMessage(false, false, 1);
                } else {
                    return CodeGenerationMessage(false, false, 0);
                }
            } else if (condition->getLabel() == "#") {
                if (left.value != right.value) {
                    return CodeGenerationMessage(false, false, 1);
                } else {
                    return CodeGenerationMessage(false, false, 0);
                }
            } else if (condition->getLabel() == "<") {
                if (left.value < right.value) {
                    return CodeGenerationMessage(false, false, 1);
                } else {
                    return CodeGenerationMessage(false, false, 0);
                }
            } else if (condition->getLabel() == "<=") {
                if (left.value <= right.value) {
                    return CodeGenerationMessage(false, false, 1);
                } else {
                    return CodeGenerationMessage(false, false, 0);
                }
            } else if (condition->getLabel() == ">") {
                if (left.value > right.value) {
                    return CodeGenerationMessage(false, false, 1);
                } else {
                    return CodeGenerationMessage(false, false, 0);
                }
            } else if (condition->getLabel() == ">=") {
                if (left.value >= right.value) {
                    return CodeGenerationMessage(false, false, 1);
                } else {
                    return CodeGenerationMessage(false, false, 0);
                }
            }
        }
    }
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
        auto isLocal = dynamic_pointer_cast<LocalVariable>(variableEntry);
        auto isParam = dynamic_pointer_cast<ParameterVariable>(variableEntry);

        auto stack = (isLocal != 0 || isParam != 0);

        if (stack) writeWithIndent("@Local variable resolved!");
        return CodeGenerationMessage(true, false, variableEntry->getOffset(), stack);
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

                if (powerOfTwo(elementSize)) {
                    auto exponent = logTwo(elementSize);
                    writeWithIndent("add r" + to_string(locationOffset.value)
                                    + ", r" + to_string(locationOffset.value)
                                    + ", r" + to_string(expressionValue.value)
                                    + ", LSL #" + to_string(exponent));
                } else {
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
                }

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

    throw CodeGenerationException("End of control. Unrecognized location");
}

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
    auto call = dynamic_pointer_cast<CallExpression>(expression);

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
            if (message.is_stack) {
                writeWithIndent("ldr r" + to_string(message.value)
                                + ", [sp, r" + to_string(message.value) + "]");
            } else {
                writeWithIndent("ldr r" + to_string(message.value)
                                + ", [r" + to_string(message.value) + ", r11]");
            }
            return CodeGenerationMessage(false, true, message.value);
        } else {
            // We need to ask for a new register to store the value.
            auto register_number = push();

            if (canImmediateValue(message.value)) {
                if (message.is_stack) {
                    writeWithIndent(
                            "ldr r" + to_string(register_number)
                            + ", [sp, #" + to_string(message.value) + "]");
                } else {
                    writeWithIndent(
                            "ldr r" + to_string(register_number)
                            + ", [r11, #" + to_string(message.value) + "]");
                }

            } else {
                writeWithIndent("ldr r" + to_string(register_number)
                                + ", =" + to_string(message.value));
                if (message.is_stack) {
                    writeWithIndent("ldr r" + to_string(register_number)
                                    + ", [sp, r" + to_string(register_number)
                                    + "]");
                } else {
                    writeWithIndent("ldr r" + to_string(register_number)
                                    + ", [r11, r"
                                    + to_string(register_number) + "]");
                }

            }
            return CodeGenerationMessage(false, true, register_number);
        }
    } else if (binary != 0) {
        auto left = resolveExpressionValue(binary->getExpressionLeft());
        auto right = resolveExpressionValue(binary->getExpressionRight());

        if (binary->getOperation() == "+") {

            if (left.is_register) {
                if (right.is_register) {
                    writeWithIndent("add r" + to_string(left.value)
                                    + ", r" + to_string(left.value)
                                    + ", r" + to_string(right.value));
                    pop();
                    return CodeGenerationMessage(false, true, left.value);
                } else {
                    // No point in generating code for an addition by 0.
                    if (right.value != 0) {
                        if (canImmediateValue(right.value)) {
                            writeWithIndent("add r" + to_string(left.value)
                                            + ", r" + to_string(left.value)
                                            + ", #" + to_string(right.value));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(right.value));
                            writeWithIndent("add r" + to_string(left.value)
                                            + ", r" + to_string(left.value)
                                            + ", r0");
                        }
                    }
                    return CodeGenerationMessage(false, true, left.value);
                }
            } else {
                if (right.is_register) {
                    if (left.value != 0) {
                        if (canImmediateValue(left.value)) {
                            writeWithIndent("add r" + to_string(right.value)
                                            + ", r" + to_string(right.value)
                                            + ", #" + to_string(left.value));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(left.value));
                            writeWithIndent("add r" + to_string(right.value)
                                            + ", r" + to_string(right.value)
                                            + ", r0");
                        }
                    }
                    return CodeGenerationMessage(false, true, right.value);
                } else {
                    return CodeGenerationMessage(
                            false, false, left.value + right.value);
                }
            }

        } else if (binary->getOperation() == "-") {
            if (left.is_register) {
                if (right.is_register) {
                    writeWithIndent("sub r" + to_string(left.value)
                                    + ", r" + to_string(left.value)
                                    + ", r" + to_string(right.value));
                    pop();
                    return CodeGenerationMessage(false, true, left.value);
                } else {
                    if (right.value != 0) {
                        if (canImmediateValue(right.value)) {
                            writeWithIndent("sub r" + to_string(left.value)
                                            + ", r" + to_string(left.value)
                                            + ", #" + to_string(right.value));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(right.value));
                            writeWithIndent("sub r" + to_string(left.value)
                                            + ", r" + to_string(left.value)
                                            + ", r0");
                        }
                    }
                    return CodeGenerationMessage(false, true, left.value);
                }
            } else {
                if (right.is_register) {
                    if (canImmediateValue(left.value)) {
                        writeWithIndent("mov r0, #" + to_string(left.value));
                    } else {
                        writeWithIndent("ldr r0, =" + to_string(left.value));
                    }
                    writeWithIndent("sub r" + to_string(right.value)
                                    + ", r0, " + to_string(right.value));
                    return CodeGenerationMessage(false, true, right.value);
                } else {
                    return CodeGenerationMessage(
                            false, false, left.value - right.value);
                }

            }

        } else if (binary->getOperation() == "*") {
            if (left.is_register) {
                if (right.is_register) {
                    writeWithIndent("mul r" + to_string(left.value)
                                    + ", r" + to_string(left.value)
                                    + ", r" + to_string(right.value));
                    pop();
                    return CodeGenerationMessage(false, true, left.value);
                } else {
                    if (right.value == 0) {
                        pop();
                        return CodeGenerationMessage(false, false, 0);
                    } else if (right.value == 1) {
                    } else if (powerOfTwo(right.value)) {
                        writeWithIndent("mov r" + to_string(left.value)
                                        + ", r" + to_string(left.value)
                                        + ", LSL #"
                                        + to_string(logTwo(right.value)));
                    } else {
                        if (canImmediateValue(right.value)) {
                            writeWithIndent("mov r0, #" + to_string(right.value));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(right.value));
                        }
                        writeWithIndent("mul r" + to_string(left.value)
                                        + ", r" + to_string(left.value)
                                        + ", r0");
                    }
                    return CodeGenerationMessage(false, true, left.value);
                }
            } else {
                if (right.is_register) {
                    if (left.value == 0) {
                        pop();
                        return CodeGenerationMessage(false, false, 0);
                    } else {
                        if (left.value == 1) {
                        } else if (powerOfTwo(left.value)) {
                            writeWithIndent("mov r" + to_string(right.value)
                                            + ", r" + to_string(right.value)
                                            + ", LSL #"
                                            + to_string(logTwo(left.value)));
                        } else {
                            if (canImmediateValue(left.value)) {
                                writeWithIndent("mov r0, #"
                                                + to_string(left.value));
                            } else {
                                writeWithIndent("ldr r0, ="
                                                + to_string(left.value));
                            }
                            writeWithIndent("mul r" + to_string(right.value)
                                            + ", r" + to_string(right.value)
                                            + ", r0");
                        }
                        return CodeGenerationMessage(false, true, right.value);
                    }
                } else {
                    return CodeGenerationMessage(
                            false, false, left.value * right.value);
                }
            }
        } else if (binary->getOperation() == "DIV") {
            if (left.is_register) {
                if (right.is_register) {
                    writeWithIndent("mov r0, r" + to_string(left.value));
                    writeWithIndent("cmp r" + to_string(right.value) + ", #0");
                    writeWithIndent("beq div_zero");
                    writeWithIndent("mov r1, r" + to_string(right.value));
                    writeWithIndent("bl __aeabi_idiv");
                    writeWithIndent("mov r" + to_string(left.value) + ", r0");
                    pop();
                } else {
                    if (right.value == 0) {
                        throw CodeGenerationException("Dividing by zero");
                    } else if (right.value == 1) {
                    } else if (powerOfTwo(right.value)) {
                        // Note that we have to use arithmetic shift.
                        writeWithIndent("mov r" + to_string(left.value)
                                        + ", r" + to_string(left.value)
                                        + ", ASR #"
                                        + to_string(logTwo(right.value)));
                    } else {
                        writeWithIndent("mov r0, r" + to_string(left.value));
                        if (canImmediateValue(right.value)) {
                            writeWithIndent("mov r1, #"
                                            + to_string(right.value));
                        } else {
                            writeWithIndent("ldr r1, ="
                                            + to_string(right.value));
                        }
                        writeWithIndent("bl __aeabi_idiv");
                        writeWithIndent("mov r" + to_string(left.value)
                                        + ", r0");
                    }
                }
                return CodeGenerationMessage(false, true, left.value);

            } else {
                if (right.is_register) {
                    writeWithIndent("cmp r"
                                    + to_string(right.value) + ", #0");
                    writeWithIndent("beq div_zero");
                    if (left.value == 0) {
                        pop();
                        return CodeGenerationMessage(false, false, 0);
                    } else {
                        if (canImmediateValue(left.value)) {
                            writeWithIndent("mov r0, #"
                                            + to_string(left.value));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(left.value));
                        }
                        writeWithIndent("mov r1, r" + to_string(right.value));
                        writeWithIndent("bl __aeabi_idiv");
                        writeWithIndent("mov r" + to_string(right.value)
                                        + ", r0");
                        return CodeGenerationMessage(false, true, right.value);
                    }
                } else {
                    if (right.value == 0) {
                        throw CodeGenerationException("Divide by zero");
                    }
                    return CodeGenerationMessage(
                            false, false, left.value / right.value);
                }
            }
        } else if (binary->getOperation() == "MOD") {

            if (left.is_register) {
                if (right.is_register) {
                    writeWithIndent("mov r0, r" + to_string(left.value));
                    writeWithIndent("cmp r" + to_string(right.value) + ", #0");
                    writeWithIndent("beq mod_zero");
                    writeWithIndent("mov r1, r" + to_string(right.value));
                    writeWithIndent("bl __aeabi_idivmod");
                    writeWithIndent("mov r1, r" + to_string(left.value));
                    pop();
                } else {
                    if (right.value == 0) {
                        throw CodeGenerationException("Modulus by zero!");
                    } else if (powerOfTwo(right.value)) {
                        if (canImmediateValue(right.value - 1)) {
                            writeWithIndent("and r" + to_string(left.value)
                                            + ", r" + to_string(left.value)
                                            + ", #"
                                            + to_string(right.value - 1));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(right.value - 1));
                            writeWithIndent("and r" + to_string(left.value)
                                            + ", r" + to_string(right.value)
                                            + ", r0");
                        }
                    } else {
                        writeWithIndent("mov r0, r" + to_string(left.value));
                        if (canImmediateValue(right.value)) {
                            writeWithIndent("mov r1, #"
                                            + to_string(right.value));
                        } else {
                            writeWithIndent("ldr r1, ="
                                            + to_string(right.value));
                        }
                        writeWithIndent("bl __aeabi_idivmod");
                        writeWithIndent("mov r1, r" + to_string(left.value));
                    }
                }
                return CodeGenerationMessage(false, true, left.value);
            } else {
                if (right.is_register) {
                    writeWithIndent("cmp r"
                                    + to_string(right.value) + ", #0");
                    writeWithIndent("beq mod_zero");
                    if (left.value == 0) {
                        pop();
                        return CodeGenerationMessage(false, false, 0);
                    } else {
                        if (canImmediateValue(left.value)) {
                            writeWithIndent("mov r0, #"
                                            + to_string(left.value));
                        } else {
                            writeWithIndent("ldr r0, ="
                                            + to_string(left.value));
                        }
                        writeWithIndent("mov r1, r" + to_string(right.value));
                        writeWithIndent("bl __aeabi_idivmod");
                        writeWithIndent("mov r1, r" + to_string(right.value));
                    }
                    return CodeGenerationMessage(false, true, right.value);
                } else {
                    if (right.value == 0) {
                        throw CodeGenerationException("Modulus by zero!");
                    } else {
                        return CodeGenerationMessage(
                                false, false, left.value % right.value);
                    }
                }
            }
        }
    } else if (call != 0) {
        processCall(call->call);
        auto return_register = push();
        writeWithIndent("mov r" + to_string(return_register) + ", r0");
        return CodeGenerationMessage(false, true, return_register);
    }
}

void CodeGenerator::indent() {
    indentLevel++;
}

void CodeGenerator::deindent() {
    indentLevel--;
}

void CodeGenerator::writeWithIndent(const string &value) {
    static unsigned int poolSize = 0;
    static unsigned int poolId = 0;
    for (int c = 0; c < indentLevel; c++) {
        stream << "    ";
    }
    stream << value << endl;

    poolSize += 4;
    if (poolSize == 4096) {
        poolSize= 0;
        stream << "b pool_" << poolId << endl;
        stream << ".ltorg" << endl;
        stream << "pool_" << poolId++ << ":" << endl;
    }
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


bool CodeGenerator::canImmediateValue(const long long int &value) {
    return value < 256 && value >= -256;
}


bool CodeGenerator::powerOfTwo(const long long &value) {
    if (value != 0) {
        return ((value & (value - 1)) == 0);
    }
    return false;
}

unsigned int CodeGenerator::logTwo(const long long &value) {
    unsigned int result = 0;
    while (((1 << result) & value) == 0) {
        result++;
    }
    return result;
}
