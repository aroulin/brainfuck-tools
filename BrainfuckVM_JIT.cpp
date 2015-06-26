#include "BrainfuckVM.h"
#include <sys/mman.h>
#include <string.h>

static void AddPrologue(std::vector<unsigned char> &code);

static void AddEpilogue(std::vector<unsigned char> &code);

static void Execute(std::vector<unsigned char> &code);

void BrainfuckVM::JIT(std::string program) {
    std::vector<unsigned char> code;
    AddPrologue(code);
    AddEpilogue(code);
    Execute(code);
}

static void AddPrologue(std::vector<unsigned char> &code) {
    code.insert(code.end(), {
            0x55,             // push %rbp
            0x48, 0x89, 0xe5, // mov %rsp, %rbp
            0x41, 0x54,       // push %r12
            0x49, 0x89, 0xFC, // mov %rsi, %r12
            0x41, 0x55,       // push %r13
            0x49, 0x89, 0xF5, // mov %rdi, %r13
            0x48, 0xC7, 0xC7, 0x59, 0x00, 0x00, 0x00,
            0x41, 0xFF, 0xD4
    });
}

static void AddEpilogue(std::vector<unsigned char> &code) {
    code.insert(code.end(), {
            0x41, 0x5d, // pop %r13
            0x41, 0x5c, // pop %r12
            0x5d,       // pop %rbp
            0xc3        // retq
    });
}

typedef int (*putchar_type)(int);
typedef int (*getchar_type)(void);

static void Execute(std::vector<unsigned char> &code) {
    void *memory = mmap(NULL, code.size(), PROT_EXEC | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    memcpy(memory, code.data(), code.size());
    void (*jit_code)(putchar_type, getchar_type) = (void (*)(putchar_type, getchar_type)) memory;
    jit_code(putchar, getchar);
    munmap(memory, code.size());
}