/* opcode_table.h - the table of all instruction names and their machine codes */

#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

/* the three instruction encoding formats used by the virtual machine */
typedef enum
{
    R_TYPE,
    I_TYPE,
    J_TYPE
} InstructionType;

/* describes one instruction: its name, its machine codes, its format, and its operand count */
typedef struct
{
    const char *name;
    int opcode;
    int funct;
    InstructionType type;
    int operand_count;
} Instruction;

/* looks up an operation name in the table, returns a pointer to it, or NULL if not found */
const Instruction *find_opcode(const char *name);

/* returns TRUE if name is an instruction name or a directive name, FALSE otherwise */
int is_reserved_word(const char *name);

#endif /* OPCODE_TABLE_H */
