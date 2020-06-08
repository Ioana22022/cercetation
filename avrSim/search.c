#include "search.h"
#include "filter.h"
#include "has.h"
#include <string.h>
#include <stdlib.h> 

int slaves = sizeof(filter)/sizeof(struct pkg);
/*
int chr_simple(char c, char str_c[])
{
	int val = (int) c;
	if(val == atoi(str_c))
	{
		return 1;
	}
	return -1;
}

int chr_interval(char c, char str_c[])
{
	int lower_bound, upper_bound;
	sscanf(str_c, "%d-%d", &lower_bound, &upper_bound);
	int val = (int) c;
	if(val >= lower_bound && val <= upper_bound)
	{
		return 1;
	}

	return -1;
}

int chr_list(char c, char str_c[])
{


	int numbers[10];
	int i = 0;
	char *curr;
	while((curr = strtok(i ? NULL : str_c, ",")) != NULL)
	{
		numbers[i++] = atoi(curr);
	}

	int j = 0;
	int val = (int) c;

	for(j = 0; j < i; j++)
	{
		if(numbers[j] == val)
		{
			return 1;
		}

	}

	return -1;
}
*/

// search slave ID function
char searchID(uint8_t slaveID)
{
#if NORMAL_SEARCH
	return searchNormalID(slaveID);

#elif HAS_SEARCH
	return searchHasID(slaveID);

#endif

	// if reached, then no search algorithm selected, returns -1
	return -1;
}

// search function code function
char searchFunction(uint8_t slaveID, uint8_t fID)
{

#if NORMAL_SEARCH
	return searchNormalFunction(slaveID, fID);

#elif HAS_SEARCH
	return searchHasFunction(slaveID, fID);
#endif

	// if reached, means no search algorithm selected, returns -1
	return -1;
}
//---------------------------------------------------------
char searchNormalAddress(uint8_t slaveID, uint16_t address)
{
	int i = 0;
	int length = filter[slaveID - 1].accepted_addr_length;
	//--------------------------------
	/*uint8_t *paddr;
	paddr = &address;
	USART2_transmit(paddr[0]);
	USART2_transmit(paddr[1]);
	*/
	//uint8_t exp = 0;
	//exp = (filter[slaveID -1].accepted_addr[i] == address);
	//USART2_transmit(exp);
	//-------------------------------
	
	for(i = 0; i < length; i++)
	{
		if(filter[slaveID -1].accepted_addr[i] == address)
		{
			return 1;
		}
	}
	return -1;
}
//---------------------------------------------------------
#if NORMAL_SEARCH
char searchNormalID(uint8_t slaveID)
{
	int i;

	for(i = 0; i < slaves; i++)
	{
		if(filter[i].id == slaveID)
		{

			return 1;
		}
					
	}

	return -1;
}

char searchNormalFunction(uint8_t slaveID, uint8_t fID)
{
	int i;
	int fCodes = filter[slaveID - 1].accepted_fct_length;

	for(i = 0; i < fCodes; i++)
	{
		if(filter[slaveID - 1].accepted_fct[i] == fID)
		{
			return 1;
		}
	}

	return -1;
}
#endif

#if HAS_SEARCH
char searchHasID(int slaveID)
{
	if(hID[slaveID] == 1)
	{
		return 1;
	}

	return -1;
}

 searchHasFunction(int slaveID, int fID)
{
	// check validity of function
	if(h[slaveID][fID] == 1)
	{
		return 1;
	}
	return -1;

}
#endif

