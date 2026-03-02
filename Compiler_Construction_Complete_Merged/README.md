# Compiler Construction Stage 1 (Lexer + Parser)

This repository contains a Stage 1 compiler front-end for the course workflow:
- Lexical analysis (twin buffer based scanning, token generation, lexical errors)
- LL(1) parsing (FIRST/FOLLOW, parse table, parse tree construction)
- Driver menu for comment removal, token list printing, parsing, and timing

The executable expected by evaluation is `stage1exe`.

## 1. Current Implementation Status

At startup, the driver prints:

`(c) Both lexical and syntax analysis modules implemented`

This matches the implemented status in `driver.c`.

## 2. Repository Layout

Main source files and responsibilities:

- `driver.c`
  - Interactive menu loop (options 0-4).
  - Invokes lexer-only flows and lexer+parser flows.
  - Prints timing for parse verification.
- `lexer.c`
  - DFA-based tokenization logic.
  - Twin-buffer input management.
  - Keyword trie setup.
  - `getStream`, `nextToken`, `getNextToken`, `removeComments`.
- `lexerDef.h`
  - Lexer data definitions (`Trie`, `trie`), separate from lexer function prototypes.
- `lexer.h`
  - Lexer function declarations.
- `parser.c`
  - Parser-facing API wrappers.
  - Calls grammar initialization + FIRST/FOLLOW + parse table + stream parser.
- `parserDef.h`
  - Data definitions for grammar, FIRST/FOLLOW, parse table, parse tree.
- `parser.h`
  - Parser function declarations.
- `grammar_sets.c`
  - Hardcoded LL(1) grammar.
  - FIRST/FOLLOW helper logic and non-terminal name mapper.
- `grammar_sets.h`
  - Grammar/FIRST/FOLLOW helper declarations.
- `parse_runtime.c`
  - Parse table builder.
  - FIRST/FOLLOW computation orchestration.
  - Predictive parsing engine (stack + parse tree stack).
  - Parse tree inorder writer.
- `parse_runtime.h`
  - Runtime parser declarations.
- `token_model.h`
  - Token enum, DFA state enum, twin buffer structure, constants.
- `grammar_model.h`
  - Wrapper include for parser data model (`#include "parserDef.h"`).
- `ascii_text.c`, `ascii_text.h`
  - Local string comparison helper used by lexer (`stringcmp`).
- `grammar_g13.txt`, `grammar_g43.txt`
  - Grammar text references aligned with implemented LL(1) updates.
- `makefile`
  - Build rules for `stage1exe`.
- `testcase1.txt` ... `testcase9.txt`
  - Input examples for lexer/parser testing.

## 3. Interface and Include Organization

The project follows the required separation style:

- Data definitions are kept in `lexerDef.h` and `parserDef.h`.
- Function prototypes are in `lexer.h`, `parser.h`, `parse_runtime.h`, `grammar_sets.h`.
- `parser.c` includes `lexer.h` because parser uses lexer interface.
- `lexer.c` does not include `lexer.h`; it includes `lexerDef.h`.

## 4. Build Requirements

Language and libraries:
- C11 (`-std=c11`)
- Standard C libraries only (`stdio.h`, `stdlib.h`, `time.h`, `stdbool.h`, etc.)

No non-C high level runtime/library dependency is required by the source code.

## 5. Building the Project

Default build:

```bash
make
```

Clean build artifacts:

```bash
make clean
```

The generated binary is:

`./stage1exe`

## 6. Building with GCC 13.3.0

If `gcc-13.3.0` is available on your machine:

```bash
make clean
make CC=gcc-13
```

On this setup, GCC 13.3.0 was installed in a Conda environment. Use:

```bash
make clean
make CC=/home/saksham/.conda/envs/gcc133/bin/x86_64-conda-linux-gnu-gcc
```

Version check command:

```bash
/home/saksham/.conda/envs/gcc133/bin/x86_64-conda-linux-gnu-gcc --version
```

## 7. Execution

Run format:

```bash
./stage1exe testcase.txt parsetreeOutFile.txt
```

Example:

```bash
./stage1exe testcase3.txt out_tree.txt
```

## 8. Driver Menu Behavior

The driver loops until option `0` is selected.

### Option 0 - Exit

- Terminates the driver.

### Option 1 - Remove comments, print comment-free code

- Calls `removeComments(inputFile, cleanFile_tmp.txt)`.
- Prints cleaned code to console.
- Deletes the temp file after printing.
- Comment syntax handled: `%` to end of line.

### Option 2 - Print lexer token list

- Invokes lexer only (`getStream(fp)`).
- Prints formatted token table:
  - `LineNo`
  - `Lexeme`
  - `Token`
- Prints total lexical error count afterward.

### Option 3 - Parse source and print parse tree

