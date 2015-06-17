#include <iostream>
#include <fstream>
#include "brainfuck.h"

int main(int argc, const char* argv[]) {

    if(argc == 1) {
        brainfuck::interpret(std::cin, std::cout);
    } else if(argc == 2) {
        std::ifstream ifstream(argv[1]);
        if(!ifstream) {
            std::cerr << "Could not open file: " << argv[1] << std::endl;
            std::exit(EXIT_FAILURE);
        }
        brainfuck::interpret(ifstream, std::cout);
    } else {
        std::cout << "USAGE:" << std::endl
            << "1) brainfuck-jit < program.bf" << std::endl
            << "2) brainfuck-jit program.bf" << std::endl;
    }

    return EXIT_SUCCESS;
}