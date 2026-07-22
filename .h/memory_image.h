/* memory_image.h - stores the code and data images, tracks IC/DC, and builds instruction words */

#ifndef MEMORY_IMAGE_H
#define MEMORY_IMAGE_H

/* resets IC to IC_START, DC to DC_START, and clears both images, ready for a new file */
void init_memory_image(void);

/* returns the current value of the instruction counter */
int get_ic(void);

/* returns the current value of the data counter */
int get_dc(void);

/* stores one byte at the current DC in the data image, and advances DC by 1 */
void add_data_byte(int byte_value);

/* stores a 32-bit word (as 4 little-endian bytes) at the current IC in the code image, and advances IC by 4 */
void add_instruction_word(int word);

/* overwrites the 4 bytes already stored at the given code address with a new 32-bit word */
void set_instruction_word(int address, int word);

/* builds a 32-bit R-type instruction word from its opcode, registers, and funct code */
int encode_r_type(int opcode, int rs, int rt, int rd, int funct);

/* builds a 32-bit I-type instruction word from its opcode, registers, and immediate value */
int encode_i_type(int opcode, int rs, int rt, int immed);

/* builds a 32-bit J-type instruction word from its opcode, register-or-label bit, and address */
int encode_j_type(int opcode, int reg_bit, int address);

/* returns the byte at the given index in the code image (index 0 is address IC_START) */
int get_code_byte(int index);

/* returns the byte at the given index in the data image (index 0 is address 0, before the ICF shift) */
int get_data_byte(int index);

#endif /* MEMORY_IMAGE_H */
