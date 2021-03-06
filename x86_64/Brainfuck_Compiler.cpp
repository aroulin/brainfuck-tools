#include "Brainfuck.h"

using namespace Brainfuck;

static void AddPrologue();

static void AddInstructions(std::string program, bool optimised);

static void AddEpilogue();

static bool CanOptimise(std::string &program, std::string::iterator &instr);

static bool IsOptimisableInstr(char instr);

/*
* Register Assignment:
* rbx: data_pointer
*/

void Brainfuck::Compile(std::string program, bool optimised) {
    AddPrologue();
    AddInstructions(program, optimised);
    AddEpilogue();
}

static bool CanOptimise(std::string &program, std::string::iterator &instr) {
    return IsOptimisableInstr(*instr) && (instr + 1) != program.end() && *(instr + 1) == *instr;
}

static bool IsOptimisableInstr(char instr) {
    return instr == '+' || instr == '-' || instr == '>' || instr == '<';
}

static void AddInstructions(std::string program, bool optimised) {

    std::vector<size_t> open_brackets_stack;
    unsigned brackets_cnt = 0;

    for (auto instr = program.begin(); instr < program.end(); instr++) {

        size_t optimised_count;
        for (optimised_count = 1; optimised && CanOptimise(program, instr); optimised_count++, instr++);

        switch (*instr) {
            case '>':
                std::cout << "add $" << optimised_count << ", %rbx" << std::endl;
                break;
            case '<':
                std::cout << "sub $" << optimised_count << ", %rbx" << std::endl;
                break;
            case '+':
                std::cout << "addb $" << optimised_count << ", (%rbx)" << std::endl;
                break;
            case '-':
                std::cout << "subb $" << optimised_count << ", (%rbx)" << std::endl;
                break;
            case '.':
                std::cout << "movzxb (%rbx), %rdi" << std::endl
                << "call putchar" << std::endl;
                break;
            case ',':
                std::cout << "call getchar" << std::endl
                << "movb %al, (%rbx)" << std::endl;
                break;
            case '[':
                brackets_cnt++;
                open_brackets_stack.push_back(brackets_cnt);
                std::cout << "cmpb $0, (%rbx)" << std::endl
                << "je end_bracket_" << brackets_cnt << std::endl
                << "begin_bracket_" << brackets_cnt << ":" << std::endl;
                break;
            case ']':
                std::cout << "cmpb $0, (%rbx)" << std::endl
                << "jne begin_bracket_" << open_brackets_stack.back() << std::endl
                << "end_bracket_" << open_brackets_stack.back() << ":" << std::endl;
                open_brackets_stack.pop_back();
                break;
            default:
                break;
        }
    }
}

static void AddPrologue() {
    std::cout   << ".text"              << std::endl
                << ".globl main"        << std::endl
                << "main:"              << std::endl
                << "push %rbp"          << std::endl
                << "mov %rsp, %rbp"     << std::endl
                << "push %rbx"          << std::endl
                << "push %r12"          << std::endl
                << "mov $30000, %rdi"   << std::endl
                << "mov $1, %rsi"       << std::endl
                << "call calloc"        << std::endl
                << "mov %rax, %rbx"     << std::endl
                << "mov %rax, %r12"     << std::endl;
}

static void AddEpilogue() {
    std::cout   << "add $8, %rsp"       << std::endl
                << "mov %r12, %rdi"     << std::endl
                << "call free"          << std::endl
                << "pop %rbx"           << std::endl
                << "pop %rbp"           << std::endl
                << "retq"               << std::endl;
}