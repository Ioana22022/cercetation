#ifndef _SEARCH_H_
#define _SEARCH_H_
#define NORMAL_SEARCH 1
#define HAS_SEARCH 0

/*helpers that actually call the search*/

int chr_simple(char c, char str_c[]);
int chr_interval(char c, char str_c[]);
int chr_list(char c, char str_c[]);

int searchID(int);
int searchFunction(int, int);
int searchNormalAddress(int, int);

#if NORMAL_SEARCH
int searchNormalID(int);
int searchNormalFunction(int, int);

#elif HAS_SEARCH
int searchHasID(int);
int searchHasFunction(int, int);
#endif

#endif
