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
    RecordBox(
        const std::unordered_map<std::string, std::shared_ptr<Box>>& entries);
    virtual ~RecordBox();

    void assign(const std::shared_ptr<RecordBox>& other);
    virtual std::shared_ptr<Box> copy() override;

private:
    std::unordered_map<std::string, std::shared_ptr<Box>> entries;

};


#endif //COMPILERS_HWK5_RECORDBOX_H
