#include <iostream>
#include <fstream>
#include "BrainfuckVM.h"

int main(int argc, const char *argv[]) {

    if (argc != 2 && argc != 3) {
        std::cout << "USAGE:" << std::endl
        << "brainfuck-jit program.bf [inputs_file]" << std::endl;
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

    std::istream* input_stream = &std::cin;

    if(argc == 3) {
         std::ifstream* inputs_file = new std::ifstream(argv[2]);
        if(!*inputs_file) {
            std::cerr << "Could not open file: " << argv[1] << std::endl;
            std::exit(EXIT_FAILURE);
        }

        input_stream = inputs_file;
    }

    BrainfuckVM bf_vm(*input_stream, std::cout);
    bf_vm.Debug(program);
    if(argc == 3) {
        delete input_stream;
    }

    return EXIT_SUCCESS;
}