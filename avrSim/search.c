#include "search.h"
#include "filter.h"
#define SLAVESZ 4


int searchID(int slaveID)
{
	int i;

	for(i = 0; i < SLAVESZ; i++)
	{
		if(filter[i].id == slaveID)
		{

			return 1;
		}
					
	}

	return -1;
}


int searchFunction(int slaveID, int fID)
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
