# Linux Assembler — Maman 14

A two-pass assembler, written in ANSI C89/C90, for a custom MIPS-like assembly language. Final project ("Maman 14") for the Open University of Israel course **20465 — Laboratory in Systems Programming**.

The assembler reads one or more `.as` source files and, for each one, expands its macros, resolves every label and address across two passes, and produces the machine-code and metadata files described below.

## Contents

- [How it works](#how-it-works)
- [Project structure](#project-structure)
- [Building](#building)
- [Usage](#usage)
- [Output files](#output-files)
- [The assembly language](#the-assembly-language)
- [Test files](#test-files)

## How it works

Labels can be referenced before they're defined, so the source can't be translated correctly in a single top-to-bottom read. The assembler solves this with a pipeline of four stages, run in order for every input file:

1. **Pre-assembler** — expands every macro call, writes the result to a `.am` file.
2. **First pass** — reads the expanded file, builds the symbol table, and encodes every instruction as completely as a single read allows.
3. **Second pass** — re-reads the expanded file and resolves everything the first pass had to leave blank (branch distances, jump/label addresses), now that every label's address is known.
4. **Output writer** — once both passes succeed with zero errors, writes the final `.ob` / `.ent` / `.ext` files.

If any stage reports an error, that file's pipeline stops immediately (errors print with their line number) and the assembler moves on to the next input file — no partial output is ever produced for a file that failed.

## Project structure

```
.
├── .c/             all .c files (one module per pipeline stage/data structure)
├── .h/              matching .h files
├── test-files/       example .as inputs, plus their generated outputs
└── Makefile
```

Each module is a `.h`/`.c` pair with one clear responsibility — e.g. `symbol_table` only manages the label table, `memory_image` only manages the simulated machine memory, `first_pass`/`second_pass` only implement their half of the two-pass algorithm.

## Building

Requires `gcc`. Compiles cleanly with no warnings under:

```bash
make
```

which is equivalent to `gcc -ansi -pedantic -Wall` on every source file. To rebuild from scratch:

```bash
make clean && make
```

## Usage

Every command-line argument is treated as an input `.as` file:

```bash
./assembler test-files/valid1.as test-files/valid2.as
```

Multiple files are processed independently — one file's errors don't affect any other file in the same run.

## Output files

For each input file that assembles successfully, up to four files are produced (same base name as the input, different extension):

| File | Contents |
|---|---|
| `.am` | The source after macro expansion |
| `.ob` | The machine code: a header line (code length, data length), then hex-encoded memory rows |
| `.ent` | One line per label declared `.entry` and actually defined in this file — only created if at least one exists |
| `.ext` | One line per instruction that references a label declared `.extern` — only created if at least one exists |

## The assembly language

A MIPS-like instruction set with three encoding formats:

- **R-type** — register-only arithmetic/logic (`add`, `sub`, `and`, `or`, `nor`) and register copies (`move`, `mvhi`, `mvlo`)
- **I-type** — one immediate/offset operand: arithmetic (`addi`, `subi`, `andi`, `ori`, `nori`), conditional branches (`beq`, `bne`, `blt`, `bgt`), and memory access (`lb`, `sb`, `lw`, `sw`, `lh`, `sh`)
- **J-type** — jumps and calls (`jmp`, `la`, `call`, `hlt`)

Plus directives for data (`.db`, `.dw`, `.dh`, `.asciz`) and cross-file linkage (`.entry`, `.extern`), and a macro system (`mcro` / `mcroend`). The full language specification is defined by the course, not by this repository.

## Test files

`test-files/` contains both kinds of example the assignment asks for:

- **`valid1.as`, `valid2.as`** — assemble successfully with zero errors; `valid2.as` also exercises macros, `.entry`, and `.extern`, so it produces all four output files.
- **`file1.as`–`file4.as`** — a range of deliberately invalid inputs (malformed macros, syntax errors, undefined symbols), used to verify the assembler's error detection and reporting.
