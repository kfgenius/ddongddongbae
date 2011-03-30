#include "Hash.h"

#include <stdio.h>

#define HASH_NUMBER 37
#define HASH_SIZE 262139

static CHash* unique;

CHash* CHash::GetInstance()
{
	if(!unique)
	{
		unique = new CHash();
	}

	return unique;
}

int CHash::GetHash(char* sz)
{
	if(sz == NULL) return 0;
	unsigned int hashvalue = 0;
	for(int lp=0; sz[lp]; lp++)
		hashvalue = (hashvalue * HASH_NUMBER + sz[lp]) % HASH_SIZE;
	return hashvalue;
}