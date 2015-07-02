#include <fstream>
#include "gmock/gmock.h"
#include "BrainfuckVM.h"

void Debugger(BrainfuckVM &vm, std::string program);

void UsageAndExit();

void PrintVMState(BrainfuckVM &vm);

int main(int argc, char **argv) {
/*
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();*/

    if (argc != 3 && argc != 4) {
        UsageAndExit();
    }

    std::ifstream program_file(argv[2]);
    if (!program_file) {
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
    if (mode == "-i") {
        vm.Interpret(program);
    } else if (mode == "-d") {
        Debugger(vm, program);
    } else if (mode == "-j") {
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

void Debugger(BrainfuckVM &vm, std::string program) {
    std::cout << "Brainfuck Debugger" << std::endl <<
    "'exit' to exit" << std::endl << std::endl;

    vm.Start(program);
    std::string debug_input;

    while (!vm.ProgramIsFinished()) {
        PrintVMState(vm);
        std::cout << "> ";
        std::cin >> debug_input;

        if (debug_input == "s") {
            vm.Step(1);
        } else if (debug_input == "r") {
            vm.GoUntil(0, true);
        } else if (debug_input == "exit") {
            exit(EXIT_SUCCESS);
        }
    }
}

void PrintVMState(BrainfuckVM &vm) {
    std::cout << "IP = " << vm.GetInstrPointer() << "\t";
    std::cout << "DP = " << vm.GetDataPointer() << std::endl;
    vm.PrintFormattedLocation();
    std::cout << "MEM = [";
    std::vector<char> mem = vm.GetMemory();
    mem.resize(10);
    for (int i: mem) {
        std::cout << i << ", ";
    }

    std::cout << "...]" << std::endl;
}