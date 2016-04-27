//
// Created by ran on 4/25/16.
//

#include "Procedure.h"


Procedure::Procedure() {

}


Procedure::~Procedure() {

}


std::string Procedure::getEntryType() const {
    return "PROCEDURE";
}


void Procedure::acceptVisitor(ScopeVisitor &visitor) {
    visitor.writeWithIndent("PROCEDURE BEGIN");
    visitor.indent();
    visitor.deindent();
    visitor.writeWithIndent("PROCEDURE END");
}