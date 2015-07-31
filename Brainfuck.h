#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <set>

namespace Brainfuck {
    void JIT(std::string program, bool optimised = false);

    void Compile(std::string program, bool optimised = false);

    class Interpreter {
    public:

        Interpreter(std::istream *inputs = &std::cin);

        void Interpret(std::string program);

        void Start(const std::string program);

        void Step(unsigned num_steps);

        void GoUntil(unsigned long target_instr_pointer, bool offset_is_from_end = false);

        void Reset();

        unsigned GetInstrPointer() const { return instr_pointer; }

        unsigned GetDataPointer() const { return data_pointer; }

        std::vector<char> GetMemory() const { return memory; }

        static const size_t memory_size = 30000;

        void PrintFormattedLocation();

        bool ProgramIsFinished();

    private:
        unsigned instr_pointer = 0;
        std::string program;
        unsigned data_pointer = 0;
        std::vector<char> memory = std::vector<char>(memory_size, 0);
        std::istream *inputs;

        static bool NotBrainfuckCmd(char cmd);

        std::string StripUnwantedCharacters(const std::string &program);

        void SingleStep();

        std::string original_program;
    };

}

#endif //BRAINFUCK_H
