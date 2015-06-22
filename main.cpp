#include <iostream>
#include <fstream>
#include "BrainfuckVM.h"

int main(int argc, const char *argv[]) {

    if (argc != 2 && argc != 3) {
        std::cout << "USAGE:" << std::endl
        << "brainfuck-jit program.bf [inputs_file]" << std::endl
        << "\tprogram.bf is the brainfuck program to execute" << std::endl
        << "\tinputs_file is an optional file containing user inputs for easier debugging";
        std::exit(EXIT_SUCCESS);
    }

    std::ifstream bf_program_file(argv[1]);
    if (!bf_program_file) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto input_stream = std::shared_ptr<std::istream>(&std::cin, [](void *) { });
    if (argc == 3) {
        input_stream = std::shared_ptr<std::istream>(new std::ifstream(argv[2]));
        if (!*input_stream) {
            std::cerr << "Could not open file: " << argv[2] << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    BrainfuckVM bf_vm(*input_stream, std::cout);
    std::vector<char> bf_program = BrainfuckVM::ReadProgram(bf_program_file);
    bf_vm.Debug(bf_program);

    return EXIT_SUCCESS;
}
