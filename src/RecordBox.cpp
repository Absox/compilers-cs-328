//
// Created by ran on 3/26/16.
//

#include "RecordBox.h"

using std::string;
using std::shared_ptr;
using std::unordered_map;

RecordBox::RecordBox(
        const unordered_map <string, shared_ptr<Box>>& entries) {
    this->entries = entries;
}

RecordBox::~RecordBox() {

}


void RecordBox::assign(const shared_ptr<RecordBox> &other) {
    unordered_map<string, shared_ptr<Box>> newEntries;
    for (auto iterator = other->entries.begin(); iterator != other->entries.end(); ++iterator) {
        newEntries[iterator->first] = iterator->second->copy();
    }
    entries = newEntries;
}


shared_ptr<Box> RecordBox::copy() {
    unordered_map<string, shared_ptr<Box>> newEntries;
    for (auto iterator = entries.begin(); iterator != entries.end(); ++iterator) {
        newEntries[iterator->first] = iterator->second->copy();
    }
    shared_ptr<RecordBox> result(new RecordBox(newEntries));
    return result;
}

