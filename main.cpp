#include <iostream>
#include <fstream>
#include "BrainfuckVM.h"

int main(int argc, const char *argv[]) {

    if (argc != 2) {
        std::cout << "USAGE:" << std::endl
        << "brainfuck-jit program.bf" << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::vector<char> program;
    if (!file.eof() && !file.fail()) {
        file.seekg(0, std::ios_base::end);
        std::streampos file_size = file.tellg();
        program.resize(file_size);

        file.seekg(0, std::ios_base::beg);
        file.read(&program[0], file_size);
    }

    BrainfuckVM bf_vm(std::cin, std::cout);
    bf_vm.Interpret(program);

    return EXIT_SUCCESS;
}