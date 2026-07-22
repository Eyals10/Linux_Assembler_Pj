/* macro_table.c - implementation of the macro table, see macro_table.h for details */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "macro_table.h"

/* the first macro in the list, or NULL if the table is currently empty */
static Macro *macro_table_head = NULL;

/* adds a new macro with an empty body, returns TRUE if added, or FALSE if the name already exists */
int add_macro(const char *name)
{
    Macro *new_macro;

    if (find_macro(name) != NULL)
    {
        return FALSE;
    }

    new_macro = (Macro *) malloc(sizeof(Macro));
    if (new_macro == NULL)
    {
        printf("fatal error: out of memory\n");
        exit(1);
    }

    /* name is assumed to already be checked to fit in MAX_LINE_LENGTH by the caller */
    strcpy(new_macro->name, name);
    new_macro->body_head = NULL;
    new_macro->body_tail = NULL;
    new_macro->next = macro_table_head;

    macro_table_head = new_macro;

    return TRUE;
}

/* looks up a macro by name, returns a pointer to it, or NULL if not found */
Macro *find_macro(const char *name)
{
    Macro *current;

    current = macro_table_head;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/* appends one line of text to the end of a macro's body, returns FALSE if the macro was not found */
int add_macro_line(const char *macro_name, const char *line)
{
    Macro *macro;
    MacroLine *new_line;

    macro = find_macro(macro_name);
    if (macro == NULL)
    {
        return FALSE;
    }

    new_line = (MacroLine *) malloc(sizeof(MacroLine));
    if (new_line == NULL)
    {
        printf("fatal error: out of memory\n");
        exit(1);
    }

    /* line is assumed to already be checked to fit in MAX_LINE_LENGTH by the caller */
    strcpy(new_line->text, line);
    new_line->next = NULL;

    if (macro->body_tail == NULL)
    {
        macro->body_head = new_line;
        macro->body_tail = new_line;
    }
    else
    {
        macro->body_tail->next = new_line;
        macro->body_tail = new_line;
    }

    return TRUE;
}

/* frees every macro and every line in the table, and empties the table for the next file */
void free_macro_table(void)
{
    Macro *current_macro;
    Macro *next_macro;
    MacroLine *current_line;
    MacroLine *next_line;

    current_macro = macro_table_head;

    while (current_macro != NULL)
    {
        next_macro = current_macro->next;

        current_line = current_macro->body_head;
        while (current_line != NULL)
        {
            next_line = current_line->next;
            free(current_line);
            current_line = next_line;
        }

        free(current_macro);
        current_macro = next_macro;
    }

    macro_table_head = NULL;
}
