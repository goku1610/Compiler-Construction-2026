# Compiler Construction Project 2025

This project implements a compiler frontend, including a Lexical Analyzer and a Syntax Analyzer (Parser), for a custom programming language.

## Team Members - Group 20

- Anmol Agrawal (2022B5A71313P)
- Saksham Rohatgi (2022B2A70653P)
- Aditya Thole (2022B3A70374P)
- Nirmal Swaroop Raghunandan (2022B3A70512P)
- Ansh Nohria (2022B5A71614P)

## Project Overview

The project consists of two main stages:

1.  **Lexical Analysis:** Tokenizes the source code, handling comments and identifying invalid tokens.
2.  **Syntax Analysis:** Parses the token stream against the language grammar using a predictive parser, generating a parse tree.

## Prerequisites

- GCC Compiler
- Make

## Build Instructions

To build the project, use the provided `makefile`.

### Build Lexer and Parser (Stage 1)

```bash
make parser
```

This generates the executable `stage1exe`.

### Build Lexer Only

```bash
make lexer
```

This generates the executable `myprogram`.

## Usage

Run the compiled executable with the input source file and the desired output file path.

```bash
./stage1exe <input_file> <output_file>
```

### Menu Options

Upon running the program, you will be presented with the following options:

- **0:** Exit
- **1:** Remove comments (clean source code saved to output file)
- **2:** Print token list (Lexical Analysis)
- **3:** Parse code and print parse tree (Syntax Analysis)
- **4:** Measure execution time

## File Structure

- **driver.c:** Main entry point handling user interaction and invoking lexer/parser.
- **lexer.c / lexer.h:** Implementation of the lexical analyzer (DFA based).
- **parser.c / parser.h:** Implementation of the predictive parser.
- **grammar.txt:** text file containing the grammar rules of the language.
- **keyword_table.c:** Handles keyword lookups.
- **makefile:** Build configuration.

## Language Specifications

The language supports:

- **Data Types:** `int`, `real`, `record`, `union`
- **Control Structures:** `while`, `if-then-else`
- **I/O:** `read`, `write`
- **Functions:** Definition and calls with parameters.
