//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK4_TYPE_H
#define COMPILERS_HWK4_TYPE_H

#include "Entry.h"

#include <string>

class Type : public Entry {
public:
    Type(const std::string& name);
    virtual ~Type();

    std::string getName() const;
    virtual std::string getEntryType() const override;
    virtual void acceptVisitor(ScopeVisitor& visitor) override;
private:
    std::string name;
};


#endif //COMPILERS_HWK4_TYPE_H
