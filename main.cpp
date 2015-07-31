#include <fstream>
#include <iostream>
#include <regex>
#include "gmock/gmock.h"
#include "Brainfuck.h"

void Debugger(Brainfuck::Interpreter &vm, std::string program);

void UsageAndExit();

void PrintVMState(Brainfuck::Interpreter &vm);

int main(int argc, char **argv) {

    if (argc != 3 && argc != 4) {
        UsageAndExit();
    }

    std::ifstream program_file(argv[2]);
    if (!program_file) {
        std::cout << "Unknown program file " << argv[2];
        exit(EXIT_FAILURE);
    }

    // Read program
    std::string program;
    program_file.seekg(0, std::ios::end);
    program.resize((unsigned long) program_file.tellg());
    program_file.seekg(0, std::ios::beg);
    program_file.read(&program[0], program.size());
    program_file.close();


    std::istream *inputs = &std::cin;
    std::unique_ptr<std::ifstream> inputs_file = nullptr;
    if (argc == 4) {
        inputs_file.reset(new std::ifstream(argv[3]));
        inputs = inputs_file.get();
        if (!*inputs_file) {
            std::cout << "Unknown inputs file " << argv[3];
            exit(EXIT_FAILURE);
        }
    }

    Brainfuck::Interpreter vm(inputs);
    std::string mode = argv[1];
    if (mode == "-i") {
        vm.Interpret(program);
    } else if (mode == "-d") {
        Debugger(vm, program);
    } else if (mode == "-j" || mode == "-jo") {
        if (argc == 4) {
            std::cout << "Unfortunately, you cannot specify a file for the inputs while using the JIT" << std::endl;
            exit(EXIT_FAILURE);
        }
        bool optimised = mode == "-jo";
        Brainfuck::JIT(program, optimised);
    } else if (mode == "-c" || mode == "-co") {
        if (argc == 4) {
            std::cout << "Unfortunately, you cannot specify a file for the inputs while using the compiler" <<
            std::endl;
            exit(EXIT_FAILURE);
        }
        bool optimised = mode == "-co";
        Brainfuck::Compile(program, optimised);
    } else {
        UsageAndExit();
    }

    return 0;
}

void UsageAndExit() {
    std::cout << "USAGE:" << std::endl
                             << "Brainfuck { -i | -d | -c | -co | -j | -jo } program.bf [inputs.txt]" << std::endl;
    exit(EXIT_SUCCESS);
}

void Debugger(Brainfuck::Interpreter &vm, std::string program) {
    std::cout << "Brainfuck Debugger" << std::endl <<
    "'exit' to exit" << std::endl << std::endl;

    vm.Start(program);
    std::string debug_input;

    while (!vm.ProgramIsFinished()) {
        PrintVMState(vm);
        std::cout << "> ";
        std::cin >> debug_input;

        if (std::cin.eof()) {
            debug_input = "r";
            std::cout << "r";
        }

        std::regex pattern_step(R"([sS](\d+)?)");
        std::regex pattern_go(R"([gG](-)?(\d+))");
        std::regex pattern_dump(R"([dD](\d+)(-(\d+))?)");
        std::smatch matches;

        if (std::regex_search(debug_input, matches, pattern_step)) {
            if (matches.size() > 1 && matches[1].matched)
                vm.Step(static_cast<unsigned>(std::stoi(matches[1])));
            else
                vm.Step(1);
        } else if (debug_input == "r" || debug_input == "R") {
            vm.GoUntil(0, true);
        } else if (std::regex_search(debug_input, matches, pattern_go)) {
            unsigned target_ip = 0;
            if (matches.size() > 2 && matches[2].matched) {
                target_ip = static_cast<unsigned>(std::stoi(matches[2]));
                if (matches[1].matched) {
                    vm.GoUntil(target_ip, true);
                } else {
                    vm.GoUntil(target_ip);
                }
            }
        } else if (std::regex_search(debug_input, matches, pattern_dump)) {
            if (matches[1].matched) {
                if (matches[2].matched && matches[3].matched) {
                    // dump array between these positions
                    unsigned start = static_cast<unsigned>(std::stoi(matches[1]));
                    unsigned end = static_cast<unsigned>(std::stoi(matches[3]));
                    for (auto i = start; i <= end; i++) {
                        std::cout << (int) vm.GetMemory().at(i) << " ";
                    }
                    std::cout << std::endl;
                } else {
                    // dump array at given position
                    std::cout << (int) vm.GetMemory().at(static_cast<unsigned>(std::stoi(matches[1]))) << std::endl;
                }

                std::cout << std::endl;
            }
        } else if (debug_input == "exit") {
            exit(EXIT_SUCCESS);
        }
    }
}

void PrintVMState(Brainfuck::Interpreter &vm) {
    std::cout << "IP = " << vm.GetInstrPointer() << "\t";
    std::cout << "DP = " << vm.GetDataPointer() << std::endl;
    vm.PrintFormattedLocation();
    std::vector<char> mem = vm.GetMemory();
    mem.resize(10);

    std::string mem_output = "";
    long cursor_pos = -1;
    for (unsigned i = 0; i < mem.size(); i++) {
        if (i == vm.GetDataPointer())
            cursor_pos = mem_output.size();
        mem_output += std::to_string((int) mem[i]) + ", ";
    }

    mem_output += "...";
    if (cursor_pos == -1)
        cursor_pos = mem_output.size() - 1;

    std::cout << "DP: \t ";
    for (auto i = 0; i < cursor_pos; i++) {
        std::cout << " ";
    }
    std::cout << "v" << std::endl;
    std::cout << "MEM:\t[" << mem_output << "]" << std::endl << std::endl;

}