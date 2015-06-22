#include <iostream>
#include <algorithm>
#include <set>
#include <fstream>
#include <regex>
#include "BrainfuckVM.h"
#include "terminal_color.h"

#define STEP_UNTIL_END 0

BrainfuckVM::BrainfuckVM(std::istream &input_stream, std::ostream &output_stream) : input_stream(&input_stream),
                                                                                    output_stream(&output_stream) {
}

static bool not_brainfuck_cmd(char cmd) {
    std::set<char> valid_cmds{'>', '<', '+', '-', '.', ',', '[', ']'};
    return valid_cmds.find(cmd) == valid_cmds.end();
}

static std::vector<char> strip_program(std::vector<char> unstripped_program) {
    unstripped_program.erase(std::remove_if(unstripped_program.begin(), unstripped_program.end(), not_brainfuck_cmd),
                             unstripped_program.end());
    unstripped_program.shrink_to_fit();
    return unstripped_program;
}

void BrainfuckVM::Interpret(std::vector<char> &program) {
    this->program = strip_program(program);
    reset();

    Step(STEP_UNTIL_END);

    *output_stream << std::endl << "Done" << std::endl;
}

void BrainfuckVM::reset() {

    instr_pointer = 0;
    data_pointer = 0;
    memory.resize(BRAINFUCK_MEM_SIZE, 0);
}

/**
 * @param nb_step number of steps to do, 0 means step until end of program (See STEP_UNTIL_END)
 * @param prompt_input prompt to show when asking for input
 * @param prompt_output prompt to show when giving output
 */
void BrainfuckVM::Step(unsigned nb_step) {
    char read = 0;

    for (auto i = 0; instr_pointer < program.size() && (nb_step == 0 || i < nb_step); i++) {
        switch (program[instr_pointer]) {
            case '>':
                data_pointer++;
                break;
            case '<':
                data_pointer--;
                break;
            case '+':
                memory[data_pointer]++;
                break;
            case '-':
                memory[data_pointer]--;
                break;
            case '.':
                *output_stream << memory[data_pointer];
                break;
            case ',':
                *input_stream >> read;
                if (!input_stream->eof()) // no change when EOF on input
                    memory[data_pointer] = read;
                break;
            case '[':
                if (memory[data_pointer] == 0) {
                    // Jump after corresponding ']'
                    unsigned remaining = 1;
                    do {
                        instr_pointer++;
                        if (program[instr_pointer] == '[') {
                            remaining++;
                        } else if (program[instr_pointer] == ']') {
                            remaining--;
                        }
                    } while (remaining != 0);
                }
                break;
            case ']':
                if (memory[data_pointer] != 0) {
                    // Jump after corresponding '['
                    unsigned remaining = 1;
                    do {
                        instr_pointer--;
                        if (program[instr_pointer] == ']') {
                            remaining++;
                        } else if (program[instr_pointer] == '[') {
                            remaining--;
                        }
                    } while (remaining != 0);
                }
                break;
            default:
                break;
        }

        instr_pointer++;
    }
}

void BrainfuckVM::Debug(std::vector<char> program, std::istream &debug_stream) {
    this->unstripped_program = program;
    this->program = strip_program(program);

    reset();

    while (instr_pointer < this->program.size()) {
        *output_stream << "> ";
        std::string debug_cmd;
        debug_stream >> debug_cmd;

        if (debug_stream.eof()) {
            debug_cmd = "R";
            *output_stream << "R";
        }

        std::regex pattern_step(R"(S(\d+)?)");
        std::regex pattern_go(R"(G(-)?(\d+))");
        std::regex pattern_dump(R"(D(\d+)(-(\d+))?)");
        std::smatch matches;

        if (std::regex_search(debug_cmd, matches, pattern_step)) {
            if (matches.size() > 1 && matches[1].matched)
                Step(static_cast<unsigned>(std::stoi(matches[1])));
            else
                Step(1);
        } else if (debug_cmd == "R") {
            Step(STEP_UNTIL_END);
        } else if (std::regex_search(debug_cmd, matches, pattern_go)) {
            unsigned target_ip = 0;
            if (matches.size() > 2 && matches[2].matched) {
                target_ip = static_cast<unsigned>(std::stoi(matches[2]));

                if (matches[1].matched) {
                    target_ip = static_cast<unsigned>(this->program.size() - target_ip);
                }

                while (instr_pointer != target_ip && instr_pointer < this->program.size()) {
                    Step(1);
                }

            }
        } else if (std::regex_search(debug_cmd, matches, pattern_dump)) {
            if (matches[1].matched) {

                if (matches[2].matched && matches[3].matched) {
                    // dump array between these positions
                    unsigned start = static_cast<unsigned>(std::stoi(matches[1]));
                    unsigned end = static_cast<unsigned>(std::stoi(matches[3]));
                    for (auto i = start; i <= end; i++) {
                        *output_stream << static_cast<unsigned>(static_cast<unsigned char>(memory.at(i))) << " ";
                    }
                    *output_stream << std::endl;
                } else {
                    // dump array at given position
                    *output_stream <<
                    static_cast<unsigned>(static_cast<unsigned char>(memory.at(
                            static_cast<unsigned>(std::stoi(matches[1]))))) << std::endl;
                }
            }
        } else if (debug_cmd == "P") {
            *output_stream << "DP: " << data_pointer << std::endl;
        } else if (debug_cmd == "L") {
            unsigned strip_program_index = 0;
            unsigned unstripped_program_index = 0;

            for (strip_program_index = 0; strip_program_index < instr_pointer; strip_program_index++) {
                while (not_brainfuck_cmd(this->unstripped_program[unstripped_program_index])) {
                    *output_stream << this->unstripped_program[unstripped_program_index];
                    unstripped_program_index++;
                }

                *output_stream << this->unstripped_program[unstripped_program_index];
                unstripped_program_index++;
            }

            while (not_brainfuck_cmd(this->unstripped_program[unstripped_program_index])) {
                *output_stream << this->unstripped_program[unstripped_program_index];
                unstripped_program_index++;
            }

            // print current instr_pointer cmd in color
            *output_stream << "\x1B[1m" KRED << this->unstripped_program[unstripped_program_index] << RST;
            unstripped_program_index++;

            for (; unstripped_program_index < unstripped_program.size(); unstripped_program_index++) {
                *output_stream << this->unstripped_program[unstripped_program_index];
            }

            *output_stream << std::endl << "IP: " << instr_pointer << std::endl;
        }
    }

    *output_stream << std::endl << "Done" << std::endl;
}

std::vector<char> BrainfuckVM::ReadProgram(std::ifstream &bf_program_file) {
    std::vector<char> bf_program;
    if (!bf_program_file.eof() && !bf_program_file.fail()) {
        bf_program_file.seekg(0, std::ios_base::end);
        std::streampos file_size = bf_program_file.tellg();
        bf_program.resize(static_cast<unsigned long>(file_size));
        bf_program_file.seekg(0, std::ios_base::beg);
        bf_program_file.read(&bf_program[0], file_size);
    }
    return bf_program;
}
