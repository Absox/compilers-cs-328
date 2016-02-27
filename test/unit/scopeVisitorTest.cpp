//
// Created by Ran on 2/26/2016.
//

#include <memory>
#include <iostream>
#include <Constant.h>
#include <Record.h>

#include "gtest/gtest.h"
#include "ScopeVisitor.h"
#include "Scope.h"
#include "Type.h"

using std::shared_ptr;
using std::cout;
using std::endl;

TEST(scopeVisitorTest, testVisitBasicScope) {
    shared_ptr<Type> integer = shared_ptr<Type>(new Type("INTEGER"));
    shared_ptr<Scope> universe(new Scope);
    universe->addEntry("INTEGER", integer);

    shared_ptr<Scope> current(new Scope(universe));

    shared_ptr<Entry> x(new Constant(5, integer));

    shared_ptr<Scope> y_scope(new Scope(current));
    shared_ptr<Entry> y(new Record(y_scope));

    current->addEntry("x", x);
    current->addEntry("y", y);

    shared_ptr<Entry> x_inner(new Constant(7, integer));

    y_scope->addEntry("x", x_inner);

    ScopeVisitor visitor;
    current->acceptVisitor(visitor);

    cout << endl;
    cout << visitor.getContent() << endl;

    ASSERT_TRUE(current->getEntry("INTEGER") == universe->getEntry("INTEGER"));
    ASSERT_TRUE(current->getEntry("INTEGER") == integer);

}
