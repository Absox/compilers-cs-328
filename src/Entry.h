//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_ENTRY_H
#define COMPILERS_HWK4_ENTRY_H

#include <string>

class Entry {
public:
    Entry();
    virtual ~Entry();
    virtual std::string getEntryType() const = 0;
private:
};


#endif //COMPILERS_HWK4_ENTRY_H
