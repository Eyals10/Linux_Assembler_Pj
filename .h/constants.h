/* constants.h - shared limits and fixed values used across the whole assembler. */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define TRUE 1
#define FALSE 0

/* a source line is at most 80 characters, not counting the '\n' */
#define MAX_LINE_LENGTH 80

/* a label is at most 31 characters long */
#define MAX_LABEL_LENGTH 31

/* the most operands or data values that could realistically fit on one 80-character line */
#define MAX_OPERANDS 40

/* registers are numbered $0 through $31 */
#define MAX_REGISTER_NUMBER 31

/* the first instruction is placed at address 100 in memory */
#define IC_START 100

/* the data counter starts counting from 0 */
#define DC_START 0

/* every machine instruction is encoded into exactly 4 bytes */
#define BYTES_PER_INSTRUCTION 4

/* the .ob file lists this many bytes per row */
#define BYTES_PER_OUTPUT_ROW 4

/* generous upper bound for an input file name, unrelated to MAX_LINE_LENGTH (which is about source lines) */
#define MAX_FILENAME_LENGTH 250

/* file name extensions used by the assembler */
#define SOURCE_EXTENSION ".as"
#define EXPANDED_EXTENSION ".am"
#define OBJECT_EXTENSION ".ob"
#define ENTRIES_EXTENSION ".ent"
#define EXTERNALS_EXTENSION ".ext"

/* maximum sizes assumed for the code and data images (in bytes) */
#define MAX_CODE_IMAGE_SIZE 4096
#define MAX_DATA_IMAGE_SIZE 4096

#endif /* CONSTANTS_H */
