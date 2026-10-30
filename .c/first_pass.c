/* first_pass.c - implementation of the first pass, see first_pass.h for details */

#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "errors.h"
#include "opcode_table.h"
#include "symbol_table.h"
#include "num_utils.h"
#include "line_parser.h"
#include "memory_image.h"
#include "first_pass.h"

/* add,sub,and,or,nor: 3 registers, in the order rs, rt, rd */
static void encode_r_arithmetic(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int line_number)
{
    int rs;
    int rt;
    int rd;
    int word;

    if (!is_valid_register(operands[0], &rs) || !is_valid_register(operands[1], &rt) || !is_valid_register(operands[2], &rd))
    {
        report_error(line_number, "operand is not a legal register");
        return;
    }

    word = encode_r_type(instr->opcode, rs, rt, rd, instr->funct);
    add_instruction_word(word);
}

/* move,mvhi,mvlo: 2 registers, but the fields are inverted compared to encode_r_arithmetic */
/* this matches the assignment's own example: "move $23,$2" puts 23 in rs and 2 in rd, rt is always 0 */
static void encode_r_copy(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int line_number)
{
    int dest;
    int src;
    int word;

    if (!is_valid_register(operands[0], &dest) || !is_valid_register(operands[1], &src))
    {
        report_error(line_number, "operand is not a legal register");
        return;
    }

    word = encode_r_type(instr->opcode, dest, 0, src, instr->funct);
    add_instruction_word(word);
}

/* addi,subi,andi,ori,nori,lb,sb,lw,sw,lh,sh: register, 16-bit number, register, fully resolved now */
static void encode_i_immediate(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int line_number)
{
    int rs;
    int rt;
    int immed;
    int word;

    if (!is_valid_register(operands[0], &rs))
    {
        report_error(line_number, "operand is not a legal register");
        return;
    }

    if (!parse_integer(operands[1], &immed) || !fits_in_bits(immed, 16))
    {
        report_error(line_number, "operand is not a legal 16-bit number");
        return;
    }

    if (!is_valid_register(operands[2], &rt))
    {
        report_error(line_number, "operand is not a legal register");
        return;
    }

    word = encode_i_type(instr->opcode, rs, rt, immed);
    add_instruction_word(word);
}

/* beq,bne,blt,bgt: 2 registers and a label, the distance to the label is always filled in during the second pass */
static void encode_i_branch(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int line_number)
{
    int rs;
    int rt;
    int word;

    if (!is_valid_register(operands[0], &rs) || !is_valid_register(operands[1], &rt))
    {
        report_error(line_number, "operand is not a legal register");
        return;
    }

    if (!is_valid_label_name(operands[2]))
    {
        report_error(line_number, "operand is not a legal label");
        return;
    }

    word = encode_i_type(instr->opcode, rs, rt, 0);
    add_instruction_word(word);
}

/* jmp: either a register (resolved now) or a label (filled in during the second pass) */
static void encode_j_jmp(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int line_number)
{
    int reg_num;
    int word;

    if (operands[0][0] == '$')
    {
        if (!is_valid_register(operands[0], &reg_num))
        {
            report_error(line_number, "operand is not a legal register");
            return;
        }

        word = encode_j_type(instr->opcode, 1, reg_num);
        add_instruction_word(word);
        return;
    }

    if (!is_valid_label_name(operands[0]))
    {
        report_error(line_number, "operand is not a legal label or register");
        return;
    }

    word = encode_j_type(instr->opcode, 0, 0);
    add_instruction_word(word);
}

/* la,call: always a label, the address is always filled in during the second pass */
static void encode_j_label(const Instruction *instr, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int line_number)
{
    int word;

    if (!is_valid_label_name(operands[0]))
    {
        report_error(line_number, "operand is not a legal label");
        return;
    }

    word = encode_j_type(instr->opcode, 0, 0);
    add_instruction_word(word);
}

/* hlt: no operands at all */
static void encode_j_hlt(const Instruction *instr)
{
    int word;

    word = encode_j_type(instr->opcode, 0, 0);
    add_instruction_word(word);
}

