#include <stdint.h>
#ifndef _SEARCH_H_
#define _SEARCH_H_
#define NORMAL_SEARCH 1
#define HAS_SEARCH 0

/*helpers that actually call the search*/

char chr_simple(char c, char str_c[]);
char chr_uint8_terval(char c, char str_c[]);
char chr_list(char c, char str_c[]);

char searchID(uint8_t);
char searchFunction(uint8_t, uint8_t);
char searchNormalAddress(uint8_t, uint16_t);

#if NORMAL_SEARCH
char searchNormalID(uint8_t);
char searchNormalFunction(uint8_t, uint8_t);

#elif HAS_SEARCH
char searchHasID(int);
char searchHasFunction(int, int);
#endif

#endif
