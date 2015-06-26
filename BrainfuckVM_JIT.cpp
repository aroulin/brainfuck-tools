#include "BrainfuckVM.h"
#include <sys/mman.h>
#include <string.h>

static void AddPrologue(std::vector<unsigned char> &code);

static void AddEpilogue(std::vector<unsigned char> &code);

static void Execute(std::vector<unsigned char> &code);

static void AddInstruction(char instr, std::vector<unsigned char> &code);

/*
 * Register Assignment:
 * rbx: data_pointer
 * r12: putchar
 * r13: getchar
 */

void BrainfuckVM::JIT(std::string program) {
    std::vector<unsigned char> code;
    AddPrologue(code);
    for (auto instr: program) {
        AddInstruction(instr, code);
    }
    AddEpilogue(code);

    Execute(code);
}

static void AddInstruction(char instr, std::vector<unsigned char> &code) {
    switch (instr) {
        case '>':
            code.insert(code.end(), {
                    0x48, 0xFF, 0xC3 // inc %rbx
            });
            break;
        case '<':
            code.insert(code.end(), {
                    0x48, 0xFF, 0xCB // inc %rbx
            });
            break;
        case '+':
            code.insert(code.end(), {
                    0xFE, 0x03 // incb (%rbx)
            });
            break;
        case '-':
            code.insert(code.end(), {
                    0xFE, 0x0B // decb (%rbx)
            });
            break;
        case '.':
            code.insert(code.end(), {
                    0x48, 0x0F, 0xB6, 0x3B, // movzxb (%rbx), %rdi
                    0x41, 0xFF, 0xD4        // call %r12
            });
            break;
        case ',':
            code.insert(code.end(), {
                    0x41, 0xFF, 0xD5, // call %r13
                    0x88, 0x03        // mov (%rbx), %al
            });
            break;
    }
}

static void AddPrologue(std::vector<unsigned char> &code) {
    code.insert(code.end(), {
            0x55,             // push %rbp
            0x48, 0x89, 0xe5, // mov %rsp, %rbp
            0x41, 0x54,       // push %r12
            0x49, 0x89, 0xFC, // mov %rdi, %r12
            0x41, 0x55,       // push %r13
            0x49, 0x89, 0xF5, // mov %rsi, %r13
            0x53,             // push %rbx
            0x48, 0x89, 0xD3, // mov %rdx, %rbx
            0x48, 0x83, 0xEC, 0x08 // sub rsp, 8
    });
}

static void AddEpilogue(std::vector<unsigned char> &code) {
    code.insert(code.end(), {
            0x48, 0x83, 0xC4, 0x08, // add rsp, 8
            0x5B,       // pop %rbx
            0x41, 0x5d, // pop %r13
            0x41, 0x5c, // pop %r12
            0x5d,       // pop %rbp
            0xc3,       // retq
    });
}

typedef int (*putchar_type)(int);

typedef int (*getchar_type)(void);

static void Execute(std::vector<unsigned char> &code) {
    void *code_area = mmap(NULL, code.size(), PROT_EXEC | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    memcpy(code_area, code.data(), code.size());
    void (*jit_code)(putchar_type, getchar_type, unsigned char *) = (void (*)(putchar_type, getchar_type,
                                                                              unsigned char *)) code_area;

    // allocate brainfuck memory array
    unsigned char *memory_array = new unsigned char[BrainfuckVM::memory_size];

    jit_code(putchar, getchar, memory_array);
    munmap(code_area, code.size());
}