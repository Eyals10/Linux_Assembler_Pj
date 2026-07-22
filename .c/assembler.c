/* assembler.c - the program's entry point, runs the whole pipeline for every input file */

#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "errors.h"
#include "preassembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output_writer.h"

/* copies filename into out_base, without its trailing ".as" extension if it has one */
static void get_base_name(const char *filename, char *out_base)
{
    int length;
    int extension_length;

    length = strlen(filename);
    extension_length = strlen(SOURCE_EXTENSION);

    if (length > extension_length && strcmp(filename + length - extension_length, SOURCE_EXTENSION) == 0)
    {
        strncpy(out_base, filename, length - extension_length);
        out_base[length - extension_length] = '\0';
    }
    else
    {
        strcpy(out_base, filename);
    }
}

/* copies base followed by extension into out_filename */
static void make_filename(const char *base, const char *extension, char *out_filename)
{
    strcpy(out_filename, base);
    strcat(out_filename, extension);
}

/* runs the full pipeline (pre-assembler, first pass, second pass, output files) on one input file */
static void process_file(const char *input_filename)
{
    char base_name[MAX_FILENAME_LENGTH + 1];
    char am_filename[MAX_FILENAME_LENGTH + 5];
    char ob_filename[MAX_FILENAME_LENGTH + 5];
    char ent_filename[MAX_FILENAME_LENGTH + 5];
    char ext_filename[MAX_FILENAME_LENGTH + 5];

    errors_start_file(input_filename);

    if (strlen(input_filename) > MAX_FILENAME_LENGTH)
    {
        printf("file name is too long: %s\n", input_filename);
        return;
    }

    get_base_name(input_filename, base_name);
    make_filename(base_name, EXPANDED_EXTENSION, am_filename);
    make_filename(base_name, OBJECT_EXTENSION, ob_filename);
    make_filename(base_name, ENTRIES_EXTENSION, ent_filename);
    make_filename(base_name, EXTERNALS_EXTENSION, ext_filename);

    if (!run_preassembler(input_filename, am_filename))
    {
        return;
    }

    if (!run_first_pass(am_filename))
    {
        return;
    }

    if (!run_second_pass(am_filename))
    {
        return;
    }

    write_object_file(ob_filename);
    write_entries_file(ent_filename);
    write_externals_file(ext_filename);
}

/* the program's entry point: treats every command line argument as an input file name */
int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++)
    {
        process_file(argv[i]);
    }

    return 0;
}
