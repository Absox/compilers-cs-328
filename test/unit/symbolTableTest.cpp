//
// Created by Ran on 2/26/2016.
//

#include <memory>

#include "SymbolTable.h"
#include "gtest/gtest.h"

using std::shared_ptr;

TEST(symbolTableTest, init) {
    SymbolTable symbolTable;
    shared_ptr<Scope> scope = symbolTable.getCurrentScope();
    ASSERT_TRUE(scope->containsEntry("INTEGER"));

}
