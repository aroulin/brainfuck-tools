# brainfuck-tools
Brainfuck Compiler, JIT, Interpreter and Debugger

## Building

      mkdir build && cd build
      cmake ..
      make

## How to use it

      ./Brainfuck {-i | -d | -c | -j | -jo} program.bf [inputs.txt]

  ```-i``` for the Interpreter
  
  ```-d``` for the Debugger

  ```-c``` for the Compiler

  ```-j``` for the JIT
  
  ```-jo``` for optimised JIT

  ```program.bf``` must be a text file containing the Brainfuck program to execute/compile

  ```inputs.txt``` is an optional file containing inputs instead of using stdin (useful for debugging, see Notes below)

## Interpreter

      ./Brainfuck -i program.bf

Simple interpretation of the program (much slower than Compiler and JIT)

## Compiler

      ./Brainfuck -c program.bf > compiled_brainfuck.s
      gcc compiled_brainfuck.s -o executable_brainfuck
      ./executable_brainfuck

Compiles the program for x86_64 (amd64) Linux following System V ABI.

## JIT

    ./Brainfuck -j program.bf

Just-In-Time compilation and direct execution for x86_64 (amd64) Linux following System V ABI

## Debugger

    ./Brainfuck -d program.bf

Debug the program using the following commands:

    s<x>:     executes x brainfuck instructions (if x is not specified then x = 1)
    g<x>:     executes instructions until instruction pointer = x (if x is negative, then until x instructions from end)
    r:        run until end of the program
    d<x>-<y>: dump memory content between address x and y (y is optional to print the content of one cell only)


## Example debugging:

Brainfuck program to execute ```2+3``` (Non-Brainfuck characters are comments)

    +++    sets first operand to 3
    >++<   sets second operand to 2
    [->+<] add first op (3) to second (2) (yield 5)

Sample debugging trace

    ./Brainfuck -d ../samples/add.bf
    Brainfuck Debugger
    'exit' to exit

    IP = 0	DP = 0
    IP:	  v
    L1:	  +++    sets first operand to 3
    DP:   v
    MEM: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    
    > s2
    IP = 2	DP = 0
    IP:	  v                        
    L1:	+++ sets first operand to 3
    DP: 	 v
    MEM:	[2, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]

    > g11
    IP = 11	DP = 1
    IP:	    v                                          
    L3:	[->+<] add first op (3) to second (2) (yield 5)
    DP: 	    v
    MEM:	[2, 3, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    
    
## Benchmarks

Using samples/mandelbrot.bf program which prints the mandelbrot set

| Method        | Time      |
| ------------- |----------:|
| Interpreter   | 343.89s   |
| Compiler      | 3.62s     |
| JIT           | 3.65s     |
| JIT Optimised | 1.79s     |



