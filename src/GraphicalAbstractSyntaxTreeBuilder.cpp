//
// Created by Ran on 3/15/2016.
//

#include "GraphicalAbstractSyntaxTreeBuilder.h"

using std::vector;
using std::shared_ptr;
using std::string;
using std::dynamic_pointer_cast;
using std::endl;

GraphicalAbstractSyntaxTreeBuilder::GraphicalAbstractSyntaxTreeBuilder(
        const vector<shared_ptr<Instruction>> instructions) {
    nodeCounter = 1;

    stream << "digraph Program {" << endl;
    if (instructions.size() > 0)
        processInstructions(instructions);
    stream << "}" << endl;
}

GraphicalAbstractSyntaxTreeBuilder::~GraphicalAbstractSyntaxTreeBuilder() {
}

string GraphicalAbstractSyntaxTreeBuilder::getContent() {
    return stream.str();
}

// Returns id of first instruction.
unsigned int GraphicalAbstractSyntaxTreeBuilder::processInstructions(
        const vector<shared_ptr<Instruction>> &instructions) {

    vector<unsigned int> ids;
    for (unsigned int c = 0; c < instructions.size(); c++) {
        ids.push_back(processInstruction(instructions[c]));
    }

    if (ids.size() > 1) {
        stream << "{rank=same;" << endl;
        for (unsigned int c = 1; c < ids.size(); c++) {
            stream << ids[c-1] << " -> " << ids[c] << " [label=next]" << endl;
        }
        stream << "}" << endl;
    }

    if (instructions.size() > 0) {
        return ids[0];
    } else {
        return 0;
    }
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processInstruction(
        const std::shared_ptr<Instruction> &instruction) {
    auto assign = dynamic_pointer_cast<Assign>(instruction);
    auto ifInstruction = dynamic_pointer_cast<IfInstruction>(instruction);
    auto repeat = dynamic_pointer_cast<Repeat>(instruction);
    auto read = dynamic_pointer_cast<Read>(instruction);
    auto write = dynamic_pointer_cast<Write>(instruction);

    if (assign != 0) {
        return processAssign(assign);
    } else if (ifInstruction != 0) {
        return processIf(ifInstruction);
    } else if (repeat != 0) {
        return processRepeat(repeat);
    } else if (read != 0) {
        return processRead(read);
    } else {
        return processWrite(write);
    }
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processAssign(
        const std::shared_ptr<Assign> &assign) {
    unsigned int locationNode = processLocation(assign->getLocation());
    unsigned int expressionNode = processExpression(assign->getExpression());
    unsigned int assignNode = getNextNodeId();

    stream << assignNode << " [label=\":=\", shape=box];" << endl;
    stream << assignNode << " -> " << locationNode << " [label=location];" <<
            endl;
    stream << assignNode << " -> " << expressionNode <<
            " [label=expression];" << endl;

    return assignNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processIf(
        const std::shared_ptr<IfInstruction> &ifInstruction) {
    unsigned int conditionNode = processCondition(ifInstruction->getCondition());
    unsigned int instructionsTrueNode = processInstructions(
            ifInstruction->getInstructionsTrue());
    unsigned int ifNode = getNextNodeId();

    stream << ifNode << " [label=\"If\", shape=box];" << endl;
    stream << ifNode << " -> " << conditionNode <<
            " [label=condition];" << endl;
    stream << ifNode << " -> " << instructionsTrueNode <<
            " [label=true];" << endl;

    if (ifInstruction->getInstructionsFalse().size() > 0) {
        unsigned int instructionsFalseNode =
                processInstructions(ifInstruction->getInstructionsFalse());
        stream << ifNode << " -> " << instructionsFalseNode <<
                " [label=false];" << endl;
    }

    return ifNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processRepeat(
        const std::shared_ptr<Repeat> &repeat) {
    unsigned int conditionNode = processCondition(repeat->getCondition());
    unsigned int instructionsNode = processInstructions(
            repeat->getInstructions());
    unsigned int repeatNode = getNextNodeId();

    stream << repeatNode << " [label=\"Repeat\", shape=box];" << endl;
    stream << repeatNode << " -> " << conditionNode <<
            " [label=condition];" << endl;
    stream << repeatNode << " -> " << instructionsNode <<
            " [label=instructions];" << endl;

    return repeatNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processRead(
        const std::shared_ptr<Read> &read) {
    unsigned int locationNode = processLocation(read->getLocation());
    unsigned int readNode = getNextNodeId();

    stream << readNode << " [label=\"Read\", shape=box];" << endl;
    stream << readNode << " -> " << locationNode <<
            " [label=location];" << endl;
    return readNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processWrite(
        const std::shared_ptr<Write> &write) {
    unsigned int expressionNode = processExpression(write->getExpression());
    unsigned int writeNode = getNextNodeId();

    stream << writeNode << " [label=\"Write\", shape=box];" << endl;
    stream << writeNode << " -> " << expressionNode <<
            " [label=expression];" << endl;

    return writeNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processLocation(
        const std::shared_ptr<Location> &location) {
    auto variable = dynamic_pointer_cast<VariableLocation>(location);
    auto index = dynamic_pointer_cast<Index>(location);
    auto field = dynamic_pointer_cast<Field>(location);

    if (variable != 0) {
        return processVariable(variable);
    } else if (index != 0) {
        return processIndex(index);
    } else {
        return processField(field);
    }
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processVariable(
        const std::shared_ptr<VariableLocation> &variable) {
    unsigned int variableNode = getNextNodeId();
    unsigned int symbolTableVarNode = getNextNodeId();

    stream << variableNode << " [label=\"Variable\", shape=box];" << endl;
    stream << symbolTableVarNode << " [label=\"" <<
            variable->getIdentifier() << "\", shape=circle];" << endl;
    stream << variableNode << " -> " << symbolTableVarNode <<
            " [label=ST];" << endl;

    return variableNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processIndex(
        const std::shared_ptr<Index> &index) {
    unsigned int locationNode = processLocation(index->getLocation());
    unsigned int expressionNode = processExpression(index->getExpression());
    unsigned int indexNode = getNextNodeId();

    stream << indexNode << " [label=\"Index\", shape=box];" << endl;
    stream << indexNode << " -> " << locationNode <<
            " [label=location];" << endl;
    stream << indexNode << " -> " << expressionNode <<
            " [label=expression];" << endl;

    return indexNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processField(
        const std::shared_ptr<Field> &field) {
    unsigned int variableNode = processVariable(field->getVariable());
    unsigned int locationNode = processLocation(field->getLocation());
    unsigned int fieldNode = getNextNodeId();

    stream << fieldNode << " [label=\"Field\", shape=box];" << endl;
    stream << fieldNode << " -> " << locationNode <<
            " [label=location];" << endl;
    stream << fieldNode << " -> " << variableNode <<
            " [label=variable];" << endl;

    return fieldNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processCondition(
        const std::shared_ptr<Condition> &condition) {
    unsigned int leftNode = processExpression(condition->getExpressionLeft());
    unsigned int rightNode = processExpression(condition->getExpressionRight());
    unsigned int conditionNode = getNextNodeId();

    stream << conditionNode << " [label=\"" << condition->getLabel() <<
            "\", shape=box];" << endl;
    stream << conditionNode << " -> " << leftNode << " [label=left];" << endl;
    stream << conditionNode << " -> " << rightNode << " [label=right];" << endl;

    return conditionNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processExpression(
        const std::shared_ptr<Expression> &expression) {
    auto number = dynamic_pointer_cast<NumberExpression>(expression);
    auto location = dynamic_pointer_cast<Location>(expression);
    auto binary = dynamic_pointer_cast<BinaryExpression>(expression);

    if (number != 0) {
        return processNumber(number);
    } else if (location != 0) {
        return processLocation(location);
    } else {
        return processBinary(binary);
    }
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processNumber(
        const std::shared_ptr<NumberExpression> &number) {
    unsigned int numberNode = getNextNodeId();
    unsigned int symbolTableConstNode = getNextNodeId();

    stream << numberNode << " [label=\"Number\", shape=box];" << endl;
    stream << symbolTableConstNode << " [label=\"" << number->getValue() <<
            "\", shape=diamond];" << endl;
    stream << numberNode << " -> " << symbolTableConstNode <<
            " [label=ST];" << endl;

    return numberNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::processBinary(
        const std::shared_ptr<BinaryExpression> &binary) {
    unsigned int leftNode = processExpression(binary->getExpressionLeft());
    unsigned int rightNode = processExpression(binary->getExpressionRight());
    unsigned int binaryNode = getNextNodeId();

    stream << binaryNode << " [label=\"" << binary->getOperation() <<
            "\", shape=box];" << endl;
    stream << binaryNode << " -> " << leftNode << " [label=left];" << endl;
    stream << binaryNode << " -> " << rightNode << " [label=right];" << endl;

    return binaryNode;
}

unsigned int GraphicalAbstractSyntaxTreeBuilder::getNextNodeId() {
    return nodeCounter++;
}