//
// Created by Ran on 2/26/2016.
// rliu14@jhu.edu

#ifndef COMPILERS_HWK4_SCOPE_H
#define COMPILERS_HWK4_SCOPE_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "Entry.h"
#include "Box.h"
#include "Type.h"

class Scope {
public:
    Scope(const std::shared_ptr<Scope>& outer = 0);
    virtual ~Scope();

    bool scopeContainsEntry(const std::string& identifier) const;
    bool containsEntry(const std::string& identifier) const;

    void addEntry(const std::string& identifier,
                  const std::shared_ptr<Entry>& entry);
    std::shared_ptr<Entry> getEntry(const std::string& identifier) const;
    std::shared_ptr<Entry> getEntryInScope(const std::string& identifier) const;

    std::shared_ptr<Scope> getOuter() const;
    void setOuter(const std::shared_ptr<Scope>& outer);

    void acceptVisitor(ScopeVisitor& visitor);
    std::vector<std::string> getIdentifiersSorted();

    std::unordered_map<std::string, std::shared_ptr<Box>> buildEnvironment();

    static Scope* createUniverse();
    static bool wayToSort(std::string a, std::string b);
private:
    std::shared_ptr<Scope> outer;
    std::unordered_map<std::string, std::shared_ptr<Entry>> identifiers;
};


#endif //COMPILERS_HWK4_SCOPE_H