/* handles a .db, .dw, or .dh directive: stores each value's bytes, and records the label if there is one */
static void process_number_directive(const char *directive_name, int has_label, const char *label, const char *operand_text, int line_number)
{
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1];
    int operand_count;
    int width;
    int value;
    int label_value;
    int i;
    int byte_index;

    if (strcmp(directive_name, ".db") == 0)
    {
        width = 8;
    }
    else if (strcmp(directive_name, ".dh") == 0)
    {
        width = 16;
    }
    else
    {
        width = 32;
    }

    if (!split_operands(operand_text, operands, &operand_count))
    {
        report_error(line_number, "commas are placed incorrectly in the value list");
        return;
    }

    if (operand_count == 0)
    {
        report_error(line_number, "at least one value is required");
        return;
    }

    label_value = get_dc();

    for (i = 0; i < operand_count; i++)
    {
        if (!parse_integer(operands[i], &value))
        {
            report_error(line_number, "value is not a legal integer");
            continue;
        }

        if (!fits_in_bits(value, width))
        {
            report_error(line_number, "value is out of range for this directive");
            continue;
        }

        value = to_bits(value, width);
        for (byte_index = 0; byte_index < width / 8; byte_index++)
        {
            add_data_byte(get_byte(value, byte_index));
        }
    }

    if (has_label)
    {
        if (!add_symbol(label, label_value, SYMBOL_DATA))
        {
            report_error(line_number, "label is already defined");
        }
    }
}

/* handles a .asciz directive: stores each character plus a closing zero byte, and records the label if there is one */
static void process_asciz_directive(int has_label, const char *label, const char *operand_text, int line_number)
{
    char string_content[MAX_LINE_LENGTH + 1];
    int label_value;
    int i;

    if (!is_valid_string_literal(operand_text, string_content))
    {
        report_error(line_number, "not a legal quoted string");
        return;
    }

    label_value = get_dc();

    for (i = 0; string_content[i] != '\0'; i++)
    {
        add_data_byte(string_content[i]);
    }
    add_data_byte(0);

    if (has_label)
    {
        if (!add_symbol(label, label_value, SYMBOL_DATA))
        {
            report_error(line_number, "label is already defined");
        }
    }
}

/* handles a .extern directive: declares a symbol that is defined in another file */
static void process_extern_directive(const char *operand_text, int line_number)
{
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1];
    int operand_count;
    const Symbol *existing;

    if (!split_operands(operand_text, operands, &operand_count) || operand_count != 1)
    {
        report_error(line_number, ".extern takes exactly one label");
        return;
    }

    if (!is_valid_label_name(operands[0]))
    {
        report_error(line_number, "not a legal label name");
        return;
    }

    if (is_reserved_word(operands[0]))
    {
        report_error(line_number, "label name cannot be an instruction or directive name");
        return;
    }

    existing = find_symbol(operands[0]);
    if (existing == NULL)
    {
        add_symbol(operands[0], 0, SYMBOL_EXTERNAL);
    }
    else if (existing->kind != SYMBOL_EXTERNAL)
    {
        report_error(line_number, "symbol is already defined, cannot be extern");
    }
}

/* handles any directive statement: .db, .dw, .dh, .asciz, .entry, or .extern */
static void process_directive_line(int has_label, const char *label, const char *rest, int line_number)
{
    char directive_name[MAX_LINE_LENGTH + 1];
    const char *operand_text;

    get_first_word(rest, directive_name);
    operand_text = skip_first_word(rest);

    if (strcmp(directive_name, ".db") == 0 || strcmp(directive_name, ".dw") == 0 || strcmp(directive_name, ".dh") == 0)
    {
        process_number_directive(directive_name, has_label, label, operand_text, line_number);
        return;
    }

    if (strcmp(directive_name, ".asciz") == 0)
    {
        process_asciz_directive(has_label, label, operand_text, line_number);
        return;
    }

    if (strcmp(directive_name, ".entry") == 0)
    {
        /* entry is fully handled in the second pass */
        return;
    }

    if (strcmp(directive_name, ".extern") == 0)
    {
        process_extern_directive(operand_text, line_number);
        return;
    }

    report_error(line_number, "unknown directive");
}