- Invokes lexer + parser through `parseInputSourceCode`.
- Prints lexical and syntactic errors on console.
- On success prints:
  - `Input source code is syntactically correct...........`
- Writes parse tree to output file via `printParseTree`.

### Option 4 - Print total CPU time for syntax verification

- Measures time around parse invocation using `clock()`.
- Prints:
  - `total_CPU_time`
  - `total_CPU_time_in_seconds`

## 9. Lexer Design Summary

Core design points:

- Twin buffer:
  - `BUFFER_SIZE = 50`
  - Effective storage: `2 * BUFFER_SIZE`
- DFA token recognition via `getNextState`.
- Keyword lookup using trie (`initializeLookupTable` + `search`).
- `_main` is specially mapped to `TK_MAIN`.
- Comments:
  - `% ... end of line`
  - Ignored in parser token flow (`nextToken` skips comments).

Lexical error reporting currently includes:

- Unknown symbol
- Unknown pattern
- Variable identifier length > 20
- Function identifier length > 30

## 10. Parser Design Summary

Pipeline used in `parseInputSourceCode`:

1. Initialize grammar (`initializeGrammar`).
2. Compute FIRST/FOLLOW (`ComputeFirstAndFollowSets` -> `computeFirstandFollowSets`).
3. Build parse table (`buildParseTable`).
4. Parse stream (`parseInputSourceCodeStream`).

Predictive parser characteristics:

- Stack-based LL(1) parser.
- Parse table entries:
  - `-1`: invalid transition (consume token with error print)
  - `-2`: sync-like pop behavior for recovery path
- Root non-terminal: `NT_PROGRAM`.
- End marker token: `DOLLAR`.

## 11. Grammar Source of Truth

Important:

- Runtime parser grammar is hardcoded in `grammar_sets.c` (`initializeGrammar`).
- `grammar_g13.txt` and `grammar_g43.txt` are aligned reference files.

Implemented grammar includes the major LL(1) updates such as:

- `constructedDatatype` supports `TK_RUID`.
- `typeDefinitions` supports `actualOrRedefined` with `definetypestmt`.
- `fieldDefinition` uses `<fieldType>`.
- Nested record expansion chain via:
  - `singleOrRecId -> TK_ID option_single_constructed`
  - `oneExpansion`, `moreExpansions`
- Function call statement ending with `TK_SEM`.
- Left-factored conditional with `elsePart`.
- Arithmetic precedence non-terminals:
  - `arithmeticExpression`, `expPrime`, `term`, `termPrime`, `factor`
- Boolean NOT as `TK_NOT TK_OP <booleanExpression> TK_CL`.

## 12. Parse Tree Output Format

`printParseTreeInorder` writes an inorder traversal for n-ary tree in this order:

- leftmost child
- parent
- remaining children left-to-right

Header currently written:

`lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol`

Column behavior:

- Non-leaf lexeme is printed as `----`.
- Line number for non-leaf is `----`.
- Numeric value column is populated for `TK_NUM`/`TK_RNUM`, else `----`.
- Parent is `ROOT` for root node.

## 13. FIRST/FOLLOW and Parse Table Data Structures

Defined in `parserDef.h`:

- `NON_TERMINAL_COUNT = 53`
- `MAX_RULE_SIZE = 15`
- `FirstAndFollow`:
  - `first[][]`, `first_count[]`, `first_has_epsillon[]`
  - `follow[][]`, `follow_count[]`, `follow_rule[]`
- `table`:
  - `table[NON_TERMINAL_COUNT][TOTAL_TOKENS]`

## 14. Practical Command Set

Build and run:

```bash
make clean
make
./stage1exe testcase1.txt out_tree.txt
```

Build and run with GCC 13.3.0 toolchain path:

```bash
make clean
make CC=/home/saksham/.conda/envs/gcc133/bin/x86_64-conda-linux-gnu-gcc
./stage1exe testcase1.txt out_tree_gcc133.txt
```

Automated menu input example:

```bash
printf '2\n0\n' | ./stage1exe testcase1.txt out_tree.txt
```

## 15. Known Notes and Limitations

- `make run` target executes `./stage1exe` without CLI args, so it will show usage and exit.
- Parse tree memory is not explicitly freed before process exit.
- Console output includes lexical errors from lexer during parser runs, as required for diagnostics.
- Non-terminal names in some outputs are internal enum-style names (`NT_*`).

## 16. Compliance Notes

This implementation is structured to match the stated stage requirements:

- Separate lexer and parser modules.
- Driver with options 0-4 and persistent while loop.
- Comment removal plugin supported.
- Lexer-only token list printing supported.
- Parser flow and parse tree printing supported.
- Time measurement option supported using `<time.h>`.
- Build via `makefile` (name is exactly `makefile`).

