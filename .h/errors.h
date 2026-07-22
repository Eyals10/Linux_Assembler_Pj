/* errors.h - handles reporting of errors found while assembling one source file */

#ifndef ERRORS_H
#define ERRORS_H

/* starts error tracking for a new file: resets the error count to 0 and remembers the file name */
void errors_start_file(const char *filename);

/* reports one error: prints the file name, line number and message to stdout, and counts it */
void report_error(int line_number, const char *message);

/* returns TRUE if the current file had at least one error so far, or FALSE if it had none */
int had_errors(void);

#endif /* ERRORS_H */
