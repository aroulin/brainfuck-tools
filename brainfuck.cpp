#include "brainfuck.h"
#include <iostream>

void brainfuck::interpret(std::istream & istream, std::ostream & ostream) {
    std::string str;
    while(istream >> str) {
        ostream << str;
    }
}
