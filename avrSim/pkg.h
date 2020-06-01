#ifndef PKG_H
#define PKG_H

typedef struct pkg
{
	int id;
	int accepted_fct_length;
	int accepted_fct[8];
	int accepted_addr_length;
	int accepted_addr[8];

} pkg;

#endif
