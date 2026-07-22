/* line_parser.h - functions for classifying a source line and pulling apart its pieces */

#ifndef LINE_PARSER_H
#define LINE_PARSER_H

#include "constants.h"

/* the four kinds of statements that can appear in a source line */
typedef enum
{
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_DIRECTIVE,
    LINE_INSTRUCTION
} LineType;

/* looks at a line and decides which of the four statement types it is */
LineType classify_line(const char *line);

/* checks for a label at the start of line, copies its raw text into out_label, and points out_rest past it, returns TRUE if a label was found */
/* out_label is not validated here, so it must be sized MAX_LINE_LENGTH + 1 to safely hold even an illegally long candidate */
int extract_label(const char *line, char *out_label, const char **out_rest);

/* returns TRUE if name is a legal label: starts with a letter, then letters/digits only, at most MAX_LABEL_LENGTH characters */
int is_valid_label_name(const char *name);

/* returns TRUE if text is a legal register operand ("$" followed by digits 0-31), and fills *out_reg_num with the register number */
int is_valid_register(const char *text, int *out_reg_num);

/* returns TRUE if text is a legal quoted string of printable characters, and copies its contents (without the quotes) into out_string */
/* out_string must be sized MAX_LINE_LENGTH + 1 to safely hold the copied text */
int is_valid_string_literal(const char *text, char *out_string);

/* splits text into trimmed, comma-separated operands, fills operands and *out_count, returns FALSE if the commas are placed illegally */
int split_operands(const char *text, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int *out_count);

/* copies the first whitespace-separated word from text into out_word */
/* assumes out_word is big enough, true whenever text is at most MAX_LINE_LENGTH characters */
void get_first_word(const char *text, char *out_word);

/* returns a pointer to whatever is left in text after its first word and any whitespace after it */
const char *skip_first_word(const char *text);

#endif /* LINE_PARSER_H */
