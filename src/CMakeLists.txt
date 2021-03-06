project(compilers5_lib)
add_definitions(-std=c++11 -Wall -Wextra -pedantic)

set(SOURCE_FILES
        Array.cpp
        Constant.cpp
        Entry.cpp
        GraphNode.cpp
        GraphObserver.cpp
        Observer.cpp
        ParseException.cpp
        Parser.cpp
        ParseState.cpp
        Record.cpp
        Scanner.cpp
        ScannerException.cpp
        Scope.cpp
        TextObserver.cpp
        Token.cpp
        Type.cpp
        Variable.cpp
        SymbolTable.cpp
        ScopeVisitor.cpp
        GraphicalSymbolTree.cpp
        AbstractSyntaxTreeNode.cpp
        Instruction.cpp
        Expression.cpp
        Condition.cpp
        Assign.cpp
        IfInstruction.cpp
        Repeat.cpp
        Location.cpp
        Read.cpp
        Write.cpp
        NumberExpression.cpp
        BinaryExpression.cpp
        Index.cpp
        VariableLocation.cpp
        Field.cpp
        AbstractSyntaxTreeBuilder.cpp
        GraphicalAbstractSyntaxTreeBuilder.cpp
        Interpreter.cpp
        Environment.cpp
        Box.cpp
        IntegerBox.cpp
        ArrayBox.cpp
        RecordBox.cpp
        RuntimeException.cpp
        CodeGenerator.cpp
        CodeGenerationException.cpp
        CodeGenerationMessage.cpp
        BasicCodeGenerator.cpp
        Procedure.cpp
        Formal.cpp
        LocalVariable.cpp
        Call.cpp
        CallExpression.cpp
        ParameterVariable.cpp)

set(HEADER_FILES
        Array.h
        Constant.h
        Entry.h
        GraphNode.h
        GraphObserver.h
        Observer.h
        ParseException.h
        Parser.h
        ParseState.h
        Record.h
        Scanner.h
        ScannerException.h
        Scope.h
        TextObserver.h
        Token.h
        Type.h
        Variable.h
        SymbolTable.h
        ScopeVisitor.h
        GraphicalSymbolTree.h
        AbstractSyntaxTreeNode.h
        Instruction.h
        Expression.h
        Condition.h
        Assign.h
        IfInstruction.h
        Repeat.h
        Location.h
        Read.h
        Write.h
        NumberExpression.h
        BinaryExpression.h
        Index.h
        VariableLocation.h
        Field.h
        AbstractSyntaxTreeBuilder.h
        GraphicalAbstractSyntaxTreeBuilder.h
        Interpreter.h
        Environment.h
        Box.h
        IntegerBox.h
        ArrayBox.h
        RecordBox.h
        RuntimeException.h
        CodeGenerator.h
        CodeGenerationException.h
        CodeGenerationMessage.h
        BasicCodeGenerator.h
        Procedure.h
        Formal.h
        LocalVariable.h
        Call.h
        CallExpression.h
        ParameterVariable.h)

add_library(compilers5_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})