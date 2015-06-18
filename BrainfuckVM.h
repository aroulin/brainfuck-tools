#ifndef BRAINFUCK_JIT_BRAINFUCKVM_H
#define BRAINFUCK_JIT_BRAINFUCKVM_H

#include <array>
#include <memory>
#include <vector>

#define BRAINFUCK_MEM_SIZE 30000

class BrainfuckVM {
public:
    BrainfuckVM(std::istream& input_stream, std::ostream& output_stream);

    void Interpret(std::vector<char>& program);
    void Step(char cmd);
private:
    std::vector<char> program;
    unsigned instr_pointer;
    std::vector<char> memory;
    unsigned data_pointer;
    std::vector<unsigned> loop_stack;

    std::istream* input_stream;
    std::ostream* output_stream;

    void reset();
};


#endif //BRAINFUCK_JIT_BRAINFUCKVM_H
