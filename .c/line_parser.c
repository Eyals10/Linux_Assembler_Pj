/* line_parser.c - implementation of the line parsing functions, see line_parser.h for details */

#include <string.h>
#include <ctype.h>
#include "constants.h"
#include "num_utils.h"
#include "line_parser.h"

/* moves a pointer forward past any spaces or tabs */
static const char *skip_whitespace(const char *text)
{
    while (*text == ' ' || *text == '\t')
    {
        text++;
    }
    return text;
}

/* looks at a line and decides which of the four statement types it is */
LineType classify_line(const char *line)
{
    const char *start;
    const char *scan;

    start = skip_whitespace(line);

    if (*start == '\0' || *start == '\n')
    {
        return LINE_EMPTY;
    }

    if (*start == ';')
    {
        return LINE_COMMENT;
    }

    /* scan the first word to see if it ends with ':' before any whitespace, meaning there is a label here */
    scan = start;
    while (*scan != '\0' && *scan != '\n' && *scan != ' ' && *scan != '\t' && *scan != ':')
    {
        scan++;
    }

    if (*scan == ':')
    {
        scan++;
        scan = skip_whitespace(scan);
    }
    else
    {
        scan = start;
    }

    if (*scan == '.')
    {
        return LINE_DIRECTIVE;
    }

    return LINE_INSTRUCTION;
}

/* checks for a label at the start of line, copies its raw text into out_label, and points out_rest past it, returns TRUE if a label was found */
int extract_label(const char *line, char *out_label, const char **out_rest)
{
    const char *start;
    const char *scan;
    int length;
    int index;

    /* this repeats the label-scan in classify_line on purpose */
    /* classify_line only peeks past the label, this one actually copies the label text out, so we keep them separate */
    start = skip_whitespace(line);
    scan = start;

    while (*scan != '\0' && *scan != '\n' && *scan != ' ' && *scan != '\t' && *scan != ':')
    {
        scan++;
    }

    if (*scan != ':')
    {
        *out_rest = start;
        return FALSE;
    }

    length = scan - start;
    for (index = 0; index < length; index++)
    {
        out_label[index] = start[index];
    }
    out_label[length] = '\0';

    scan++;
    *out_rest = skip_whitespace(scan);

    return TRUE;
}

/* returns TRUE if name is a legal label: starts with a letter, then letters/digits only, at most MAX_LABEL_LENGTH characters */
int is_valid_label_name(const char *name)
{
    int length;
    int index;

    length = strlen(name);

    if (length == 0 || length > MAX_LABEL_LENGTH)
    {
        return FALSE;
    }

    if (!isalpha((unsigned char) name[0]))
    {
        return FALSE;
    }

    for (index = 1; index < length; index++)
    {
        if (!isalnum((unsigned char) name[index]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/* returns TRUE if text is a legal register operand ("$" followed by digits 0-31), and fills *out_reg_num with the register number */
int is_valid_register(const char *text, int *out_reg_num)
{
    int value;

    if (text[0] != '$')
    {
        return FALSE;
    }

    /* require a plain digit right after the '$', so a signed number like "$-5" is not mistaken for a register */
    if (!isdigit((unsigned char) text[1]))
    {
        return FALSE;
    }

    if (!parse_integer(text + 1, &value))
    {
        return FALSE;
    }

    if (value < 0 || value > MAX_REGISTER_NUMBER)
    {
        return FALSE;
    }

    *out_reg_num = value;
    return TRUE;
}

/* returns TRUE if text is a legal quoted string of printable characters, and copies its contents (without the quotes) into out_string */
int is_valid_string_literal(const char *text, char *out_string)
{
    int length;
    int index;

    length = strlen(text);

    if (length < 2 || text[0] != '"' || text[length - 1] != '"')
    {
        return FALSE;
    }

    /* first make sure every character between the quotes is printable */
    for (index = 1; index < length - 1; index++)
    {
        if (!isprint((unsigned char) text[index]))
        {
            return FALSE;
        }
    }

    /* only copy the text out once we know the whole thing is valid */
    for (index = 1; index < length - 1; index++)
    {
        out_string[index - 1] = text[index];
    }
    out_string[length - 2] = '\0';

    return TRUE;
}

/* splits text into trimmed, comma-separated operands, fills operands and *out_count, returns FALSE if the commas are placed illegally */
int split_operands(const char *text, char operands[MAX_OPERANDS][MAX_LINE_LENGTH + 1], int *out_count)
{
    const char *start;
    const char *end;
    const char *comma;
    int count;
    int length;
    int index;

    start = skip_whitespace(text);

    /* an empty (or all whitespace) operand list is valid, e.g. for hlt */
    if (*start == '\0' || *start == '\n')
    {
        *out_count = 0;
        return TRUE;
    }

    count = 0;

    while (TRUE)
    {
        /* find the end of the current operand: either the next comma, or the end of the text */
        comma = start;
        while (*comma != '\0' && *comma != '\n' && *comma != ',')
        {
            comma++;
        }

        /* trim trailing whitespace off the operand by moving end backwards from the comma */
        end = comma;
        while (end > start && (*(end - 1) == ' ' || *(end - 1) == '\t'))
        {
            end--;
        }

        length = end - start;
        if (length == 0)
        {
            return FALSE;
        }

        /* stop if we already have as many operands as we can hold */
        if (count >= MAX_OPERANDS)
        {
            return FALSE;
        }

        for (index = 0; index < length; index++)
        {
            operands[count][index] = start[index];
        }
        operands[count][length] = '\0';
        count++;

        if (*comma != ',')
        {
            break;
        }

        start = skip_whitespace(comma + 1);
    }

    *out_count = count;
    return TRUE;
}

/* copies the first whitespace-separated word from text into out_word */
void get_first_word(const char *text, char *out_word)
{
    int index;

    text = skip_whitespace(text);

    index = 0;
    while (text[index] != '\0' && text[index] != ' ' && text[index] != '\t')
    {
        out_word[index] = text[index];
        index++;
    }

    out_word[index] = '\0';
}

/* returns a pointer to whatever is left in text after its first word and any whitespace after it */
const char *skip_first_word(const char *text)
{
    text = skip_whitespace(text);

    while (*text != '\0' && *text != ' ' && *text != '\t')
    {
        text++;
    }

    return skip_whitespace(text);
}
