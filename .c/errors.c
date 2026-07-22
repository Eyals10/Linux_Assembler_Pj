/* errors.c - implementation of the error reporting module, see errors.h for details */

#include <stdio.h>
#include "constants.h"
#include "errors.h"

/* the name of the source file currently being assembled */
static const char *current_filename = "(no file)";

/* how many errors were found so far in the current file */
static int error_count = 0;

/* starts error tracking for a new file: resets the error count to 0 and remembers the file name */
void errors_start_file(const char *filename)
{
    current_filename = filename;
    error_count = 0;
}

/* reports one error: prints the file name, line number and message to stdout, and counts it */
void report_error(int line_number, const char *message)
{
    printf("%s:%d: error: %s\n", current_filename, line_number, message);
    error_count++;
}

/* returns TRUE if the current file had at least one error so far, or FALSE if it had none */
int had_errors(void)
{
    if (error_count > 0)
    {
        return TRUE;
    }
    return FALSE;
}
