#include "search.h"
#include "filter.h"
#include "has.h"

int slaves = sizeof(filter)/sizeof(struct pkg);

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


// search slave ID function
int searchID(int slaveID)
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
int searchFunction(int slaveID, int fID)
{

#if NORMAL_SEARCH
	return searchNormalFunction(slaveID, fID);

#elif HAS_SEARCH
	return searchHasFunction(slaveID, fID);
#endif

	// if reached, means no search algorithm selected, returns -1
	return -1;
}

#if NORMAL_SEARCH
int searchNormalID(int slaveID)
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

int searchNormalFunction(int slaveID, int fID)
{
	int i;
	int fCodes = sizeof(filter[slaveID - 1]) / sizeof(int);

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
int searchHasID(int slaveID)
{
	if(hID[slaveID] == 1)
	{
		return 1;
	}

	return -1;
}

int searchHasFunction(int slaveID, int fID)
{
	// check validity of function
	if(h[slaveID][fID] == 1)
	{
		return 1;
	}
	return -1;

}
#endif

