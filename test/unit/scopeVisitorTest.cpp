//
// Created by Ran on 2/26/2016.
//

#include <memory>
#include <iostream>

#include "gtest/gtest.h"
#include "ScopeVisitor.h"
#include "Scope.h"
#include "Type.h"

using std::shared_ptr;
using std::cout;
using std::endl;

TEST(scopeVisitorTest, testVisitBasicScope) {
    shared_ptr<Type> integer = shared_ptr<Type>(new Type("INTEGER"));

    Scope current;
    ScopeVisitor visitor;

    current.acceptVisitor(visitor);

    cout << endl;

    cout << visitor.getContent() << endl;

}
