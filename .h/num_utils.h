/* num_utils.h - functions for parsing numbers and converting them into machine code bits */

#ifndef NUM_UTILS_H
#define NUM_UTILS_H

/* checks if text is a legal integer (optional sign then digits only), converts it into *out_value, returns TRUE or FALSE */
int parse_integer(const char *text, int *out_value);

/* returns TRUE if value fits in a signed field of num_bits bits, or FALSE if it is too big or too small */
int fits_in_bits(int value, int num_bits);

/* returns the raw bit pattern of value, kept to its lowest num_bits bits */
int to_bits(int value, int num_bits);

/* returns one byte (0-255) out of value, where byte_index 0 is the lowest byte */
int get_byte(int value, int byte_index);

#endif /* NUM_UTILS_H */
