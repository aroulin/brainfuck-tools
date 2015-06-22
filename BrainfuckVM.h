#ifndef BRAINFUCK_JIT_BRAINFUCKVM_H
#define BRAINFUCK_JIT_BRAINFUCKVM_H

#include <array>
#include <memory>
#include <vector>
#include <iostream>

#define BRAINFUCK_MEM_SIZE 30000

class BrainfuckVM {
public:
    BrainfuckVM(std::istream &input_stream, std::ostream &output_stream);

    void Interpret(std::vector<char> &program);

    void Debug(std::vector<char> vector, std::istream &debug_stream = std::cin);

    static std::vector<char> ReadProgram(std::ifstream &bf_program_file);

private:
    std::vector<char> program;
    std::vector<char> unstripped_program;
    unsigned instr_pointer;
    std::vector<char> memory;
    unsigned data_pointer;

    std::istream *input_stream;
    std::ostream *output_stream;

    void reset();

    void Step(unsigned nb_step = 1);
};


#endif //BRAINFUCK_JIT_BRAINFUCKVM_H
