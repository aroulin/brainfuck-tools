#include <fstream>
#include "gmock/gmock.h"
#include "BrainfuckVM.h"

void Debugger(BrainfuckVM& vm);

void UsageAndExit();

int main(int argc, char **argv) {
/*
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();*/

    if(argc != 2 && argc != 3) {
        UsageAndExit();
    }

    std::ifstream program_file(argv[2]);
    if(!program_file) {
        std::cout << "Unknown program file " << argv[1];
        exit(EXIT_FAILURE);
    }

    std::string program;
    program_file.seekg(0, std::ios::end);
    program.resize(program_file.tellg());
    program_file.seekg(0, std::ios::beg);
    program_file.read(&program[0], program.size());
    program_file.close();

    BrainfuckVM vm;
    std::string mode = argv[1];
    if(mode == "-i") {
        vm.Interpret(program);
    } else if(mode == "-d") {
        Debugger(vm);
    } else if(mode == "-j") {
        vm.JIT(program);
    } else {
        UsageAndExit();
    }
}

void UsageAndExit() {
    std::cout << "USAGE:" << std::endl
    << "BrainfuckVM { -i | -d | -j } program.bf [inputs.txt]" << std::endl;
    exit(EXIT_SUCCESS);
}

void Debugger(BrainfuckVM& vm) {

}