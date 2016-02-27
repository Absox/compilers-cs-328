//
// Created by Ran on 2/26/2016.
//

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "gtest/gtest.h"
#include "Scope.h"
#include "Constant.h"

using std::shared_ptr;
using std::string;
using std::vector;

using std::cout;
using std::endl;

TEST(scopeTest, init) {
    Scope scope;
    ASSERT_TRUE(scope.getOuter() == 0);
    ASSERT_TRUE(scope.containsEntry("x") == false);
}

TEST(scopeTest, innerOuterScopeIdentifierTest) {

    shared_ptr<Scope> outer = shared_ptr<Scope>(new Scope);
    shared_ptr<Scope> inner = shared_ptr<Scope>(new Scope);
    inner->setOuter(outer);
    shared_ptr<Type> integer = shared_ptr<Type>(new Type("INTEGER"));

    inner->addEntry("x", shared_ptr<Constant>(new Constant(5, integer)));
    outer->addEntry("y", shared_ptr<Constant>(new Constant(7, integer)));

    ASSERT_TRUE(outer->scopeContainsEntry("y"));
    ASSERT_TRUE(inner->scopeContainsEntry("x"));
    ASSERT_TRUE(inner->containsEntry("y"));
    ASSERT_FALSE(inner->scopeContainsEntry("y"));

}

TEST(scopeTest, sortedIdentifiersTest) {
    Scope scope;
    scope.addEntry("Aa", 0);
    scope.addEntry("a", 0);
    scope.addEntry("asdf", 0);
    scope.addEntry("Hello", 0);
    scope.addEntry("b", 0);
    scope.addEntry("c", 0);
    scope.addEntry("D", 0);
    scope.addEntry("E", 0);

    vector<string> keys = scope.getIdentifiersSorted();
    ASSERT_TRUE(keys[0] == "Aa");
    ASSERT_TRUE(keys[1] == "D");
    ASSERT_TRUE(keys[2] == "E");
    ASSERT_TRUE(keys[3] == "Hello");
    ASSERT_TRUE(keys[4] == "a");
    ASSERT_TRUE(keys[5] == "asdf");
    ASSERT_TRUE(keys[6] == "b");
    ASSERT_TRUE(keys[7] == "c");

}

TEST(scopeTest, castingSmartPtrTest) {

    shared_ptr<Entry> entry = shared_ptr<Type>(new Type("INTEGER"));
    shared_ptr<Type> integer = std::dynamic_pointer_cast<Type>(entry);
    ASSERT_TRUE(integer->getName() == "INTEGER");

}