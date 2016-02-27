/*
 * File:   ScannerException.h
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 11:13 AM
 */

#ifndef SCANNEREXCEPTION_H
#define SCANNEREXCEPTION_H

#include <string>

class ScannerException {
public:
    ScannerException(const unsigned int& position, const std::string& message);
    virtual ~ScannerException();
    
    unsigned int getPosition() const;
    std::string getMessage() const;
private:
    unsigned int position;
    std::string message;
};

#endif /* SCANNEREXCEPTION_H */

