/* output_writer.h - writes the .ob, .ent, and .ext output files after both passes succeed */

#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

/* writes the .ob file at filename: a header line, then the code and data images in hex rows */
void write_object_file(const char *filename);

/* writes the .ent file at filename, but only if at least one symbol is marked as an entry */
void write_entries_file(const char *filename);

/* writes the .ext file at filename, but only if at least one external symbol was actually used */
void write_externals_file(const char *filename);

#endif /* OUTPUT_WRITER_H */
