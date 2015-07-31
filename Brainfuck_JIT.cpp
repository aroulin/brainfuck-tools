#include "Brainfuck.h"
#include <sys/mman.h>
#include <string.h>

using namespace Brainfuck;

static void AddPrologue(std::vector<unsigned char> &code);

static void AddEpilogue(std::vector<unsigned char> &code);

static void Execute(std::vector<unsigned char> &code);

static void AddInstruction(char instr, std::vector<unsigned char> &code,
                           std::vector<size_t> &open_brackets_offsets_stack, size_t optimised_count = 1);

static bool CanBeOptimised(char instr);

static void WriteLittleEndian(std::vector<unsigned char> &code, size_t offset, long value);

/*
 * Register Assignment:
 * rbx: data_pointer
 * r12: putchar
 * r13: getchar
 */

void Brainfuck::JIT(std::string program, bool optimised) {
    std::vector<unsigned char> code;
    std::vector<size_t> open_brackets_offsets_stack;

    AddPrologue(code);

    size_t optimised_count = 0;
    char last_optimised_instr = 0;
    for (auto instr: program) {
        if (optimised && instr == last_optimised_instr && CanBeOptimised(instr)) {
            optimised_count++;
        } else {
            if (optimised_count > 0)
                AddInstruction(last_optimised_instr, code, open_brackets_offsets_stack, optimised_count);
            if (CanBeOptimised(instr)) {
                last_optimised_instr = instr;
                optimised_count = 1;
            } else {
                AddInstruction(instr, code, open_brackets_offsets_stack);
                optimised_count = 0;
            }
        }
    }

    AddEpilogue(code);

    Execute(code);
}

static bool CanBeOptimised(char instr) {
    return instr == '+' || instr == '-' || instr == '<' || instr == '>';
}

static void AddInstruction(char instr, std::vector<unsigned char> &code,
                           std::vector<size_t> &open_brackets_offsets_stack, size_t optimised_count) {
    switch (instr) {
        case '>':
            code.insert(code.end(), {
                    0x48, 0x81, 0xC3, 0x00, 0x00, 0x00, 0x00 // add rbx, optimised_count
            });
            WriteLittleEndian(code, code.size(), optimised_count);
            break;
        case '<':
            code.insert(code.end(), {
                    0x48, 0x81, 0xEB, 0x00, 0x00, 0x00, 0x00 // sub rbx, optimised_count
            });
            WriteLittleEndian(code, code.size(), optimised_count);
            break;
        case '+':
            code.insert(code.end(), {
                    0x48, 0x81, 0x03, 0x00, 0x00, 0x00, 0x00  // addq [rbx], optimised_count
            });
            WriteLittleEndian(code, code.size(), optimised_count);
            break;
        case '-':
            code.insert(code.end(), {
                    0x48, 0x81, 0x2B, 0x00, 0x00, 0x00, 0x00 // subq [rbx], optimised_count
            });
            WriteLittleEndian(code, code.size(), optimised_count);
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
        case '[':
            code.insert(code.end(), {
                    0x80, 0x3B, 0x00, // cmpb [%rbx], 0
                    0x0F, 0x84, 0x00, 0x00, 0x00, 0x00 // je matching ]
            });
            open_brackets_offsets_stack.push_back(code.size());
            break;
        case ']': {
            code.insert(code.end(), {
                    0x80, 0x3B, 0x00, // cmpb [%rbx], 0
                    0x0F, 0x85, 0x00, 0x00, 0x00, 0x00 // jne matching ]
            });

            size_t corresponding_open_bracket_offset = open_brackets_offsets_stack.back();
            open_brackets_offsets_stack.pop_back();

            size_t offset_btw_matching_brackets = code.size() - corresponding_open_bracket_offset;
            WriteLittleEndian(code, corresponding_open_bracket_offset, offset_btw_matching_brackets);
            WriteLittleEndian(code, code.size(), -offset_btw_matching_brackets);
        }
            break;
        default:
            break;
    }
}

static void WriteLittleEndian(std::vector<unsigned char> &code, size_t offset, long value) {
    code[offset - 4] = (unsigned char) (value & 0xFF);
    code[offset - 3] = (unsigned char) ((value >> 8) & 0xFF);
    code[offset - 2] = (unsigned char) ((value >> 16) & 0xFF);
    code[offset - 1] = (unsigned char) ((value >> 24) & 0xFF);
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
    unsigned char *memory_array = new unsigned char[Interpreter::memory_size];
    memset(memory_array, 0, Interpreter::memory_size);

    jit_code(putchar, getchar, memory_array);
    munmap(code_area, code.size());
    delete[] memory_array;
}
