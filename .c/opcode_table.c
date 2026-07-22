/* opcode_table.c - implementation of the instruction table, see opcode_table.h for details */

#include <string.h>
#include "constants.h"
#include "opcode_table.h"

/* the full table of instructions supported by the assembly language */
static const Instruction instruction_table[] = {
    { "add",  0,  1, R_TYPE, 3 },
    { "sub",  0,  2, R_TYPE, 3 },
    { "and",  0,  3, R_TYPE, 3 },
    { "or",   0,  4, R_TYPE, 3 },
    { "nor",  0,  5, R_TYPE, 3 },
    { "move", 1,  1, R_TYPE, 2 },
    { "mvhi", 1,  2, R_TYPE, 2 },
    { "mvlo", 1,  3, R_TYPE, 2 },
    { "addi", 10, 0, I_TYPE, 3 },
    { "subi", 11, 0, I_TYPE, 3 },
    { "andi", 12, 0, I_TYPE, 3 },
    { "ori",  13, 0, I_TYPE, 3 },
    { "nori", 14, 0, I_TYPE, 3 },
    { "bne",  15, 0, I_TYPE, 3 },
    { "beq",  16, 0, I_TYPE, 3 },
    { "blt",  17, 0, I_TYPE, 3 },
    { "bgt",  18, 0, I_TYPE, 3 },
    { "lb",   19, 0, I_TYPE, 3 },
    { "sb",   20, 0, I_TYPE, 3 },
    { "lw",   21, 0, I_TYPE, 3 },
    { "sw",   22, 0, I_TYPE, 3 },
    { "lh",   23, 0, I_TYPE, 3 },
    { "sh",   24, 0, I_TYPE, 3 },
    { "jmp",  30, 0, J_TYPE, 1 },
    { "la",   31, 0, J_TYPE, 1 },
    { "call", 32, 0, J_TYPE, 1 },
    { "hlt",  63, 0, J_TYPE, 0 }
};

/* how many instructions are in the table above */
static const int instruction_count = sizeof(instruction_table) / sizeof(instruction_table[0]);

/* looks up an operation name in the table, returns a pointer to it, or NULL if not found */
const Instruction *find_opcode(const char *name)
{
    int i;

    for (i = 0; i < instruction_count; i++)
    {
        if (strcmp(instruction_table[i].name, name) == 0)
        {
            return &instruction_table[i];
        }
    }

    return NULL;
}

/* the directive names recognized by the assembler, without their leading '.' */
static const char *directive_names[] = { "db", "dw", "dh", "asciz", "entry", "extern" };

/* how many directive names are in the array above */
static const int directive_count = sizeof(directive_names) / sizeof(directive_names[0]);

/* macro-related keywords, which also cannot be used as labels, macro names, or extern names */
static const char *macro_keywords[] = { "mcro", "mcroend" };

/* how many macro keywords are in the array above */
static const int macro_keyword_count = sizeof(macro_keywords) / sizeof(macro_keywords[0]);

/* returns TRUE if name is an instruction name, a directive name, or a macro keyword, FALSE otherwise */
int is_reserved_word(const char *name)
{
    int i;

    if (find_opcode(name) != NULL)
    {
        return TRUE;
    }

    for (i = 0; i < directive_count; i++)
    {
        if (strcmp(directive_names[i], name) == 0)
        {
            return TRUE;
        }
    }

    for (i = 0; i < macro_keyword_count; i++)
    {
        if (strcmp(macro_keywords[i], name) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}
