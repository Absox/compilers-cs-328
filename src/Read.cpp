//
// Created by Ran on 3/14/2016.
// rliu14@jhu.edu

#include "Read.h"

using std::shared_ptr;

Read::Read(const shared_ptr<Location>& location) {
    this->location = location;
}

Read::~Read() {

}

std::string Read::getLabel() {
    return "Read";
}
