/* output_writer.c - implementation of the output file writer, see output_writer.h for details */

#include <stdio.h>
#include "constants.h"
#include "symbol_table.h"
#include "memory_image.h"
#include "second_pass.h"
#include "output_writer.h"

/* writes the .ob file at filename: a header line, then the code and data images in hex rows */
void write_object_file(const char *filename)
{
    FILE *file;
    int code_length;
    int data_length;
    int row_start;
    int address;
    int row_bytes;
    int i;

    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("cannot open file %s\n", filename);
        return;
    }

    code_length = get_ic() - IC_START;
    data_length = get_dc();

    fprintf(file, "%d %d\n", code_length, data_length);

    /* code rows are always a full BYTES_PER_OUTPUT_ROW wide, since every instruction is 4 bytes */
    for (row_start = 0; row_start < code_length; row_start += BYTES_PER_OUTPUT_ROW)
    {
        address = IC_START + row_start;
        fprintf(file, "%04d", address);
        for (i = 0; i < BYTES_PER_OUTPUT_ROW; i++)
        {
            fprintf(file, " %02X", get_code_byte(row_start + i));
        }
        fprintf(file, "\n");
    }

    /* the last data row may have fewer than BYTES_PER_OUTPUT_ROW bytes */
    for (row_start = 0; row_start < data_length; row_start += BYTES_PER_OUTPUT_ROW)
    {
        address = get_ic() + row_start;
        row_bytes = data_length - row_start;
        if (row_bytes > BYTES_PER_OUTPUT_ROW)
        {
            row_bytes = BYTES_PER_OUTPUT_ROW;
        }

        fprintf(file, "%04d", address);
        for (i = 0; i < row_bytes; i++)
        {
            fprintf(file, " %02X", get_data_byte(row_start + i));
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

/* writes the .ent file at filename, but only if at least one symbol is marked as an entry */
void write_entries_file(const char *filename)
{
    FILE *file;
    const Symbol *symbol;
    int any_entries;

    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("cannot open file %s\n", filename);
        return;
    }

    any_entries = FALSE;
    for (symbol = get_first_symbol(); symbol != NULL; symbol = symbol->next)
    {
        if (symbol->is_entry)
        {
            fprintf(file, "%s %04d\n", symbol->name, symbol->value);
            any_entries = TRUE;
        }
    }

    fclose(file);

    if (!any_entries)
    {
        remove(filename);
    }
}

/* writes the .ext file at filename, but only if at least one external symbol was actually used */
void write_externals_file(const char *filename)
{
    FILE *file;
    const ExternalUsage *usage;
    int any_usage;

    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("cannot open file %s\n", filename);
        return;
    }

    any_usage = FALSE;
    for (usage = get_first_external_usage(); usage != NULL; usage = usage->next)
    {
        fprintf(file, "%s %04d\n", usage->symbol_name, usage->address);
        any_usage = TRUE;
    }

    fclose(file);

    if (!any_usage)
    {
        remove(filename);
    }
}
