/* num_utils.c - implementation of the number utility functions, see num_utils.h for details */

#include <limits.h>
#include "constants.h"
#include "num_utils.h"

/* checks if text is a legal integer (optional sign then digits only), converts it into *out_value, returns TRUE or FALSE */
int parse_integer(const char *text, int *out_value)
{
    int  index = 0;
    int sign = 1;
    long value = 0;
    int has_digits = FALSE;
    
    if (text[0] == '+' || text[0] == '-')
    {
        if (text[0] == '-')
        {
            sign = -1;
        }
        index = 1;
    }

    while (text[index] != '\0')
    {
        if (text[index] < '0' || text[index] > '9')
        {
            return FALSE;
        }

        value = value * 10 + (text[index] - '0');
        has_digits = TRUE;
        index++;

        /* stop as soon as the digits so far exceed INT_MAX; this only works because long is wider than int here */
        if (value > INT_MAX)
        {
            return FALSE;
        }
    }

    if (!has_digits)
    {
        return FALSE;
    }

    *out_value = (int) value * sign;
    return TRUE;
}

/* returns TRUE if value fits in a signed field of num_bits bits, or FALSE if it is too big or too small */
int fits_in_bits(int value, int num_bits)
{
    int min_value;
    int max_value;

    /* a 32-bit field already covers every value an int can hold, so nothing to check */
    if (num_bits >= 32)
    {
        return TRUE;
    }

    max_value = (1 << (num_bits - 1)) - 1;
    min_value = -(1 << (num_bits - 1));

    if (value < min_value || value > max_value)
    {
        return FALSE;
    }

    return TRUE;
}

/* returns the raw bit pattern of value, kept to its lowest num_bits bits */
int to_bits(int value, int num_bits)
{
    unsigned int mask;

    /* a 32-bit field already covers every bit of an int, no masking needed */
    if (num_bits >= 32)
    {
        return value;
    }

    /* mask uses 1u (unsigned) instead of 1, so the shift stays well defined even when num_bits is 31 */
    /* this also assumes int is two's complement, which is true for gcc, so masking a negative value still works */
    mask = (1u << num_bits) - 1u;
    return value & mask;
}

/* returns one byte (0-255) out of value, where byte_index 0 is the lowest byte */
int get_byte(int value, int byte_index)
{
    /* works for negative values too: & 0xFF only keeps bits that came from value, never the shifted-in fill bits */
    return (value >> (byte_index * 8)) & 0xFF;
}
