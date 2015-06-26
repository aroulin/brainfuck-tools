#ifndef BRAINFUCK_JIT_BRAINFUCKVM_H
#define BRAINFUCK_JIT_BRAINFUCKVM_H

#include <vector>
#include <string>
#include <algorithm>
#include <set>

class BrainfuckVM {
public:

    void JIT(std::string program);

    void Interpret(std::string program);

    void Start(const std::string program);

    void Step(unsigned num_steps);

    void GoUntil(unsigned int target_instr_pointer, bool offset_is_from_end = false);

    void Reset();

    unsigned GetInstrPointer() const { return instr_pointer; }

    unsigned GetDataPointer() const { return data_pointer; }

    std::vector<char> GetMemory() const { return memory; }

    static const size_t memory_size = 30000;

    std::string GetFormattedLocation();

private:
    unsigned instr_pointer = 0;
    std::string program;
    unsigned data_pointer = 0;
    std::vector<char> memory = std::vector<char>(memory_size, 0);

    static bool NotBrainfuckCmd(char cmd);

    std::string StripUnwantedCharacters(const std::string &program);

    void SingleStep();

    bool ProgramIsFinished();
};

#endif //BRAINFUCK_JIT_BRAINFUCKVM_H
