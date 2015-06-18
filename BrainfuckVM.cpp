#include <iostream>
#include "BrainfuckVM.h"

BrainfuckVM::BrainfuckVM(std::istream & input_stream, std::ostream & output_stream) : input_stream(&input_stream),
                                                                                      output_stream(&output_stream) {
}

void BrainfuckVM::Interpret(std::vector<char> & program) {
    this->program = program;
    reset();

    while(instr_pointer < program.size()) {
        Step(program[instr_pointer]);
    }
}

void BrainfuckVM::reset() {

    instr_pointer = 0;
    data_pointer = 0;
    memory.resize(BRAINFUCK_MEM_SIZE, 0);
}

void BrainfuckVM::Step(char cmd) {
    switch(cmd) {
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
        case '.':
            *output_stream << memory[data_pointer];
            break;
        case ',':
            *input_stream >> memory[data_pointer];
            break;
        case '[':
            if(memory[data_pointer] == 0) {
                // Jump after corresponding ']'
                unsigned remaining = 1;
                do {
                    instr_pointer++;
                    if(program[instr_pointer] == '[') {
                        remaining++;
                    } else if(program[instr_pointer] == ']') {
                        remaining--;
                    }
                } while(remaining != 0);
            }
            break;
        case ']':
            if(memory[data_pointer] != 0) {
                // Jump after corresponding ']'
                unsigned remaining = 1;
                do {
                    instr_pointer--;
                    if(program[instr_pointer] == ']') {
                        remaining++;
                    } else if(program[instr_pointer] == '[') {
                        remaining--;
                    }
                } while(remaining != 0);
            }
            break;
        default:
            break;
    }

    instr_pointer++;
}
