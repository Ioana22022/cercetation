#include <stdint.h>
#ifndef _SEARCH_H_
#define _SEARCH_H_
#define NORMAL_SEARCH 1
#define HAS_SEARCH 0

/*helpers that actually call the search*/

int chr_simple(char c, char str_c[]);
int chr_uint8_terval(char c, char str_c[]);
int chr_list(char c, char str_c[]);

int searchID(uint8_t);
int searchFunction(uint8_t, uint8_t);
int searchNormalAddress(uint8_t, uint16_t);

#if NORMAL_SEARCH
int searchNormalID(uint8_t);
int searchNormalFunction(uint8_t, uint8_t);

#elif HAS_SEARCH
int searchHasID(int);
int searchHasFunction(int, int);
#endif

#endif
