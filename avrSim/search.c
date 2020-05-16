#include "search.h"
#include "filter.h"
#include "has.h"
#define NORMAL_SEARCH 0
#define HAS_SEARCH 1

int slaves = sizeof(filter)/sizeof(struct pkg);

int searchID(int slaveID)
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

#if NORMAL_SEARCH
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
int searchHasFunction(int slaveID, int fID)
{
	// populate h[][]
	insertH();

	// check validity of function
	if(h[slaveID][fID] == 1)
	{
		return 1;
	}
	return -1;

}
#endif

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
