//
// Created by Ran on 3/14/2016.
//

#ifndef COMPILERS_HWK5_INDEX_H
#define COMPILERS_HWK5_INDEX_H

#include <memory>

#include "Location.h"

class Index : public Location {
public:
    Index();
    virtual ~Index();
    virtual std::string getType();
private:
    std::shared_ptr<Location> location;
    std::shared_ptr<Expression> expression;

};


#endif //COMPILERS_HWK5_INDEX_H
