#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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

struct pdu
{
	int allowed_command; // 0x10
	char addr_interval[15]; // 40005, 40001 - 40005
	char val_interval[10]; // 0, 0-256. 258, 261, 263
};

struct adu
{
	char slaves[5];
	struct pdu vpdu;
};

struct adu filter[] = 
{
	{
		"1-3",  
		{0x10, "40001-40005", "0-256"}
	},
	{
		"12",
		{0x10, "40001", "254"}
	}

};

int chr_interval(char c, char str_c[])
{
	int lower_bound, upper_bound;
	sscanf(str_c, "%d-%d", &lower_bound, &upper_bound);
	int val = (int) c;
	if(val >= lower_bound && val <= upper_bound)
	{
		printf("pass!\n");
		return 1;
	}
	printf("denied!\n");
	return 0;
}

int alldigit(char a[], int n)
{
	for(int i = 0; i < n; i++)
	{
		if(isdigit(a[i]) == 0)
		{
			return -1;
		}
	}
	
	return 1;
}

int main(int argc, char **argv)

{
	//char a[] = "40001-40005";
	//char *p = &a[0];
	//int lower_bound, higher_bound;
	//sscanf(a, "%d-%d", &lower_bound, &higher_bound);
	//printf("(Lower bound, Upper bound): (%d, %d)", lower_bound, higher_bound);

	char allowed_slaves[5];
	int num_slaves = sizeof(filter)/sizeof(filter[0]);
	for (int i = 0; i < num_slaves; i++)
	{
		printf("%s\n", filter[i].slaves);
	}
	int is_interval = 0, is_simple = 0, is_list = 0;

	printf("num_slaves = %d\n", num_slaves);	

	char c = 9;
	c = atoi(argv[1]);
	for (int i = 0; i < num_slaves; i++)
	{
	//	printf("%s", filter[i].slaves);
		if(strchr(filter[i].slaves, '-'))
		{
			is_interval = 1;
			chr_interval(c, filter[i].slaves);
			
		}
		else if(strchr(filter[i].slaves, ','))
		{
			is_list = 1;
		}
		else if(alldigit(filter[i].slaves, strlen(filter[i].slaves)) > 0)
		{
			is_simple = 1; 
		}
		
		if(is_interval)
		{
			printf("interval\n");
			is_interval = 0;
		}

		if(is_list)
		{
			printf("list\n");
			is_list = 0;
		}
		if(is_simple)
		{
			printf("simple\n");
			is_simple = 0;
		}
	}


	return 0;
}
