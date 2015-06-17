#include "brainfuck.h"
#include <iostream>

void Brainfuck::Interpret(std::istream &istream, std::ostream &ostream) {
    std::string str;
    while (istream >> str) {
        ostream << str;
    }
}
