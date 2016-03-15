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
#include <memory>
#include <vector>
#include <GraphicalSymbolTree.h>
#include <AbstractSyntaxTreeBuilder.h>

#include "Scanner.h"
#include "Parser.h"
#include "TextObserver.h"
#include "GraphObserver.h"

using std::ifstream;
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

bool fileExists(const char* filename) {
    ifstream file(filename);
    file.close();
    return file.good();
}

string &readFileContents(const char* filename, string& target) {
    ifstream file(filename);

    file.seekg(0, file.end);
    long length = file.tellg();
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

    const int SCANNER = 3;
    const int PARSER = 4;
    const int SYMBOL_TABLE = 5;
    const int ABSTRACT_SYNTAX_TREE = 6;

    try {

        int option = 0;
        char* filename = 0;
        string content;
        bool graphical = false;

        if (argc < 2) throw NO_OPTIONS;

        if (argv[1][0] == '-') {
            if (argv[1][1] == 's') {
                option = SCANNER;
            } else if (argv[1][1] == 'c') {
                option = PARSER;
            } else if (argv[1][1] == 't') {
                option = SYMBOL_TABLE;
            } else if (argv[1][1] == 'a') {
                option = ABSTRACT_SYNTAX_TREE;
            } else {
                throw INVALID_OPTIONS;
            }
        } else {
            throw NO_OPTIONS;
        }

        if (argc > 2) {
            if (argv[2][0] == '-') {
                if (argv[2][1] == 'g' && option != SCANNER) {
                    graphical = true;
                    if (argc > 3) {
                        filename = argv[3];
                    }

                } else {
                    throw INVALID_OPTIONS;
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
        unique_ptr<Parser> parser;
        vector<Token> tokens;

        switch (option) {
            case SCANNER:
                tokens = scanner.all();
                for (unsigned int c = 0; c < tokens.size(); c++) {
                    cout << tokens[c] << endl;
                }
                break;
            case PARSER:
                parser = unique_ptr<Parser>(new Parser(&scanner));
                if (graphical) {
                    GraphObserver observer;
                    parser->addObserver(&observer);

                    try {
                        parser->parse();
                        cout << observer.getContent();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                } else {
                    TextObserver observer;
                    parser->addObserver(&observer);

                    try {
                        parser->parse();
                        cout << observer.getContent();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                }
                break;
            case SYMBOL_TABLE:
                parser = unique_ptr<Parser>(new Parser(&scanner, false));
                if (graphical) {
                    try {
                        parser->parse();
                        GraphicalSymbolTree treeBuilder(
                                parser->getSymbolTable());
                        cout << treeBuilder.getContent();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                } else {
                    try {
                        parser->parse();
                        ScopeVisitor visitor;
                        parser->getSymbolTable().getCurrentScope()
                                ->acceptVisitor(visitor);
                        cout << visitor.getContent();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                }
                break;
            case ABSTRACT_SYNTAX_TREE:
                parser = unique_ptr<Parser>(new Parser(&scanner, false));
                if (graphical) {
                    try {
                        parser->parse();
                    } catch (ParseException& e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                } else {
                    try {
                        parser->parse();
                        AbstractSyntaxTreeBuilder astBuilder(
                                parser->getSymbolTable(),
                                parser->getAbstractSyntaxTree());
                        cout << astBuilder.getContent();
                    } catch (ParseException &e) {
                        cerr << "error: " << e.getMessage() << endl;
                    }
                }
                break;
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