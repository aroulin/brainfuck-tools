#include "BrainfuckVM.h"

void BrainfuckVM::Interpret(const std::string program_to_interpret) {
    instr_pointer = 0;

    this->program = StripUnwantedCharacters(program_to_interpret);

    while (!ProgramIsFinished()) {
        SingleStep();
    }
}

void BrainfuckVM::Start(const std::string program_to_debug) {
    instr_pointer = 0;
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

void BrainfuckVM::Step(unsigned int num_steps) {
    for (auto i = 0; i < num_steps; i++) {
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

void BrainfuckVM::GoUntil(int target_instr_pointer, bool offset_is_from_end) {
    if (offset_is_from_end)
        target_instr_pointer = program.size() - target_instr_pointer;
    while (!ProgramIsFinished() && instr_pointer != target_instr_pointer) {
        SingleStep();
    }
}
