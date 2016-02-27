/* 
 * Driver for simple compiler.
 * 
 * File:   main.cpp
 * Author: Ran Liu
 * rliu14@jhu.edu
 *
 * Created on February 20, 2016, 10:56 AM
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Scanner.h"
#include "Parser.h"
#include "Token.h"
#include "TextObserver.h"
#include "GraphObserver.h"

using std::ifstream;
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;

bool fileExists(const char* filename) {
    ifstream file(filename);
    if (file.good()) {
        file.close();
        return true;
    } else {
        file.close();
        return false;
    }
}

string &readFileContents(const char* filename, string& target) {
    ifstream file(filename);

    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);

    target = string(length, ' ');
    file.read(&target[0], length);

    return target;
}

string &readFromConsole(string& target) {
    target = string("");
    char buffer;

    while (!cin.eof()) {
        cin.get(buffer);
        target += buffer;
    }

    return target;
}

int main(int argc, char **argv) {

    const int NO_OPTIONS = 0;
    const int INVALID_OPTIONS = 1;
    const int NONEXISTENT_FILE = 2;
    
    try {
        
        char* filename = 0;
        string content;
        
        if (argc < 2) throw NO_OPTIONS;
        
        char* option = argv[1];

        // TODO refactor driver code to be more DRY

        if (option[0] == '-') {
            if (option[1] == 's') {
                
                if (argc > 2) {
                    filename = argv[2];
                }
                
                if (filename != 0) {
                    if (fileExists(filename)) {
                        readFileContents(filename, content);
                    } else {
                        throw NONEXISTENT_FILE;
                    }
                } else {
                    readFromConsole(content);
                }
                
                Scanner scanner(content);
                vector<Token> tokens = scanner.all();
                
                for (unsigned int c = 0; c < tokens.size(); c++) {
                    cout << tokens[c] << endl;
                }
                
            } else if (option[1] == 'c') {
                
                bool graphical = false;
                
                if (argc > 2) {
                    option = argv[2];
                    if (option[0] == '-' && option[1] == 'g') {
                        graphical = true;
                        
                        if (argc > 3) {
                            filename = argv[3];
                        }
                        
                    } else {
                        filename = argv[2];
                    }
                    
                }
                
                if (filename != 0) {
                    if (fileExists(filename)) {
                        readFileContents(filename, content);
                    } else {
                        throw NONEXISTENT_FILE;
                    }
                } else {
                    readFromConsole(content);
                }

                Scanner scanner(content);
                Parser parser(&scanner);
                if (graphical) {
                    GraphObserver observer;
                    parser.addObserver(&observer);
                    
                    try {
                        parser.parse();
                        cout << observer.getContent();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                } else {
                    TextObserver observer;
                    parser.addObserver(&observer);
                    
                    try {
                        parser.parse();
                        cout << observer.getContent();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                }
                
            } else if (option[1] == 't') {

                bool graphical = false;
                if (argc > 2) {
                    option = argv[2];
                    if (option[0] == '-' && option[1] == 'g') {
                        graphical = true;

                        if (argc > 3) {
                            filename = argv[3];
                        }

                    } else {
                        filename = argv[2];
                    }

                }

                if (filename != 0) {
                    if (fileExists(filename)) {
                        readFileContents(filename, content);
                    } else {
                        throw NONEXISTENT_FILE;
                    }
                } else {
                    readFromConsole(content);
                }

                Scanner scanner(content);
                Parser parser(&scanner, false);

                if (graphical) {
                    try {
                        parser.parse();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                } else {
                    try {
                        parser.parse();

                        ScopeVisitor textVisitor;
                        parser.getSymbolTable().getCurrentScope()
                                ->acceptVisitor(textVisitor);
                        cout << textVisitor.getContent();

                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                }

            } else {
                throw INVALID_OPTIONS;
            }
        } else {
            throw NO_OPTIONS;
        }
        
        
        
    } catch (int& e) {
        switch(e) {
            case NO_OPTIONS:
                cerr << "error: no options specified" << endl;
                break;
            case INVALID_OPTIONS:
                cerr << "error: invalid option" << endl;
                break;
            case NONEXISTENT_FILE:
                cerr << "error: file does not exist" << endl;
        }
    }

    return 0;
}