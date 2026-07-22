/* second_pass.c - implementation of the second pass, see second_pass.h for details */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "errors.h"
#include "opcode_table.h"
#include "symbol_table.h"
#include "num_utils.h"
#include "line_parser.h"
#include "memory_image.h"
#include "second_pass.h"

/* the first and last recorded external symbol usages, or NULL if there are none yet */
static ExternalUsage *external_usage_head = NULL;
static ExternalUsage *external_usage_tail = NULL;

/* frees every recorded external symbol usage, and empties the list for the next file */
static void free_external_usage_list(void)
{
    ExternalUsage *current;
    ExternalUsage *next_usage;

    current = external_usage_head;

    while (current != NULL)
    {
        next_usage = current->next;
        free(current);
        current = next_usage;
    }

    external_usage_head = NULL;
    external_usage_tail = NULL;
}

/* records that the instruction at address used the external symbol symbol_name */
/* symbol_name is assumed to already be checked to fit in MAX_LABEL_LENGTH by the caller */
static void add_external_usage(const char *symbol_name, int address)
{
    ExternalUsage *new_usage;

    new_usage = (ExternalUsage *) malloc(sizeof(ExternalUsage));
    if (new_usage == NULL)
    {
        printf("fatal error: out of memory\n");
        exit(1);
    }

    strcpy(new_usage->symbol_name, symbol_name);
    new_usage->address = address;
    new_usage->next = NULL;

    /* append at the tail, so usages come back out in the order they were found */
    if (external_usage_tail == NULL)
    {
        external_usage_head = new_usage;
        external_usage_tail = new_usage;
    }
    else
    {
        external_usage_tail->next = new_usage;
        external_usage_tail = new_usage;
    }
}

/* completes a branch instruction (beq,bne,blt,bgt) by computing the distance to its label */
static void complete_branch_instruction(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int current_ic, int line_number)
{
    int rs;
    int rt;
    int distance;
    int word;
    const Symbol *target;

    if (!is_valid_register(operands[0], &rs) || !is_valid_register(operands[1], &rt))
    {
        /* this can't really happen: first_pass already checked these are legal registers */
        return;
    }

    target = find_symbol(operands[2]);
    if (target == NULL)
    {
        report_error(line_number, "undefined label");
        return;
    }

    if (target->kind == SYMBOL_EXTERNAL)
    {
        report_error(line_number, "a branch target cannot be an external symbol");
        return;
    }

    distance = target->value - current_ic;
    if (!fits_in_bits(distance, 16))
    {
        report_error(line_number, "branch distance is out of range");
        return;
    }

    word = encode_i_type(instr->opcode, rs, rt, distance);
    set_instruction_word(current_ic, word);
}

/* completes a jmp (label form), la, or call instruction by resolving its label's address */
static void complete_jump_instruction(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int current_ic, int line_number)
{
    int word;
    const Symbol *target;

    target = find_symbol(operands[0]);
    if (target == NULL)
    {
        report_error(line_number, "undefined label");
        return;
    }

    if (target->kind == SYMBOL_EXTERNAL)
    {
        /* the address field stays 0, as it already is from the first pass */
        add_external_usage(target->name, current_ic);
        return;
    }

    word = encode_j_type(instr->opcode, 0, target->value);
    set_instruction_word(current_ic, word);
}

/* completes the encoding of one instruction line, if it still needs it, at the given address */
static void process_instruction_line(const char *rest, int current_ic, int line_number)
{
    char op_name[MAX_LINE_LENGTH + 1];
    const char *operand_text;
    const Instruction *instr;
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1];
    int operand_count;

    get_first_word(rest, op_name);
    operand_text = skip_first_word(rest);

    instr = find_opcode(op_name);
    if (instr == NULL)
    {
        /* this can't really happen: second_pass only runs after first_pass found zero errors */
        /* we check again anyway so bad data fails safely, but we do not repeat the error message */
        return;
    }

    if (!split_operands(operand_text, operands, &operand_count))
    {
        /* same reasoning as above: already checked and reported by the first pass */
        return;
    }

    if (operand_count != instr->operand_count)
    {
        /* same reasoning as above: already checked and reported by the first pass */
        return;
    }

    if (instr->type == R_TYPE)
    {
        /* R-type instructions are always fully resolved in the first pass */
        return;
    }

    if (instr->type == I_TYPE)
    {
        if (strcmp(instr->name, "beq") == 0 || strcmp(instr->name, "bne") == 0 ||
            strcmp(instr->name, "blt") == 0 || strcmp(instr->name, "bgt") == 0)
        {
            complete_branch_instruction(instr, operands, current_ic, line_number);
        }
        /* the other I-type instructions (addi,subi,andi,ori,nori,lb,sb,lw,sw,lh,sh) */
        /* are always fully resolved in the first pass, so there is nothing left to do */
        return;
    }

    /* J_TYPE */
    if (strcmp(instr->name, "hlt") == 0)
    {
        /* no operands, already fully resolved in the first pass */
        return;
    }

    if (strcmp(instr->name, "jmp") == 0 && operands[0][0] == '$')
    {
        /* register form, already fully resolved in the first pass */
        return;
    }

    complete_jump_instruction(instr, operands, current_ic, line_number);
}

/* handles an .entry line: marks the operand symbol as an entry point */
static void process_entry_directive(const char *operand_text, int line_number)
{
    char entry_name[MAX_LINE_LENGTH + 1];
    const Symbol *target;

    get_first_word(operand_text, entry_name);

    target = find_symbol(entry_name);
    if (target == NULL)
    {
        report_error(line_number, "entry symbol was not defined in this file");
        return;
    }

    if (target->kind == SYMBOL_EXTERNAL)
    {
        report_error(line_number, "an external symbol cannot also be an entry");
        return;
    }

    mark_entry(entry_name);
}

/* runs the second pass over filename (the expanded .am file), completing every deferred instruction */
int run_second_pass(const char *filename)
{
    FILE *file;
    char line[MAX_LINE_LENGTH + 2];
    char label[MAX_LINE_LENGTH + 1];
    char directive_name[MAX_LINE_LENGTH + 1];
    const char *rest;
    const char *operand_text;
    int line_number;
    int length;
    int current_ic;
    LineType type;

    free_external_usage_list();

    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("cannot open file %s\n", filename);
        return FALSE;
    }

    line_number = 0;
    /* memory_image's IC is already stuck at its final value after the first pass finished */
    /* so we track our own address counter here instead, the same way the first pass did */
    current_ic = IC_START;

    while (fgets(line, MAX_LINE_LENGTH + 2, file) != NULL)
    {
        line_number++;

        /* strip the trailing newline, if there is one, so line always holds just the content */
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n')
        {
            line[length - 1] = '\0';
        }

        type = classify_line(line);
        if (type == LINE_EMPTY || type == LINE_COMMENT)
        {
            continue;
        }

        /* the label itself, if there is one, was already handled in the first pass */
        extract_label(line, label, &rest);

        if (type == LINE_DIRECTIVE)
        {
            get_first_word(rest, directive_name);

            if (strcmp(directive_name, ".entry") == 0)
            {
                operand_text = skip_first_word(rest);
                process_entry_directive(operand_text, line_number);
            }

            continue;
        }

        process_instruction_line(rest, current_ic, line_number);
        current_ic += BYTES_PER_INSTRUCTION;
    }

    fclose(file);

    if (had_errors())
    {
        return FALSE;
    }

    return TRUE;
}

/* returns the first recorded external symbol usage, for iterating over all of them, or NULL if there are none */
const ExternalUsage *get_first_external_usage(void)
{
    return external_usage_head;
}
