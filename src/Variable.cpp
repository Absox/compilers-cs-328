//
// Created by Ran on 2/26/2016.
//

#include "Variable.h"

using std::string;

Variable::Variable() {

}

Variable::~Variable() {

}

string Variable::getEntryType() const {
    return "VARIABLE";
}

void Variable::acceptVisitor(ScopeVisitor &visitor) {

}
