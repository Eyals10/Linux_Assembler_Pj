/* preassembler.h - expands macros in a source file before the real assembly passes run */

#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

/* reads input_filename, expands every macro, and writes the result to output_filename */
/* the caller must call errors_start_file for this input file before calling this */
/* returns TRUE if the file had no macro-related errors, or FALSE if it did */
int run_preassembler(const char *input_filename, const char *output_filename);

#endif /* PREASSEMBLER_H */
