//
// Created by Ran on 2/26/2016.
//

#include <iostream>
#include <memory>

#include "gtest/gtest.h"
#include "Scope.h"
#include "Constant.h"

using std::shared_ptr;

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

    inner->addEntry("x", shared_ptr<Constant>(new Constant(5)));
    outer->addEntry("y", shared_ptr<Constant>(new Constant(7)));

    ASSERT_TRUE(outer->scopeContainsEntry("y"));
    ASSERT_TRUE(inner->scopeContainsEntry("x"));
    ASSERT_TRUE(inner->containsEntry("y"));
    ASSERT_FALSE(inner->scopeContainsEntry("y"));

}