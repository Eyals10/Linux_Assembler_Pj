/* preassembler.c - implementation of the pre-assembler stage, see preassembler.h for details */

#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "errors.h"
#include "opcode_table.h"
#include "macro_table.h"
#include "line_parser.h"
#include "preassembler.h"

/* generously large so a whole physical line always fits in one read, even a too-long test line */
#define RAW_LINE_BUFFER_SIZE 500

/* returns TRUE if line has nothing left in it but whitespace */
static int is_blank(const char *line)
{
    while (*line == ' ' || *line == '\t')
    {
        line++;
    }

    return (*line == '\0');
}

/* returns TRUE if word appears as a whole word anywhere in text, not just as its first word */
static int contains_word(const char *text, const char *word)
{
    char current_word[MAX_LINE_LENGTH + 1];
    const char *rest;

    rest = text;
    get_first_word(rest, current_word);

    while (current_word[0] != '\0')
    {
        if (strcmp(current_word, word) == 0)
        {
            return TRUE;
        }

        rest = skip_first_word(rest);
        get_first_word(rest, current_word);
    }

    return FALSE;
}

/* reads input_filename, expands every macro, and writes the result to output_filename */
int run_preassembler(const char *input_filename, const char *output_filename)
{
    FILE *input_file;
    FILE *output_file;
    char line[RAW_LINE_BUFFER_SIZE];
    char first_word[MAX_LINE_LENGTH + 1];
    char current_macro_name[MAX_LINE_LENGTH + 1];
    int line_number;
    int inside_macro;
    int length;
    const Macro *macro;
    const MacroLine *body_line;
    const char *after_mcroend;
    const char *after_mcro;
    const char *after_name;

    /* clear out any macros left over from a previous input file */
    free_macro_table();

    input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        printf("cannot open input file %s\n", input_filename);
        return FALSE;
    }

    output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        printf("cannot open output file %s\n", output_filename);
        fclose(input_file);
        return FALSE;
    }

    line_number = 0;
    inside_macro = FALSE;

    while (fgets(line, RAW_LINE_BUFFER_SIZE, input_file) != NULL)
    {
        line_number++;

        /* strip the trailing newline, if there is one, so line always holds just the content */
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n')
        {
            line[length - 1] = '\0';
            length--;
        }

        if (length > MAX_LINE_LENGTH)
        {
            report_error(line_number, "line is longer than 80 characters");
            continue;
        }

        get_first_word(line, first_word);

        /* while inside a macro definition, a line can only be mcroend or body content */
        if (inside_macro)
        {
            if (strcmp(first_word, "mcroend") == 0)
            {
                after_mcroend = skip_first_word(line);
                if (!is_blank(after_mcroend))
                {
                    report_error(line_number, "extra characters after mcroend");
                }

                inside_macro = FALSE;
                continue;
            }

            add_macro_line(current_macro_name, line);
            continue;
        }

        macro = find_macro(first_word);
        if (macro != NULL)
        {
            body_line = macro->body_head;
            while (body_line != NULL)
            {
                fprintf(output_file, "%s\n", body_line->text);
                body_line = body_line->next;
            }
            continue;
        }

        if (strcmp(first_word, "mcro") == 0)
        {
            after_mcro = skip_first_word(line);
            get_first_word(after_mcro, current_macro_name);
            after_name = skip_first_word(after_mcro);

            if (current_macro_name[0] == '\0')
            {
                report_error(line_number, "macro definition is missing a name");
                continue;
            }

            if (is_reserved_word(current_macro_name))
            {
                report_error(line_number, "macro name cannot be an instruction or directive name");
                continue;
            }

            if (!is_blank(after_name))
            {
                report_error(line_number, "extra characters after macro name");
                continue;
            }

            add_macro(current_macro_name);
            inside_macro = TRUE;
            continue;
        }

        if (strcmp(first_word, "mcroend") == 0)
        {
            report_error(line_number, "mcroend without a matching mcro");
            continue;
        }

        if (contains_word(line, "mcro") || contains_word(line, "mcroend"))
        {
            report_error(line_number, "mcro or mcroend must be the first word on its line");
            continue;
        }

        fprintf(output_file, "%s\n", line);
    }

    fclose(input_file);
    fclose(output_file);

    if (had_errors())
    {
        remove(output_filename);
        return FALSE;
    }

    return TRUE;
}
