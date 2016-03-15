//
// Created by Ran on 3/14/2016.
//

#include "Index.h"

using std::shared_ptr;

Index::Index(const shared_ptr<Location>& location,
             const shared_ptr<Expression>& expression) {
    this->location = location;
    this->expression = expression;
}

Index::~Index() {

}

std::string Index::getLabel() {
    return "Index";
}
