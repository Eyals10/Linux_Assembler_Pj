/* symbol_table.h - stores every label found while assembling one source file */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "constants.h"

/* what kind of address a symbol's value refers to */
typedef enum
{
    SYMBOL_CODE,
    SYMBOL_DATA,
    SYMBOL_EXTERNAL
} SymbolKind;

/* one label in the symbol table, and a pointer to the next one in the list */
typedef struct Symbol
{
    char name[MAX_LABEL_LENGTH + 1];
    int value;
    SymbolKind kind;
    int is_entry;
    struct Symbol *next;
} Symbol;

/* adds a new symbol, returns TRUE if added, or FALSE if the name already exists */
int add_symbol(const char *name, int value, SymbolKind kind);

/* looks up a symbol by name, returns a pointer to it, or NULL if not found */
Symbol *find_symbol(const char *name);

/* returns the first symbol in the table, for iterating over all of them via ->next, or NULL if the table is empty */
const Symbol *get_first_symbol(void);

/* marks an existing symbol as an entry, returns FALSE if the symbol was not found */
int mark_entry(const char *name);

/* adds icf to the value of every symbol whose kind is SYMBOL_DATA */
void update_data_symbols(int icf);

/* frees every symbol in the table, and empties the table for the next file */
void free_symbol_table(void);

#endif /* SYMBOL_TABLE_H */
