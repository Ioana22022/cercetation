#include "filter.h"
#include "has.h"
#define SLAVESZ 4;

int h[255][8];
int hID[255];

void insertH()
{
	int i, j;
	int fCodes = 0;

	for (i = 0; i < 4; i++)
	{
		fCodes = sizeof(filter[i].accepted_fct) / sizeof(int);
		for(j = 0; j < fCodes; j++)
		{
			h[filter[i].id][filter[i].accepted_fct[j]] = 1;
		}
	}
}

void insertHId()
{
	int i;
	for(i = 0 ; i < 4; i++)
	{
		hID[filter[i].id] = 1;
	}
}

