#ifndef PKG_H
#define PKG_H
#include <stdint.h>

typedef struct pkg
{
	uint8_t id;
	int accepted_fct_length;
	uint8_t accepted_fct[8];
	int accepted_addr_length;
	uint16_t accepted_addr[8];

} pkg;

#endif
