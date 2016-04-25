//
// Created by ran on 4/25/16.
//

#ifndef COMPILERS_HWK5_PROCEDURE_H
#define COMPILERS_HWK5_PROCEDURE_H


#include "Entry.h"
#include "Scope.h"

class Procedure : public Entry {
public:
    Procedure();

    virtual void acceptVisitor(ScopeVisitor &visitor) override;
    virtual std::string getEntryType() const override;

private:
    std::shared_ptr<Scope> scope;
};


#endif //COMPILERS_HWK5_PROCEDURE_H
