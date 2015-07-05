#include <iostream>
#include "BrainfuckVM.h"

void BrainfuckVM::Interpret(const std::string program_to_interpret) {
    Start(program_to_interpret);
    while (!ProgramIsFinished()) {
        SingleStep();
    }
}

void BrainfuckVM::Start(const std::string program_to_debug) {
    instr_pointer = 0;
    this->original_program = program_to_debug;
    this->program = StripUnwantedCharacters(program_to_debug);
}

bool BrainfuckVM::NotBrainfuckCmd(char cmd) {
    std::set<char> valid_cmds{'>', '<', '+', '-', '.', ',', '[', ']'};
    return valid_cmds.find(cmd) == valid_cmds.end();
}

std::string BrainfuckVM::StripUnwantedCharacters(const std::string &program) {
    std::string stripped_program(program);
    stripped_program.erase(std::remove_if(stripped_program.begin(), stripped_program.end(), NotBrainfuckCmd),
                           stripped_program.end());
    return stripped_program;
}

void BrainfuckVM::Step(unsigned num_steps) {
    for (unsigned i = 0; i < num_steps; i++) {
        SingleStep();
    }
}

void BrainfuckVM::SingleStep() {
    if (ProgramIsFinished())
        return;

    switch (program.at(instr_pointer)) {
        case '>':
            data_pointer++;
            break;
        case '<':
            data_pointer--;
            break;
        case '+':
            memory[data_pointer]++;
            break;
        case '-':
            memory[data_pointer]--;
            break;
        case '[': {
            if (memory[data_pointer] != 0)
                break;
            int remainingCloseBrackets = 1;
            do {
                instr_pointer++;
                if (program[instr_pointer] == '[')
                    remainingCloseBrackets++;
                else if (program[instr_pointer] == ']')
                    remainingCloseBrackets--;
            } while (remainingCloseBrackets > 0);
            break;
        }
        case ']': {
            if (memory[data_pointer] == 0)
                break;
            int remainingOpenBrackets = 1;
            do {
                instr_pointer--;
                if (program[instr_pointer] == ']')
                    remainingOpenBrackets++;
                else if (program[instr_pointer] == '[')
                    remainingOpenBrackets--;
            } while (remainingOpenBrackets > 0);
        }
            break;
        case '.':
            std::cout << (char) memory[data_pointer];
            break;
        case ',':
            *inputs >> memory[data_pointer];
            break;
    }

    instr_pointer++;
}

bool BrainfuckVM::ProgramIsFinished() {
    return instr_pointer >= program.size();
}

void BrainfuckVM::Reset() {
    instr_pointer = 0;
    data_pointer = 0;
    std::fill(memory.begin(), memory.end(), 0);
}

void BrainfuckVM::GoUntil(unsigned target_instr_pointer, bool offset_is_from_end) {
    if (offset_is_from_end)
        target_instr_pointer = program.size() - target_instr_pointer;
    while (!ProgramIsFinished() && instr_pointer != target_instr_pointer) {
        SingleStep();
    }
}

void BrainfuckVM::PrintFormattedLocation() {
    unsigned original_ip = 0;
    unsigned ip = 0;
    unsigned line = 1;
    unsigned line_start_offset = 0;
    unsigned offset_in_line = 0;
    do {
        if (ip < instr_pointer) {
            ip++;
            offset_in_line++;
            original_ip++;
        }

        while (NotBrainfuckCmd(original_program.at(original_ip))) {
            if (original_program.at(original_ip) == '\n') {
                line++;
                line_start_offset = original_ip + 1;
                offset_in_line = 0;
            } else {
                offset_in_line++;
            }
            original_ip++;
        }
    } while (ip < instr_pointer);

    std::cout << "IP:\t";

    for (auto i = line_start_offset; original_program[i] != '\n'; i++) {
        if (i - line_start_offset == offset_in_line)
            std::cout << "v";
        else
            std::cout << " ";
    }

    std::cout << std::endl;
    std::cout << "L" << line << ":\t";

    for (auto i = line_start_offset; original_program[i] != '\n'; i++) {
        std::cout << original_program[i];
    }

    std::cout << std::endl;
}

BrainfuckVM::BrainfuckVM(std::istream *inputs) : inputs(inputs) {

}
