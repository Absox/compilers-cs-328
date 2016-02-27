//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_SCOPE_H
#define COMPILERS_HWK4_SCOPE_H

#include <memory>
#include <unordered_map>
#include <string>

#include "Entry.h"

class Scope {
public:
    Scope();
    virtual ~Scope();

    bool scopeContainsEntry(const std::string& identifier) const;
    bool containsEntry(const std::string& identifier) const;

    void addEntry(const std::string& identifier, const std::shared_ptr<Entry>& entry);
    std::shared_ptr<Entry> getEntry(const std::string& identifier) const;

    std::shared_ptr<Scope> getOuter() const;
    void setOuter(const std::shared_ptr<Scope> outer);

    static Scope* createUniverse();

private:
    std::shared_ptr<Scope> outer;
    std::unordered_map<std::string, std::shared_ptr<Entry>> identifiers;
};


#endif //COMPILERS_HWK4_SCOPE_H
