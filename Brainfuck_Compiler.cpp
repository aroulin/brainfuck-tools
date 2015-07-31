#include "Brainfuck.h"

using namespace Brainfuck;

static void AddPrologue();

static void AddEpilogue();

static void AddInstruction(char instr, std::vector<size_t> &open_brackets_stack, unsigned &brackets_cnt,
                           size_t optimised_count = 1);

static bool CanBeOptimised(char instr);

/*
 * Register Assignment:
 * rbx: data_pointer
 */

void Brainfuck::Compile(std::string program, bool optimised) {
    std::vector<size_t> open_brackets_stack;
    unsigned brackets_cnt = 0;

    AddPrologue();

    size_t optimised_count = 0;
    char last_optimised_instr = 0;
    for (auto instr: program) {
        if (optimised && instr == last_optimised_instr && CanBeOptimised(instr)) {
            optimised_count++;
        } else {
            if (optimised_count > 0)
                AddInstruction(last_optimised_instr, open_brackets_stack, brackets_cnt, optimised_count);
            if (CanBeOptimised(instr)) {
                last_optimised_instr = instr;
                optimised_count = 1;
            } else {
                AddInstruction(instr, open_brackets_stack, brackets_cnt);
                optimised_count = 0;
            }
        }
    }

    AddEpilogue();
}

static bool CanBeOptimised(char instr) {
    return instr == '+' || instr == '-' || instr == '<' || instr == '>';
}

static void AddInstruction(char instr, std::vector<size_t> &open_brackets_stack, unsigned &brackets_cnt,
                           size_t optimised_count) {
    switch (instr) {
        case '>':
            std::cout << "add $" << optimised_count << ", %rbx" << std::endl;
            break;
        case '<':
            std::cout << "sub $" << optimised_count << ", %rbx" << std::endl;
            break;
        case '+':
            std::cout << "add $" << optimised_count << ", (%rbx)" << std::endl;
            break;
        case '-':
            std::cout << "sub $" << optimised_count << ", (%rbx)" << std::endl;
            break;
        case '.':
            std::cout << "movzxb (%rbx), %rdi" << std::endl
                      << "call putchar"       << std::endl;
            break;
        case ',':
            std::cout << "call getchar"       << std::endl
                      << "movb %al, (%rbx)"    << std::endl;
            break;
        case '[':
            brackets_cnt++;
            open_brackets_stack.push_back(brackets_cnt);
            std::cout   << "cmpb $0, (%rbx)" << std::endl
                        << "je end_bracket_" << brackets_cnt << std::endl
                        << "begin_bracket_"  << brackets_cnt << ":" << std::endl;
            break;
        case ']':
            std::cout   << "cmpb $0, (%rbx)" << std::endl
                        << "jne begin_bracket_" << open_brackets_stack.back() << std::endl
                        << "end_bracket_" << open_brackets_stack.back() << ":" << std::endl;
            open_brackets_stack.pop_back();
            break;
        default:
            break;
    }
}

static void AddPrologue() {
    std::cout   << ".text"              << std::endl
                << ".globl main"        << std::endl
                << "main:"              << std::endl
                << "push %rbp"          << std::endl
                << "mov %rsp, %rbp"     << std::endl
                << "push %rbx"          << std::endl
                << "mov $30000, %rdi"   << std::endl
                << "sub $8, %rsp"       << std::endl
                << "call malloc"        << std::endl
                << "mov %rax, %rbx"     << std::endl;
}

static void AddEpilogue() {
    std::cout   << "add $8, %rsp"       << std::endl
                << "pop %rbx"           << std::endl
                << "pop %rbp"           << std::endl
                << "retq"               << std::endl;
}