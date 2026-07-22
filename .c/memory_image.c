/* memory_image.c - implementation of the memory image module, see memory_image.h for details */

#include "constants.h"
#include "num_utils.h"
#include "memory_image.h"

/* the code image and data image byte arrays */
static int code_image[MAX_CODE_IMAGE_SIZE];
static int data_image[MAX_DATA_IMAGE_SIZE];

/* the instruction counter and data counter */
static int ic;
static int dc;

/* resets IC to IC_START, DC to DC_START, and clears both images, ready for a new file */
void init_memory_image(void)
{
    int index;

    ic = IC_START;
    dc = DC_START;

    for (index = 0; index < MAX_CODE_IMAGE_SIZE; index++)
    {
        code_image[index] = 0;
    }

    for (index = 0; index < MAX_DATA_IMAGE_SIZE; index++)
    {
        data_image[index] = 0;
    }
}

/* returns the current value of the instruction counter */
int get_ic(void)
{
    return ic;
}

/* returns the current value of the data counter */
int get_dc(void)
{
    return dc;
}

/* stores one byte at the current DC in the data image, and advances DC by 1 */
void add_data_byte(int byte_value)
{
    data_image[dc] = byte_value;
    dc++;
}

/* stores a 32-bit word (as 4 little-endian bytes) at the current IC in the code image, and advances IC by 4 */
void add_instruction_word(int word)
{
    int index;

    for (index = 0; index < BYTES_PER_INSTRUCTION; index++)
    {
        code_image[ic - IC_START + index] = get_byte(word, index);
    }

    ic += BYTES_PER_INSTRUCTION;
}

/* overwrites the 4 bytes already stored at the given code address with a new 32-bit word */
void set_instruction_word(int address, int word)
{
    int index;

    for (index = 0; index < BYTES_PER_INSTRUCTION; index++)
    {
        code_image[address - IC_START + index] = get_byte(word, index);
    }
}

/* builds a 32-bit R-type instruction word from its opcode, registers, and funct code */
int encode_r_type(int opcode, int rs, int rt, int rd, int funct)
{
    /* word is unsigned so shifting a value into bit 31 (opcodes can be as high as 63) stays well defined */
    unsigned int word;

    word = 0;
    word |= ((unsigned int) to_bits(opcode, 6)) << 26;
    word |= ((unsigned int) to_bits(rs, 5)) << 21;
    word |= ((unsigned int) to_bits(rt, 5)) << 16;
    word |= ((unsigned int) to_bits(rd, 5)) << 11;
    word |= ((unsigned int) to_bits(funct, 5)) << 6;
    /* bits 0-5 are left as 0, this instruction format doesn't use them */

    return (int) word;
}

/* builds a 32-bit I-type instruction word from its opcode, registers, and immediate value */
int encode_i_type(int opcode, int rs, int rt, int immed)
{
    /* unsigned so shifting into bit 31 stays well defined, same reason as encode_r_type */
    unsigned int word;

    word = 0;
    word |= ((unsigned int) to_bits(opcode, 6)) << 26;
    word |= ((unsigned int) to_bits(rs, 5)) << 21;
    word |= ((unsigned int) to_bits(rt, 5)) << 16;
    word |= (unsigned int) to_bits(immed, 16);

    return (int) word;
}

/* builds a 32-bit J-type instruction word from its opcode, register-or-label bit, and address */
int encode_j_type(int opcode, int reg_bit, int address)
{
    /* unsigned so shifting into bit 31 stays well defined, same reason as encode_r_type */
    unsigned int word;

    word = 0;
    word |= ((unsigned int) to_bits(opcode, 6)) << 26;
    word |= ((unsigned int) to_bits(reg_bit, 1)) << 25;
    word |= (unsigned int) to_bits(address, 25);

    return (int) word;
}

/* returns the byte at the given index in the code image */
int get_code_byte(int index)
{
    return code_image[index];
}

/* returns the byte at the given index in the data image */
int get_data_byte(int index)
{
    return data_image[index];
}
