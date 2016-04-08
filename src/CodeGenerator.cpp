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
        : symbolTable(symbolTable) {
    indentLevel = 0;
    labelCounter = 0;

    calculateOffsets();
    initializeProgram();
    processInstructions(instructions);
    finalizeProgram();
}

string CodeGenerator::getContent() {
    return stream.str();
}

void CodeGenerator::calculateOffsets() {
    // First, register the universal integer as 4 bytes.
    auto universalInt = dynamic_pointer_cast<Type>(
            symbolTable.getCurrentScope()->getOuter()->getEntry("INTEGER"));
    typeSizes[universalInt] = 4;

    totalBytes = calculateScopeOffsets(symbolTable.getCurrentScope());

}

int CodeGenerator::calculateScopeOffsets(const shared_ptr<Scope> &scope) {
    int nextOffset = 0;

    vector<string> identifiers = scope->getIdentifiersSorted();
    for (unsigned c = 0; c < identifiers.size(); c++) {
        auto variable = dynamic_pointer_cast<Variable>(
                scope->getEntryInScope(identifiers[c]));
        if (variable != 0) {

            variable->setOffset(nextOffset);
            int size = getTypeSize(variable->getType());
            nextOffset += size;
        }
    }

    return nextOffset;
}


int CodeGenerator::getTypeSize(const shared_ptr<Type> &type) {

    // If found in the hashmap, store its size.
    if (typeSizes.find(type) != typeSizes.end()) {
        return typeSizes[type];
    }

    // Otherwise, we're dealing with an array or a record.
    auto array = dynamic_pointer_cast<Array>(type);
    auto record = dynamic_pointer_cast<Record>(type);

    if (array != 0) {
        int result = array->getLength() * getTypeSize(array->getType());
        typeSizes[array] = result;
        return result;
    } else {
        int result = calculateScopeOffsets(record->getScope());
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
    writeWithIndent("mov fp, sp");
    if (totalBytes > 0) writeWithIndent("ldr r10, =variables");

    writeWithIndent("");
}


void CodeGenerator::finalizeProgram() {
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
    writeWithIndent("mov sp, fp");
    writeWithIndent("pop {fp, lr}");
    writeWithIndent("bx lr");

    deindent();
    writeWithIndent("standard_error:");
    indent();
    writeWithIndent(".word stderr");
    deindent();
}


void CodeGenerator::processInstructions(
        const vector<shared_ptr<Instruction>> &instructions) {
    for (int c = 0; c < instructions.size(); c++) {
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


void CodeGenerator::processAssign(const shared_ptr<Assign> &assign) {

    // We need to resolve our target offset.
    resolveLocationOffset(assign->getLocation());

    // We need to know if we're assigning integers, arrays, or records.
    auto type = getLocationType(assign->getLocation());
    auto record = dynamic_pointer_cast<Record>(type);
    auto array = dynamic_pointer_cast<Array>(type);

    if (record != 0 || array != 0) {
        int numBytesToMove = typeSizes[type];
        auto origin_location = dynamic_pointer_cast<Location>(
                assign->getExpression());

        resolveLocationOffset(origin_location);

        // Pop the origin offset.
        writeWithIndent("pop {r0}");
        // Calculate origin address.
        writeWithIndent("add r0, r0, r10");
        // Pop the destination offset.
        writeWithIndent("pop {r1}");
        writeWithIndent("add r1, r1, r10");

        for (int currentOffset = 0;
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
        writeWithIndent("add r2, r1, r10");
        writeWithIndent("str r0, [r2]");
    }

}

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

    // Pop offset of location we're reading to from stack.
    resolveLocationOffset(read->getLocation());
    writeWithIndent("pop {r3}");

    // Address goes into r1
    // Calculate address
    writeWithIndent("add r1, r3, r10");

    // Format goes into r0
    writeWithIndent("ldr r0, =input_format");
    // bl scanf
    writeWithIndent("bl __isoc99_scanf");
}

void CodeGenerator::processWrite(const shared_ptr<Write> &write) {

    resolveExpressionValue(write->getExpression());
    // Pop value we're writing from stack.
    // value goes into r1
    writeWithIndent("pop {r1}");
    // format goes into r0
    writeWithIndent("ldr r0, =output_format");
    // bl printf
    writeWithIndent("bl printf");
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


void CodeGenerator::resolveLocationOffset(
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
        int elementSize = typeSizes[array->getType()];

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

void CodeGenerator::resolveExpressionValue(
        const shared_ptr<Expression> &expression) {
    // Ultimately pushes a numerical value onto the stack.

    auto number = dynamic_pointer_cast<NumberExpression>(expression);
    auto location = dynamic_pointer_cast<Location>(expression);
    auto binary = dynamic_pointer_cast<BinaryExpression>(expression);

    if (number != 0) {
        // Load the value into r3
        writeWithIndent("ldr r3, =" + to_string(number->getValue()));
        writeWithIndent("push {r3}");

    } else if (location != 0) {

        // We need to resolve the offset of the location first.
        resolveLocationOffset(location);
        // Pop the offset into r3.
        writeWithIndent("pop {r3}");
        // Calculate the address.
        writeWithIndent("add r0, r10, r3");
        // Load the value from the address.
        writeWithIndent("ldr r1, [r0]");
        // Push onto the stack.
        writeWithIndent("push {r1}");

    } else if (binary != 0) {

        // We need to pop two numbers, then do the operation, then push result.
        // Push right
        resolveExpressionValue(binary->getExpressionRight());
        // Push left
        resolveExpressionValue(binary->getExpressionLeft());

        // Pop left
        writeWithIndent("pop {r0}");
        // Pop right
        writeWithIndent("pop {r1}");

        if (binary->getOperation() == "+") {
            writeWithIndent("add r3, r0, r1");
        } else if (binary->getOperation() == "-") {
            writeWithIndent("sub r3, r0, r1");
        } else if (binary->getOperation() == "*") {
            writeWithIndent("mul r3, r0, r1");
        } else if (binary->getOperation() == "DIV") {
            writeWithIndent("cmp r1, #0");
            writeWithIndent("beq div_zero");

            writeWithIndent("bl __aeabi_idiv");
            writeWithIndent("mov r3, r0");
        } else if (binary->getOperation() == "MOD") {
            writeWithIndent("cmp r1, #0");
            writeWithIndent("beq mod_zero");

            writeWithIndent("bl __aeabi_idivmod");
            writeWithIndent("mov r3, r1");
        }
        writeWithIndent("push {r3}");

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

