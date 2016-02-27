//
// Created by Ran on 2/26/2016.
//

#ifndef COMPILERS_HWK4_CONSTNAT_H
#define COMPILERS_HWK4_CONSTNAT_H


#include "Entry.h"

class Constant : public Entry {
public:
    Constant(const int& value);
    virtual ~Constant();

    void setValue(const int& value);
    virtual std::string getEntryType() const override;
    int getValue();

private:
    int value;
};


#endif //COMPILERS_HWK4_CONSTNAT_H
