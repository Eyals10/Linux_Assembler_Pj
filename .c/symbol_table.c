/* symbol_table.c - implementation of the symbol table, see symbol_table.h for details */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "symbol_table.h"

/* the first and last symbols in the list, or NULL if the table is currently empty */
static Symbol *symbol_table_head = NULL;
static Symbol *symbol_table_tail = NULL;

/* adds a new symbol, returns TRUE if added, or FALSE if the name already exists */
int add_symbol(const char *name, int value, SymbolKind kind)
{
    Symbol *new_symbol;

    if (find_symbol(name) != NULL)
    {
        return FALSE;
    }

    new_symbol = (Symbol *) malloc(sizeof(Symbol));
    if (new_symbol == NULL)
    {
        printf("fatal error: out of memory\n");
        exit(1);
    }

    /* name is assumed to already be checked to fit in MAX_LABEL_LENGTH by the caller */
    strcpy(new_symbol->name, name);
    new_symbol->value = value;
    new_symbol->kind = kind;
    new_symbol->is_entry = FALSE;
    new_symbol->next = NULL;

    /* append at the tail, so symbols come back out in the order they were defined */
    if (symbol_table_tail == NULL)
    {
        symbol_table_head = new_symbol;
        symbol_table_tail = new_symbol;
    }
    else
    {
        symbol_table_tail->next = new_symbol;
        symbol_table_tail = new_symbol;
    }

    return TRUE;
}

/* returns the first symbol in the table, for iterating over all of them via ->next, or NULL if the table is empty */
const Symbol *get_first_symbol(void)
{
    return symbol_table_head;
}

/* looks up a symbol by name, returns a pointer to it, or NULL if not found */
Symbol *find_symbol(const char *name)
{
    Symbol *current;

    current = symbol_table_head;

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

/* marks an existing symbol as an entry, returns FALSE if the symbol was not found */
int mark_entry(const char *name)
{
    Symbol *found;

    found = find_symbol(name);
    if (found == NULL)
    {
        return FALSE;
    }

    found->is_entry = TRUE;
    return TRUE;
}

/* adds icf to the value of every symbol whose kind is SYMBOL_DATA */
void update_data_symbols(int icf)
{
    Symbol *current;

    current = symbol_table_head;

    while (current != NULL)
    {
        if (current->kind == SYMBOL_DATA)
        {
            current->value += icf;
        }
        current = current->next;
    }
}

/* frees every symbol in the table, and empties the table for the next file */
void free_symbol_table(void)
{
    Symbol *current;
    Symbol *next_symbol;

    current = symbol_table_head;

    while (current != NULL)
    {
        next_symbol = current->next;
        free(current);
        current = next_symbol;
    }

    symbol_table_head = NULL;
    symbol_table_tail = NULL;
}
