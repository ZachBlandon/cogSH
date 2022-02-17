#ifndef COGSH_H
#define COGSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aliasTable.c"
#include "varTable.c"
#include "prectok.c"
#include "findStr.c"
#include "../apps/myCP.c"
#include "../apps/myLS.c"


//find find_str in fp and place whatever comes after it into rtnBuffer
void searchFile(FILE* fp, char** rtnBuffer, const char* find_str)
{
	char temp[SIZE]; //temp array to hold a line from fp

	while (fgets(temp, SIZE, fp) != NULL) //put line from fp into temp
	{
		if (findStr(temp, rtnBuffer, find_str, 0))
		{
			return;
		}
	}
}

#endif
