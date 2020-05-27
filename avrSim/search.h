#ifndef _SEARCH_H_
#define _SEARCH_H_
#define NORMAL_SEARCH 0
#define HAS_SEARCH 1

int searchID(int);
int searchFunction(int, int);

#if NORMAL_SEARCH
int searchNormalID(int);
int searchNormalFunction(int, int);

#elif HAS_SEARCH
int searchHasID(int);
int searchHasFunction(int, int);
#endif

#endif
