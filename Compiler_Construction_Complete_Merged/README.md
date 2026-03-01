# Compiler Construction Complete Merged

This repository merges the working logic from:

- `Compiler_Construction_Project_2025`
- `Compiler-Construction`

The codebase has been reorganized and renamed into a new structure while preserving compiler behavior.

## Layout

- `app/`
  - `driver_console.c` (CLI entry + menu flow)
- `lexer/`
  - `stream_lexer.c`
  - `keyword_bank.c`
- `parser/`
  - `ll1_frontend.c`
  - `first_follow.c`
- `shared/`
  - `ascii_text.c`
- `include/`
  - `core/token_model.h`
  - `core/grammar_model.h`
  - `lexer/stream_lexer.h`
  - `lexer/keyword_bank.h`
  - `parser/ll1_frontend.h`
  - `parser/first_follow.h`
  - `shared/ascii_text.h`
- `resources/`
  - `grammar_g13.txt`
  - `grammar_g43.txt`
- `tests/` (all testcase files from both source projects)
- `docs/` (reference PDFs/DOCX and original README copies)

## Build

```bash
make
```

## Run

```bash
./stage1exe [inputFilePath [outputFileName]]
```

All generated files are written inside `outputs/` in this project directory.
Example: if you pass `result.txt`, output is saved as `outputs/result.txt`.
If no command-line args are provided, the program lets you select a testcase
from `tests/` interactively.

## Menu

- `0`: Exit
- `1`: Remove comments and write clean code to output file
- `2`: Print lexer token list
- `3`: Parse code and write parse tree to output file
- `4`: Parse code and print execution time

## Parse Tree Visualization

When you run menu option `3`, the compiler now also writes:

- `outputs/<name>.dot` (Graphviz parse-tree graph)
- `outputs/<name>.svg` (auto-generated if `dot` is installed)
