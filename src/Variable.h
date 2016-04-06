//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK4_VARIABLE_H
#define COMPILERS_HWK4_VARIABLE_H

#include <memory>

#include "Type.h"

class Variable : public Entry {
public:

    Variable(const std::shared_ptr<Type>& type);
    virtual ~Variable();

    virtual std::string getEntryType() const override;
    virtual void acceptVisitor(ScopeVisitor& visitor) override;
    std::shared_ptr<Type> getType() const;

private:
    std::shared_ptr<Type> type;
    int offset;
};


#endif //COMPILERS_HWK4_VARIABLE_H
