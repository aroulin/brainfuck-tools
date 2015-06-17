#ifndef BRAINFUCK_JIT_BRAINFUCK_H
#define BRAINFUCK_JIT_BRAINFUCK_H

#include <iosfwd>

namespace brainfuck {

    void interpret(std::istream & istream, std::ostream & ostream);
}

#endif //BRAINFUCK_JIT_BRAINFUCK_H
