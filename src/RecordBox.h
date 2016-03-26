//
// Created by ran on 3/26/16.
//

#ifndef COMPILERS_HWK5_RECORDBOX_H
#define COMPILERS_HWK5_RECORDBOX_H

#include <unordered_map>
#include <string>
#include <memory>

#include "Box.h"

class RecordBox : public Box {
public:
    RecordBox();
    virtual ~RecordBox();


private:
    std::unordered_map<std::string, std::shared_ptr<Box>> entries;

};


#endif //COMPILERS_HWK5_RECORDBOX_H
