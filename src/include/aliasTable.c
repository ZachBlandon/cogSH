#ifndef ALIASTABLE_C
#define ALIASTABLE_C

#include "uthash.h"

#define MAX_ALIAS 100
#define ALIAS_SIZE 32


//struct to hold alias names and values and facilitate quick and easy CRUD
struct aliasStruct 
{
	char name[ALIAS_SIZE]; //name of alias
	char value[SIZE];	  //value that alias holds
	UT_hash_handle hh;	  // required by uthash to make struct hashable
};


struct aliasStruct *aliasTable = NULL; //base pointer to hash table


//delete an alias with char* name in aliasTable
int deleteAlias(char* name)
{
	struct aliasStruct* as;

	HASH_FIND_STR(aliasTable, name, as); //find alias
	
	if (as != NULL)						//if alias exists
	{
		HASH_DEL(aliasTable, as);		//delete alias from table
		free(as);						//free struct memory
		return 1;					//returns 1 if alias deleted
	}
	
	return 0;						//returns 0 if no alias found
}


//add an alias to aliasTable
int addAlias(char* name, char* value)
{
	int rtn = deleteAlias(name); //if alias with char* name is found then delete it	
	
	struct aliasStruct* as; //create new aliasStruct and place name and values into it
	as = (struct aliasStruct*)malloc(sizeof(struct aliasStruct));
	strcpy(as->name, name);
	strcpy(as->value, value);

	HASH_ADD_STR(aliasTable, name, as); //add to hash table
	
	return rtn; //returns 1 if alias overwritten, 0 if not
}


//find an alias with char* name in aliasTable
struct aliasStruct* findAlias(char* name)
{
	struct aliasStruct* as;
	HASH_FIND_STR(aliasTable, name, as); //find alias and place aliasStruct into as
	
	//return struct if alias with char* name is found, else returns NULL
	return as;
}


//delele all elements from aliasTable and free used memory
void deleteAllAlias()
{
	struct aliasStruct* as, *temp;
	
	HASH_ITER(hh, aliasTable, as, temp) //uthash macro to iterate through aliasTable
	{
		HASH_DEL(aliasTable, as);	//delete element from aliasTable
		free(as);					//free struct memory
	}
}


#endif
