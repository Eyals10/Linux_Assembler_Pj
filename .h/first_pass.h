/* first_pass.h - the first pass over the expanded source file, see the project's algorithm for details */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

/* runs the first pass over filename (the expanded .am file), building the symbol table and memory image */
/* the caller must call errors_start_file for this input file before calling this */
/* returns TRUE if the file had no errors so far, or FALSE if it did */
int run_first_pass(const char *filename);

#endif /* FIRST_PASS_H */
