/* second_pass.h - the second pass over the expanded source file, see the project's algorithm for details */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "constants.h"

/* one recorded usage of an external symbol inside a J-type instruction, and a pointer to the next one */
typedef struct ExternalUsage
{
    char symbol_name[MAX_LABEL_LENGTH + 1];
    int address;
    struct ExternalUsage *next;
} ExternalUsage;

/* runs the second pass over filename (the expanded .am file), completing every deferred instruction */
/* the caller must call errors_start_file for this input file before calling this, and run_first_pass must have already succeeded on it */
/* returns TRUE if the file had no errors so far, or FALSE if it did */
int run_second_pass(const char *filename);

/* returns the first recorded external symbol usage, for iterating over all of them, or NULL if there are none */
const ExternalUsage *get_first_external_usage(void);

#endif /* SECOND_PASS_H */
