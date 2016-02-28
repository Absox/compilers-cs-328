//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_CONSTNAT_H
#define COMPILERS_HWK4_CONSTNAT_H


#include "Entry.h"
#include "Type.h"

#include <string>
#include <memory>

class Constant : public Entry {
public:
    Constant(const int& value, const std::shared_ptr<Type>& type);
    virtual ~Constant();

    void setValue(const int& value);
    virtual std::string getEntryType() const override;
    virtual void acceptVisitor(ScopeVisitor& visitor);
    int getValue();
    std::shared_ptr<Type> getType() const;

private:
    int value;
    std::shared_ptr<Type> type;
};


#endif //COMPILERS_HWK4_CONSTNAT_H
