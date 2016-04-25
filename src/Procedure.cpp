//
// Created by ran on 4/25/16.
//

#include "Procedure.h"


Procedure::Procedure() {

}


std::string Procedure::getEntryType() const {
    return "PROCEDURE";
}


void Procedure::acceptVisitor(ScopeVisitor &visitor) {

}

