#include <stdio.h>

/*
 *
 *
 *
 * {SLAVE or SLAVES,
    {ALLOWED COMMAND 1, ADDRESS or ADDRESS INTERVAL, VALUE INTERVAL},
    {ALLOWED COMMAND 2, ADDRESS or ADDRESS INTERVAL, VALUE INTERVAL},
    {ALLOWED COMMAND 3, ADDRESS or ADDRESS INTERVAL, VALUE INTERVAL},
}

SLAVES poate fi 
1 sau 1,2,3 sau 1-3 sau 1-2, 3-10, 12, 14

ALLOWED COMMAND: WRITE_HOLDING_REGISTERS sau functia in numere (16) sau in hex (0x10) (cred ca asta e)
ADDRESS or ADDRESS interval - 40005 sau 40001-40005
VALUE sau VALUE interval - 0, 0-256, 258, 261, 263 sau * (anything)

*/



int main()

{
	char a[] = "40001-40005";
	//char *p = &a[0];
	int lower_bound, higher_bound;
	sscanf(a, "%d-%d", &lower_bound, &higher_bound);
	printf("(Lower bound, Upper bound): (%d, %d)", lower_bound, higher_bound);
	return 0;
}
