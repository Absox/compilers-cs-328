//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK4_RECORD_H
#define COMPILERS_HWK4_RECORD_H

#include <memory>

#include "Scope.h"
#include "Type.h"

class Record : public Type {
public:
    Record(const std::shared_ptr<Scope>& scope);
    virtual ~Record();

    std::shared_ptr<Scope> getScope() const;
    void setScope(const std::shared_ptr<Scope>& scope);

    virtual void acceptVisitor(ScopeVisitor& visitor) override;
    virtual std::shared_ptr<Box> initializeBox() override;

private:
    std::shared_ptr<Scope> scope;
};


#endif //COMPILERS_HWK4_RECORD_H
