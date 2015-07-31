#include "Brainfuck.h"

using namespace Brainfuck;

static void AddPrologue();

static void AddEpilogue();

static void AddInstruction(char instr, std::vector<size_t> &open_brackets_stack, unsigned& brackets_cnt);

/*
 * Register Assignment:
 * rbx: data_pointer
 */

void Brainfuck::Compile(std::string program, bool optimised) {
    std::vector<size_t> open_brackets_stack;
    unsigned brackets_cnt = 0;

    AddPrologue();
    for (auto instr: program) {
        AddInstruction(instr, open_brackets_stack, brackets_cnt);
    }
    AddEpilogue();
}

static void AddInstruction(char instr, std::vector<size_t> &open_brackets_stack, unsigned& brackets_cnt) {
    switch (instr) {
        case '>':
            std::cout << "inc %rbx" << std::endl;
            break;
        case '<':
            std::cout << "dec %rbx" << std::endl;
            break;
        case '+':
            std::cout << "incb (%rbx)" << std::endl;
            break;
        case '-':
            std::cout << "decb (%rbx)" << std::endl;
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