/* macro_table.h - stores every macro definition found while assembling one source file */

#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "constants.h"

/* one line of text inside a macro's body, and a pointer to the next line */
typedef struct MacroLine
{
    char text[MAX_LINE_LENGTH + 1];
    struct MacroLine *next;
} MacroLine;

/* one macro definition: its name, the lines in its body, and a pointer to the next macro */
typedef struct Macro
{
    /* sized for a whole line, not MAX_LABEL_LENGTH, since macro names don't follow label syntax rules */
    char name[MAX_LINE_LENGTH + 1];
    MacroLine *body_head;
    MacroLine *body_tail;
    struct Macro *next;
} Macro;

/* adds a new macro with an empty body, returns TRUE if added, or FALSE if the name already exists */
int add_macro(const char *name);

/* appends one line of text to the end of a macro's body, returns FALSE if the macro was not found */
int add_macro_line(const char *macro_name, const char *line);

/* looks up a macro by name, returns a pointer to it, or NULL if not found */
Macro *find_macro(const char *name);

/* frees every macro and every line in the table, and empties the table for the next file */
void free_macro_table(void);

#endif /* MACRO_TABLE_H */