/* handles any instruction statement, dispatching to the right encoder for its operand pattern */
static void process_instruction_line(int has_label, const char *label, const char *rest, int line_number)
{
    char op_name[MAX_LINE_LENGTH + 1];
    const char *operand_text;
    const Instruction *instr;
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1];
    int operand_count;

    if (has_label)
    {
        if (!add_symbol(label, get_ic(), SYMBOL_CODE))
        {
            report_error(line_number, "label is already defined");
        }
    }

    get_first_word(rest, op_name);
    operand_text = skip_first_word(rest);

    instr = find_opcode(op_name);
    if (instr == NULL)
    {
        report_error(line_number, "unknown operation name");
        return;
    }

    if (!split_operands(operand_text, operands, &operand_count))
    {
        report_error(line_number, "commas are placed incorrectly in the operand list");
        return;
    }

    if (operand_count != instr->operand_count)
    {
        report_error(line_number, "wrong number of operands for this operation");
        return;
    }

    if (instr->type == R_TYPE)
    {
        /* move, mvhi, mvlo take 2 registers; add, sub, and, or, nor take 3 */
        if (instr->operand_count == 2)
        {
            encode_r_copy(instr, operands, line_number);
        }
        else
        {
            encode_r_arithmetic(instr, operands, line_number);
        }
        return;
    }

    if (instr->type == I_TYPE)
    {
        if (strcmp(instr->name, "beq") == 0 || strcmp(instr->name, "bne") == 0 ||
            strcmp(instr->name, "blt") == 0 || strcmp(instr->name, "bgt") == 0)
        {
            encode_i_branch(instr, operands, line_number);
        }
        else
        {
            encode_i_immediate(instr, operands, line_number);
        }
        return;
    }

    if (strcmp(instr->name, "jmp") == 0)
    {
        encode_j_jmp(instr, operands, line_number);
    }
    else if (strcmp(instr->name, "hlt") == 0)
    {
        encode_j_hlt(instr);
    }
    else
    {
        encode_j_label(instr, operands, line_number);
    }
}

/* runs the first pass over filename (the expanded .am file), building the symbol table and memory image */
int run_first_pass(const char *filename)
{
    FILE *file;
    char line[MAX_LINE_LENGTH + 2];
    char label[MAX_LINE_LENGTH + 1];
    const char *rest;
    int line_number;
    int has_label;
    int length;
    LineType type;
    int icf;

    free_symbol_table();
    init_memory_image();

    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("cannot open file %s\n", filename);
        return FALSE;
    }

    line_number = 0;

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

        /* stop before this line could write past the end of an image */
        /* one data line stores at most MAX_OPERANDS 4-byte values, so that much room must be free */
        if ((type == LINE_INSTRUCTION && get_ic() - IC_START + BYTES_PER_INSTRUCTION > MAX_CODE_IMAGE_SIZE) ||
            (type == LINE_DIRECTIVE && get_dc() + MAX_OPERANDS * 4 > MAX_DATA_IMAGE_SIZE))
        {
            report_error(line_number, "program is too large for the memory image");
            break;
        }

        has_label = extract_label(line, label, &rest);
        if (has_label && !is_valid_label_name(label))
        {
            report_error(line_number, "not a legal label name");
            has_label = FALSE;
        }
        else if (has_label && is_reserved_word(label))
        {
            report_error(line_number, "label name cannot be an instruction or directive name");
            has_label = FALSE;
        }

        if (type == LINE_DIRECTIVE)
        {
            process_directive_line(has_label, label, rest, line_number);
        }
        else
        {
            process_instruction_line(has_label, label, rest, line_number);
        }
    }

    fclose(file);

    icf = get_ic();
    update_data_symbols(icf);

    if (had_errors())
    {
        return FALSE;
    }

    return TRUE;
}
